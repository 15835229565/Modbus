/******************************************************************************/
/** ģ�����ƣ�ModbusͨѶ                                                     **/
/** �ļ����ƣ�mbconfig.h                                                     **/
/** ��    ����V1.0.0                                                         **/
/** ��    �飺��������ModbusЭ��ջʹ�õ���ض���                             **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2015-07-18          ���Ҿ�            �����ļ�               **/
/**     V1.1.0  2018-09-11          ���Ҿ�            ���ASCII���뷽ʽ      **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef _MB_CONFIG_H
#define _MB_CONFIG_H

/*�����Ƿ�ʹ��RTU��վ���ܣ�0Ϊ���ã�1Ϊʹ��*/
#ifndef MB_RTU_MASTER_ENABLED
#define MB_RTU_MASTER_ENABLED		(0)
#endif

/*�����Ƿ�ʹ��RTU��վ���ܣ�0Ϊ���ã�1Ϊʹ��*/
#ifndef MB_RTU_SLAVE_ENABLED
#define MB_RTU_SLAVE_ENABLED		(0)
#endif

/*�����Ƿ�ʹ��ASCII��վ���ܣ�0Ϊ���ã�1Ϊʹ��*/
#ifndef MB_ACSII_MASTER_ENABLED
#define MB_ACSII_MASTER_ENABLED		(0)
#endif

/*�����Ƿ�ʹ��ASCII��վ���ܣ�0Ϊ���ã�1Ϊʹ��*/
#ifndef MB_ASCII_SLAVE_ENABLED
#define MB_ASCII_SLAVE_ENABLED		(0)
#endif

/*�����Ƿ�ʹ��TCP���������ܣ�0Ϊ���ã�1Ϊʹ��*/
#ifndef MB_TCP_SERVER_ENABLED
#define MB_TCP_SERVER_ENABLED		(0)
#endif

/*�����Ƿ�ʹ��TCP�ͻ��˹��ܣ�0Ϊ���ã�1Ϊʹ��*/
#ifndef MB_TCP_CLIENT_ENABLED
#define MB_TCP_CLIENT_ENABLED		(0)
#endif

#if MB_RTU_MASTER_ENABLED > (0)
#include "mbrtumaster.h"
#endif

#if MB_RTU_SLAVE_ENABLED > (0)
#include "mbrtuslave.h"
#endif

#if MB_TCP_CLIENT_ENABLED > (0)
#include "mbtcpclient.h"
#endif

#if MB_TCP_SERVER_ENABLED > (0)
#include "mbtcpserver.h"
#endif

#include "mbcommon.h"

#endif
/*********** (C) COPYRIGHT 1999-2018 Moonan Technology *********END OF FILE****/