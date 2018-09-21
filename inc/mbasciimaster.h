/******************************************************************************/
/** ģ�����ƣ�ModbusͨѶ                                                     **/
/** �ļ����ƣ�mbasciimaster.h                                                **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺��������Modbus ASCII��վ������Լ�����                         **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2018-09-11          ���Ҿ�            �����ļ�               **/
/**                                                                          **/
/******************************************************************************/

#ifndef __modbusasciimaster_h
#define __modbusasciimaster_h

#include "mbascii.h"
#include "mbcommon.h"

/*���ɷ��ʷ�����������*/
uint16_t CreateAccessAsciiSlaveCommand(ObjAccessInfo objInfo,void *dataList,uint8_t *commandBytes);

/*�����յ��ķ�������Ӧ��Ϣ*/
void ParsingAsciiSlaveRespondMessage(uint8_t *recievedMessage, uint8_t *command,uint16_t rxLength);

/*���յ�������Ϣ���ж��Ƿ��Ƿ��������б�������ķ�����Ϣ*/
int FindAsciiCommandForRecievedMessage(uint8_t *recievedMessage,uint8_t (*commandList)[17],uint16_t commandNumber);

#endif
/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/