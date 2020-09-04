/******************************************************************************/
/** ģ�����ƣ�ModbusͨѶ                                                     **/
/** �ļ����ƣ�mbtcpclient.c                                                  **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺��������Modbus TCP�ͻ��˵�������Լ�����                       **/
/**           1��Modbus TCP�ͻ����û�Ӧ�õĽӿڲ�                            **/
/**           2��ʵ�ַ�����������������ɲ����䴫��Ӧ�ò�                    **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2016-04-17          ľ��              �����ļ�               **/
/**     V1.1.0  2020-09-01          ľ��              �������ֽṹ����       **/
/**                                                                          **/
/******************************************************************************/

#ifndef __mbtcpclient_h
#define __mbtcpclient_h

#include "mbtcp.h"
#include "mbcommon.h"

/* ����ɱ�д����Ȧ�����ݶ������� */
typedef struct WritedCoilListType {
  uint16_t coilAddress;         //��д��Ȧ���ĵ�ַ
  uint8_t value;                //��д��ֵ
  uint8_t writedStatus:1;       //д��Ȧ��״̬��1��Ҫд��0����Ҫ
  uint8_t conStatus:1;          //�Ƿ���������ַ��1������0������
}WritedCoilListNode;

/* ����ɱ�д���ּĴ������ݶ������� */
typedef struct WritedRegisterListType{
  uint16_t registerAddress;     //��д���ּĴ�����ַ
  uint16_t value;               //��д��ֵ
  uint16_t writedStatus:1;      //д�Ĵ�����״̬��1��Ҫд��0����Ҫ
  uint16_t conStatus:1;          //�Ƿ���������ַ��1������0������
  uint16_t dataType:14;         //���ݰ��������Ĵ�����1����2����4����0��ʾ��ǰһ�Ĵ���ͬ��һ������
}WritedRegisterListNode;

/* ���屻����TCP�������������� */
typedef struct AccessedTCPServerType{
  uint32_t flagPresetServer;                          //д�����������־
  union {
    uint32_t ipNumber;
    uint8_t ipSegment[4];
  }ipAddress;                                          //��������IP��ַ
  
  struct WritedCoilListHeadType{
    WritedCoilListNode *pWritedCoilNode;        //��д��Ȧ���ڵ�ָ��
    uint32_t writedCoilNumber;                  //��д��Ȧ���ڵ������
  }pWritedCoilHeadNode;                         //��д����Ȧ���б�
  
  struct WritedRegisterListHeadType{
    WritedRegisterListNode *pWritedRegisterNode;  //��д�Ĵ������ڵ�ָ��
    uint32_t writedRegisterNumber;              //��д�Ĵ������ڵ������
  }pWritedRegisterHeadNode;                     //��д�ı��ּĴ����б�
  
  struct AccessedTCPServerType *pNextNode;      //��һ��TCP�������ڵ�
}TCPAccessedServerType;

/* ���屾��TCP�ͻ��˶������� */
typedef struct LocalTCPClientType{
  uint16_t transaction;                                 //�����ʶ��
  uint16_t cmdNumber;                                  //�����������������
  uint16_t cmdOrder;                                   //��ǰ�����������б��е�λ��
  uint8_t (*pReadCommand)[12];                         //�������б��洢����������
  struct ServerListHeadType {
    TCPAccessedServerType *pServerNode;                 //�������ڵ�ָ��
    uint32_t serverNumber;                              //������������
  }ServerHeadNode;                                      //Server���������ͷ�ڵ�
  UpdateCoilStatusType pUpdateCoilStatus;               //������Ȧ������
  UpdateInputStatusType pUpdateInputStatus;             //��������״̬������
  UpdateHoldingRegisterType pUpdateHoldingRegister;     //���±��ּĴ���������
  UpdateInputResgisterType pUpdateInputResgister;       //��������Ĵ���������
}TCPLocalClientType;

/*���ɷ��ʷ�����������*/
uint16_t CreateAccessServerCommand(TCPLocalClientType *client,ObjAccessInfo objInfo,void *dataList,uint8_t *commandBytes);

/*�����յ��ķ�������Ӧ��Ϣ*/
void ParsingServerRespondMessage(TCPLocalClientType *client,uint8_t *recievedMessage);

/*������������ѷ��������б����������*/
void AddCommandBytesToList(TCPLocalClientType *client,uint8_t *commandBytes);

/* ʹ�ܻ���ʧ��д������������־λ���޸Ĵ�վ��дʹ�ܱ�־λ�� */
void ModifyWriteTCPServerEnableFlag(TCPLocalClientType *client,uint8_t ipAddress,bool en);

/* ��÷�������дʹ�ܱ�־λ��״̬ */
bool GetWriteTCPServerEnableFlag(TCPLocalClientType *client,uint8_t ipAddress);

/* �жϵ�ǰ�Ƿ���д����ʹ�ܣ�����True��û��д���� */
bool CheckWriteTCPServerNone(TCPLocalClientType *client);

/* ʵ����TCP���������� */
void InstantiateTCPServerObject(TCPAccessedServerType *server,
                                TCPLocalClientType *client,
                                uint8_t ipSegment1,uint8_t ipSegment2,
                                uint8_t ipSegment3,uint8_t ipSegment4);



/*��ʼ��TCP�ͻ��˶���*/
void InitializeTCPClientObject(TCPLocalClientType *client,      //Ҫ��ʼ���Ŀͻ��˶���
                               uint16_t cmdNumber,              //�����������������
                               uint8_t (*pReadCommand)[12],     //�������������б�
                               UpdateCoilStatusType pUpdateCoilStatus,
                               UpdateInputStatusType pUpdateInputStatus,
                               UpdateHoldingRegisterType pUpdateHoldingRegister,
                               UpdateInputResgisterType pUpdateInputResgister
                               );




#endif
/*********** (C) COPYRIGHT 1999-2016 Moonan Technology *********END OF FILE****/