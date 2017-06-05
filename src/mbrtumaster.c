/******************************************************************************/
/** ģ�����ƣ�ModbusͨѶ                                                     **/
/** �ļ����ƣ�modbusrtumaster.c                                              **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺��������Modbus RTU��վ������Լ�����                           **/
/**           1�����ͷ�������                                                **/
/**           2������������Ϣ                                                **/
/**           3�����ݷ�����Ϣ�޸�����                                        **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2016-04-17          ���Ҿ�            �����ļ�               **/
/**                                                                          **/
/******************************************************************************/

#include "mbrtumaster.h"

#define SlaveNumber 1

/*�����д��վ���������б�*/
uint8_t commandMasterList[SlaveNumber][8];
uint16_t commandMasterIndex=0;

/*������������ѷ��������б����������*/
static void AddCommandBytesToList(uint8_t *commandBytes);
/*���յ�������Ϣ���ж��Ƿ��Ƿ��������б�������ķ�����Ϣ��*/
static int FindCommandForRecievedMessage(uint8_t *recievedMessage);
/*�������վ״̬��������Ϣ������Ȧ״̬λ0x012������*/
static void HandleReadCoilStatusRespond(uint8_t *receivedMesasage,uint16_t startAddress,uint16_t quantity);
/*�������վ״̬��������Ϣ��������״̬λ0x02������*/
static void HandleReadInputStatusRespond(uint8_t *receivedMesasage,uint16_t startAddress,uint16_t quantity);
/*�������վ�Ĵ���ֵ�ķ�����Ϣ�������ּĴ���0x03�����룩*/
static void HandleReadHoldingRegisterRespond(uint8_t *receivedMesasage,uint16_t startAddress,uint16_t quantity);
/*�������վ�Ĵ���ֵ�ķ�����Ϣ��������Ĵ���0x04������*/
static void HandleReadInputRegisterRespond(uint8_t *receivedMesasage,uint16_t startAddress,uint16_t quantity);
/*�жϽ��յ�����Ϣ�Ƿ��Ƿ�������ķ�����Ϣ*/
static bool CheckMessageAgreeWithCommand(uint8_t *recievedMessage,uint8_t *command);

void (*HandleSlaveRespond[])(uint8_t *,uint16_t,uint16_t)={HandleReadCoilStatusRespond,
                                                           HandleReadInputStatusRespond,
                                                           HandleReadHoldingRegisterRespond,
                                                           HandleReadInputRegisterRespond};

/*���ɷ��ʷ�����������*/
uint16_t CreateAccessSlaveCommand(ObjAccessInfo objInfo,void *dataList,uint8_t *commandBytes)
{
  uint16_t commandLength=0;
  /*���ɶ���������������������0x01��0x02��0x03��0x04,�����12���ֽ�*/
  if((objInfo.functionCode>=ReadCoilStatus)&&(objInfo.functionCode <= ReadInputStatus))
  {
    commandLength=SyntheticReadWriteSlaveCommand(objInfo,NULL,NULL,commandBytes);
    
    AddCommandBytesToList(commandBytes);        /*��¼���͵Ķ�����*/
  }

  /*����Ԥ�÷�������������������0x05,0x0F,������淢�����ݶ���*/
  if((objInfo.functionCode==WriteSingleCoil)&&(objInfo.functionCode==WriteMultipleCoil))
  {
    bool *statusList=(bool*)dataList;
    commandLength=SyntheticReadWriteSlaveCommand(objInfo,statusList,NULL,commandBytes);
  }
  
  /*����Ԥ�÷�������������������0x06,0x10,������淢�����ݶ���*/
  if((objInfo.functionCode==WriteSingleRegister)&&(objInfo.functionCode==WriteMultipleRegister))
  {
    uint16_t *registerList=(uint16_t*)dataList;
    commandLength=SyntheticReadWriteSlaveCommand(objInfo,NULL,registerList,commandBytes);
  }

  return commandLength;
}

/*�����յ��ķ�������Ӧ��Ϣ*/
void ParsingSlaveRespondMessage(uint8_t *recievedMessage,uint8_t *command)
{
  /*������Ƕ������ķ�����Ϣ����Ҫ����*/
  if(recievedMessage[1]>0x04)
  {
    return;
  }

  if(command==NULL)
  {
    /*�жϽ��յ�����Ϣ�Ƿ�����Ӧ������*/
    int cmdIndex=FindCommandForRecievedMessage(recievedMessage);
  
    if((cmdIndex<0))
    {
      return;
    }
    command=commandMasterList[cmdIndex];
  }
  
  FunctionCode fuctionCode=(FunctionCode)recievedMessage[1];
  uint16_t startAddress=(uint16_t)command[2];
  startAddress=(startAddress<<8)+(uint16_t)command[3];
  uint16_t quantity=(uint16_t)command[4];
  quantity=(quantity<<8)+(uint16_t)command[5];
  
  if((fuctionCode>=ReadCoilStatus)&&(fuctionCode<=ReadInputRegister))
  {
    HandleSlaveRespond[fuctionCode-1](recievedMessage,startAddress,quantity);
  }
}

/*������������ѷ��������б����������*/
static void AddCommandBytesToList(uint8_t *commandBytes)
{
  if(commandMasterIndex>SlaveNumber)
  {
    commandMasterIndex=0;
  }
  
  for(int i=0;i<8;i++)
  {
    commandMasterList[commandMasterIndex][i]=commandBytes[i];
  }

  commandMasterIndex++;
}

/*���յ�������Ϣ���ж��Ƿ��Ƿ��������б�������ķ�����Ϣ��*/
/*��������ѷ��������б���ɾ�������������������������Ϣ*/
static int FindCommandForRecievedMessage(uint8_t *recievedMessage)
{
  int cmdIndex=-1;
  
  if(CheckMessageAgreeWithCommand(recievedMessage,commandMasterList[commandMasterIndex])==true)
  {
    cmdIndex=commandMasterIndex;
  }
  else
  {
    for(int i=0;i<SlaveNumber;i++)
    {
      if(CheckMessageAgreeWithCommand(recievedMessage,commandMasterList[i])==true)
      {
        cmdIndex=i;
        break;
      }
    }
  }
  return cmdIndex;
}
        
/*�жϽ��յ�����Ϣ�Ƿ��Ƿ�������ķ�����Ϣ*/
static bool CheckMessageAgreeWithCommand(uint8_t *recievedMessage,uint8_t *command)
{
  bool aw=false;
  
  if((recievedMessage[0]==command[0])&&(recievedMessage[1]==command[1]))
  {
    uint16_t quantity=(uint16_t)commandMasterList[commandMasterIndex][4];
    quantity=(quantity<<8)+(uint16_t)commandMasterList[commandMasterIndex][5];
    uint8_t bytescount=0;
    if((recievedMessage[1]==ReadCoilStatus)||(recievedMessage[1]==ReadInputStatus))
    {
      bytescount=(uint8_t)((quantity-1)/8+1);
    }
    
    if((recievedMessage[1]==ReadHoldingRegister)||(recievedMessage[1]==ReadInputRegister))
    {
      bytescount=quantity*2;
    }
    
    if(recievedMessage[2]==bytescount)
    {
      aw=true;
    }
  }
  
  return aw;
}
/*�������վ״̬��������Ϣ������Ȧ״̬λ0x012������*/
static void HandleReadCoilStatusRespond(uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  bool coilStatus[256];
  
  TransformClientReceivedData(receivedMessage,quantity,coilStatus,NULL);
  
  UpdateCoilStatus(startAddress,quantity,coilStatus);
}

/*�������վ״̬��������Ϣ��������״̬λ0x02������*/
static void HandleReadInputStatusRespond(uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  bool inputStatus[256];
  
  TransformClientReceivedData(receivedMessage,quantity,inputStatus,NULL);
  
  UpdateInputStatus(startAddress,quantity,inputStatus);
}

/*�������վ�Ĵ���ֵ�ķ�����Ϣ�������ּĴ���0x03�����룩*/
static void HandleReadHoldingRegisterRespond(uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  uint16_t holdingRegister[125];
  
  TransformClientReceivedData(receivedMessage,quantity,NULL,holdingRegister);
  
  UpdateHoldingRegister(startAddress,quantity,holdingRegister);
}

/*�������վ�Ĵ���ֵ�ķ�����Ϣ��������Ĵ���0x04������*/
static void HandleReadInputRegisterRespond(uint8_t *receivedMessage,uint16_t startAddress,uint16_t quantity)
{
  uint16_t inputRegister[125];
  
  TransformClientReceivedData(receivedMessage,quantity,NULL,inputRegister);
  
  UpdateInputResgister(startAddress,quantity,inputRegister);
}

/*********** (C) COPYRIGHT 1999-2016 Moonan Technology *********END OF FILE****/