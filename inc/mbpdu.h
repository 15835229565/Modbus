/******************************************************************************/
/** ģ�����ƣ�ModbusͨѶ                                                     **/
/** �ļ����ƣ�mbpdu.h                                                        **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺����ʵ��modbus����Э��ջPDU���ַ�װ                            **/
/**--------------------------------------------------------------------------**/
/* һ�����͵�Modbus����֡�����²�����ɣ�                                     */
/* <------------------- MODBUS������·Ӧ�����ݵ�Ԫ��ADU�� ----------------->  */
/*              <------ MODBUS��Э�����ݵ�Ԫ��PDU�� ------->                */
/*  +-----------+---------------+----------------------------+-------------+  */
/*  |  ��ַ��   |    ������     | ������                     | CRC/LRC     |  */
/*  +-----------+---------------+----------------------------+-------------+  */
/*                                                                            */
/* һ��TCP/IP Modbus����֡�����²�����ɣ�                                    */
/* <------------------- MODBUS TCP/IPӦ�����ݵ�Ԫ��ADU�� ------------------>  */
/*                       <-------- MODBUS��Э�����ݵ�Ԫ��PDU�� ---------->  */
/*  +--------------------+---------------+---------------------------------+  */
/*  |  MBAPͷ��          |    ������     | ������                          |  */
/*  +--------------------+---------------+---------------------------------+  */
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2015-06-17          ���Ҿ�            �����ļ�               **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __mbpdu_h
#define __mbpdu_h

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"
#include "string.h"

/*����Modbus�Ĳ��������룬֧��01��02��03��04��05��06��15��16������*/
typedef enum {
  ReadCoilStatus=0x01,       	/*����Ȧ״̬����������λ��״̬��*/
  ReadInputStatus=0x02,      	/*������λ״̬�����������λ��״̬��*/
  ReadHoldingRegister=0x03,  	/*�����ּĴ�������������ּĴ�������ֵ��*/
  ReadInputRegister=0x04,    	/*������Ĵ��������������Ĵ�������ֵ��*/
  WriteSingleCoil=0x05,      	/*ǿ�Ƶ�����Ȧ��ǿ�Ƶ������λ��״̬��*/
  WriteSingleRegister=0x06,  	/*Ԥ�Ƶ����Ĵ������趨һ���Ĵ�������ֵ��*/
  WriteMultipleCoil=0x0F,    	/*ǿ�ƶ����Ȧ��ǿ�ƶ�����λ��״̬��*/
  WriteMultipleRegister=0x10 	/*Ԥ�ƶ���Ĵ������趨����Ĵ�������ֵ��*/
}FunctionCode;

/*������յ�ָ�������ʱ�Ĵ�����*/
typedef enum{
  MB_OK=0x00,
  InvalidFunctionCode=0x01,//���Ϸ����ܴ���
  IllegalDataAddress=0x02,//�Ƿ������ݵ�ַ
  IllegalDataValues=0x03,//�Ƿ�������ֵ���߷�Χ
  OperationFail=0x04
}ModbusStatus;

/*�������ڴ���Ҫ���ʴ�վ��������������Ϣ*/
typedef struct{
  uint8_t unitID;
  FunctionCode functionCode;
  uint16_t startingAddress;
  uint16_t quantity;
}ObjAccessInfo;

/*��ΪRTU��վ��TCP�ͻ��ˣ�ʱ�����ɶ�дRTU��վ��TCP�����������������*/
uint16_t GenerateReadWriteCommand(ObjAccessInfo objInfo,bool *statusList,uint16_t *registerList,uint8_t commandBytes[]);

/*������վ���ͻ��ˣ��ӷ�������ȡ������*/
void TransformClientReceivedData(uint8_t * receivedMessage,uint16_t quantity,bool *statusList,uint16_t *registerLister);

/*������վ�����ʵ���Ӧ������0x01��0x02��0x03��0x04������,������Ӧ��Ϣ�ĳ���*/
uint16_t GenerateMasterAccessRespond(uint8_t *receivedMesasage,bool *statusList,uint16_t *registerList,uint8_t *respondBytes);

/*��鹦�����Ƿ���ȷ*/
ModbusStatus CheckFunctionCode(FunctionCode fc);

#endif
/*********** (C) COPYRIGHT 1999-2016 Moonan Technology *********END OF FILE****/
