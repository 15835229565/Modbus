/******************************************************************************/
/** ģ�����ƣ�ModbusͨѶ                                                     **/
/** �ļ����ƣ�modbusrtumaster.h                                              **/
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

#ifndef __mbrtumaster_h
#define __mbrtumaster_h

#include "mbrtu.h"
#include "mbcommon.h"

/*���ɷ��ʷ�����������*/
uint16_t CreateAccessSlaveCommand(ObjAccessInfo objInfo,void *dataList,uint8_t *commandBytes);

/*�����յ��ķ�������Ӧ��Ϣ*/
void ParsingSlaveRespondMessage(uint8_t *recievedMessage,uint8_t *command);

/*���յ�������Ϣ���ж��Ƿ��Ƿ��������б�������ķ�����Ϣ*/
int FindCommandForRecievedMessage(uint8_t *recievedMessage,uint8_t (*commandList)[8],uint16_t commandNumber);

#endif
/*********** (C) COPYRIGHT 1999-2016 Moonan Technology *********END OF FILE****/