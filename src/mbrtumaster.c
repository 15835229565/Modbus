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
/**     V1.5.0  2018-01-16          ���Ҿ�            �޸�ָ��洢������     **/
/**                                                                          **/
/******************************************************************************/

#include "mbrtumaster.h"

/*�������վ״̬��������Ϣ������Ȧ״̬λ0x01������*/
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

/*��������CreateAccessSlaveCommand�����ɷ��ʷ�����������*/
/*������ObjAccessInfo objInfo,Ҫ���ɷ�������Ķ�����Ϣ*/
/*      void *dataList,д�������б��Ĵ���Ϊuint16_t���ͣ�״̬��Ϊbool����*/
/*      uint8_t *commandBytes,���ɵ������б�*/
/*����ֵ��uint16_t�����ɵ�����ĳ���*/
uint16_t CreateAccessSlaveCommand(ObjAccessInfo objInfo,void *dataList,uint8_t *commandBytes)
{
  uint16_t commandLength=0;
  /*���ɶ���������������������0x01��0x02��0x03��0x04,�����8���ֽ�*/
  if((objInfo.functionCode>=ReadCoilStatus)&&(objInfo.functionCode <= ReadInputRegister))
  {
    commandLength=SyntheticReadWriteSlaveCommand(objInfo,NULL,NULL,commandBytes);
  }

  /*����Ԥ�÷�������������������0x05,0x0F,������淢�����ݶ���*/
  if((objInfo.functionCode==WriteSingleCoil)||(objInfo.functionCode==WriteMultipleCoil))
  {
    bool *statusList=(bool*)dataList;
    commandLength=SyntheticReadWriteSlaveCommand(objInfo,statusList,NULL,commandBytes);
  }
  
  /*����Ԥ�÷�������������������0x06,0x10,������淢�����ݶ���*/
  if((objInfo.functionCode==WriteSingleRegister)||(objInfo.functionCode==WriteMultipleRegister))
  {
    uint16_t *registerList=(uint16_t*)dataList;
    commandLength=SyntheticReadWriteSlaveCommand(objInfo,NULL,registerList,commandBytes);
  }

  return commandLength;
}

/*�����յ��ķ�������Ӧ��Ϣ*/
/*uint8_t *recievedMessage,���յ�����Ϣ�б�*/
/*uint8_t *command,���͵Ķ����������ΪNULL���������б��в���*/
void ParsingSlaveRespondMessage(uint8_t *recievedMessage,uint8_t *command)
{
  /*������Ƕ������ķ�����Ϣ����Ҫ����*/
  if(recievedMessage[1]>0x04)
  {
    return;
  }

  if(command==NULL)
  {
    return;
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

/*���յ�������Ϣ���ж��Ƿ��Ƿ��������б�������ķ�����Ϣ��*/
/*��������ѷ��������б���ɾ�������������������������Ϣ*/
int FindCommandForRecievedMessage(uint8_t *recievedMessage,uint8_t (*commandList)[8],uint16_t commandNumber)
{
  int cmdIndex=-1;
  
    for(int i=0;i<commandNumber;i++)
    {
      if(CheckMessageAgreeWithCommand(recievedMessage,commandList[i])==true)
      {
        cmdIndex=i;
        break;
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
    uint16_t quantity=(uint16_t)command[4];
    quantity=(quantity<<8)+(uint16_t)command[5];
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