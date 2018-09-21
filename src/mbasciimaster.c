/******************************************************************************/
/** ģ�����ƣ�ModbusͨѶ                                                     **/
/** �ļ����ƣ�mbasciimaster.c                                                **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺��������Modbus ASCII��վ������Լ�����                         **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2018-09-11          ���Ҿ�            �����ļ�               **/
/**                                                                          **/
/******************************************************************************/

#include "mbasciimaster.h"

/*�������վ״̬��������Ϣ������Ȧ״̬λ0x01������*/
static void HandleReadCoilStatusRespond(uint8_t *receivedMesasage, uint16_t startAddress, uint16_t quantity);
/*�������վ״̬��������Ϣ��������״̬λ0x02������*/
static void HandleReadInputStatusRespond(uint8_t *receivedMesasage, uint16_t startAddress, uint16_t quantity);
/*�������վ�Ĵ���ֵ�ķ�����Ϣ�������ּĴ���0x03�����룩*/
static void HandleReadHoldingRegisterRespond(uint8_t *receivedMesasage, uint16_t startAddress, uint16_t quantity);
/*�������վ�Ĵ���ֵ�ķ�����Ϣ��������Ĵ���0x04������*/
static void HandleReadInputRegisterRespond(uint8_t *receivedMesasage, uint16_t startAddress, uint16_t quantity);
/*�жϽ��յ�����Ϣ�Ƿ��Ƿ�������ķ�����Ϣ*/
static bool CheckMessageAgreeWithCommand(uint8_t *recievedMessage, uint8_t *command);

void (*HandleAsciiSlaveRespond[])(uint8_t *, uint16_t, uint16_t) = {HandleReadCoilStatusRespond,
                                                                    HandleReadInputStatusRespond,
                                                                    HandleReadHoldingRegisterRespond,
                                                                    HandleReadInputRegisterRespond};

/*��������CreateAccessSlaveCommand�����ɷ��ʷ�����������*/
/*������ObjAccessInfo objInfo,Ҫ���ɷ�������Ķ�����Ϣ*/
/*      void *dataList,д�������б��Ĵ���Ϊuint16_t���ͣ�״̬��Ϊbool����*/
/*      uint8_t *commandBytes,���ɵ������б�*/
/*����ֵ��uint16_t�����ɵ�����ĳ���*/
uint16_t CreateAccessAsciiSlaveCommand(ObjAccessInfo objInfo, void *dataList, uint8_t *commandBytes)
{
    uint16_t commandLength = 0;
    /*���ɶ���������������������0x01��0x02��0x03��0x04,�����17���ֽ�*/
    if ((objInfo.functionCode >= ReadCoilStatus) && (objInfo.functionCode <= ReadInputRegister))
    {
        commandLength = SyntheticReadWriteAsciiSlaveCommand(objInfo, NULL, NULL, commandBytes);
    }

    /*����Ԥ�÷�������������������0x05,0x0F,������淢�����ݶ���*/
    if ((objInfo.functionCode == WriteSingleCoil) || (objInfo.functionCode == WriteMultipleCoil))
    {
        bool *statusList = (bool *)dataList;
        commandLength = SyntheticReadWriteAsciiSlaveCommand(objInfo, statusList, NULL, commandBytes);
    }

    /*����Ԥ�÷�������������������0x06,0x10,������淢�����ݶ���*/
    if ((objInfo.functionCode == WriteSingleRegister) || (objInfo.functionCode == WriteMultipleRegister))
    {
        uint16_t *registerList = (uint16_t *)dataList;
        commandLength = SyntheticReadWriteAsciiSlaveCommand(objInfo, NULL, registerList, commandBytes);
    }

    return commandLength;
}

/*�����յ��ķ�������Ӧ��Ϣ*/
/*uint8_t *recievedMessage,���յ�����Ϣ�б�*/
/*uint8_t *command,���͵Ķ����������ΪNULL���������б��в���*/
void ParsingAsciiSlaveRespondMessage(uint8_t *recievedMessage, uint8_t *command,uint16_t rxLength)
{
    /*�ж��Ƿ�ΪModbus ASCII��Ϣ*/
    if (0x3A != recievedMessage[0])
    {
        return ;
    }

    /*�ж���Ϣ�Ƿ��������*/
    if ((rxLength < 17) || (recievedMessage[rxLength - 2] != 0x0D) || (recievedMessage[rxLength - 1] != 0x0A))
    {
        return ;
    }

    uint16_t length = rxLength - 3;
    uint8_t hexMessage[256];

    if (!CovertAsciiMessageToHex(recievedMessage + 1, hexMessage, length))
    {
        return ;
    }
    /*������Ƕ������ķ�����Ϣ����Ҫ����*/
    if (hexMessage[1] > 0x04)
    {
        return;
    }

    if (command == NULL)
    {
        return;
    }

    uint8_t hexCommand[256];
    CovertAsciiMessageToHex(command + 1, hexCommand, 14);

    FunctionCode fuctionCode = (FunctionCode)hexMessage[1];
    uint16_t startAddress = (uint16_t)hexCommand[2];
    startAddress = (startAddress << 8) + (uint16_t)hexCommand[3];
    uint16_t quantity = (uint16_t)hexCommand[4];
    quantity = (quantity << 8) + (uint16_t)hexCommand[5];

    if ((fuctionCode >= ReadCoilStatus) && (fuctionCode <= ReadInputRegister))
    {
        HandleAsciiSlaveRespond[fuctionCode - 1](hexMessage, startAddress, quantity);
    }
}

/*���յ�������Ϣ���ж��Ƿ��Ƿ��������б�������ķ�����Ϣ��*/
/*��������ѷ��������б���ɾ�������������������������Ϣ*/
int FindAsciiCommandForRecievedMessage(uint8_t *recievedMessage, uint8_t (*commandList)[17], uint16_t commandNumber)
{
    int cmdIndex = -1;

    for (int i = 0; i < commandNumber; i++)
    {
        if (CheckMessageAgreeWithCommand(recievedMessage, commandList[i]) == true)
        {
            cmdIndex = i;
            break;
        }
    }
    return cmdIndex;
}

/*�жϽ��յ�����Ϣ�Ƿ��Ƿ�������ķ�����Ϣ*/
static bool CheckMessageAgreeWithCommand(uint8_t *recievedMessage, uint8_t *command)
{
    bool aw = false;

    if ((recievedMessage[1] == command[1]) && (recievedMessage[2] == command[2]) && (recievedMessage[3] == command[3]) && (recievedMessage[4] == command[5]))
    {
        uint8_t hexMessage[256];
        uint8_t hexCommand[256];

        CovertAsciiMessageToHex(command + 1, hexCommand, 14);
        CovertAsciiMessageToHex(recievedMessage + 1, hexMessage, 6);

        uint16_t quantity = (uint16_t)hexCommand[4];
        quantity = (quantity << 8) + (uint16_t)hexCommand[5];
        uint8_t bytescount = 0;
        if ((hexMessage[1] == ReadCoilStatus) || (hexMessage[1] == ReadInputStatus))
        {
            bytescount = (uint8_t)((quantity - 1) / 8 + 1);
        }

        if ((hexMessage[1] == ReadHoldingRegister) || (hexMessage[1] == ReadInputRegister))
        {
            bytescount = quantity * 2;
        }

        if (hexMessage[2] == bytescount)
        {
            aw = true;
        }
    }

    return aw;
}
/*�������վ״̬��������Ϣ������Ȧ״̬λ0x012������*/
static void HandleReadCoilStatusRespond(uint8_t *receivedMessage, uint16_t startAddress, uint16_t quantity)
{
    bool coilStatus[256];

    TransformClientReceivedData(receivedMessage, quantity, coilStatus, NULL);

    UpdateCoilStatus(startAddress, quantity, coilStatus);
}

/*�������վ״̬��������Ϣ��������״̬λ0x02������*/
static void HandleReadInputStatusRespond(uint8_t *receivedMessage, uint16_t startAddress, uint16_t quantity)
{
    bool inputStatus[256];

    TransformClientReceivedData(receivedMessage, quantity, inputStatus, NULL);

    UpdateInputStatus(startAddress, quantity, inputStatus);
}

/*�������վ�Ĵ���ֵ�ķ�����Ϣ�������ּĴ���0x03�����룩*/
static void HandleReadHoldingRegisterRespond(uint8_t *receivedMessage, uint16_t startAddress, uint16_t quantity)
{
    uint16_t holdingRegister[125];

    TransformClientReceivedData(receivedMessage, quantity, NULL, holdingRegister);

    UpdateHoldingRegister(startAddress, quantity, holdingRegister);
}

/*�������վ�Ĵ���ֵ�ķ�����Ϣ��������Ĵ���0x04������*/
static void HandleReadInputRegisterRespond(uint8_t *receivedMessage, uint16_t startAddress, uint16_t quantity)
{
    uint16_t inputRegister[125];

    TransformClientReceivedData(receivedMessage, quantity, NULL, inputRegister);

    UpdateInputResgister(startAddress, quantity, inputRegister);
}

/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/