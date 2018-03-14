/*********************************************************************************************************
**
**                                    �й�������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: lwip_if.h
**
** ��   ��   ��: Han.Hui (����)
**
** �ļ���������: 2014 �� 05 �� 02 ��
**
** ��        ��: netif lock �ӿ�.
*********************************************************************************************************/

#ifndef __LWIP_IF_H
#define __LWIP_IF_H

/*********************************************************************************************************
  ����ӿ���
*********************************************************************************************************/

#ifdef  __SYLIXOS_KERNEL
INT  if_list_init(VOID);
INT  if_list_lock(BOOL  bWrite);
INT  if_list_unlock(VOID);

#define LWIP_IF_LIST_LOCK(w)    if_list_lock(w)
#define LWIP_IF_LIST_UNLOCK()   if_list_unlock()
#endif                                                                  /*  __SYLIXOS_KERNEL            */

#endif                                                                  /*  __LWIP_IF_H                 */
/*********************************************************************************************************
  END
*********************************************************************************************************/