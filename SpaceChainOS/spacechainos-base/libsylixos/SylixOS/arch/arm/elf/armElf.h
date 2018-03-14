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
** ��   ��   ��: armElf.h
**
** ��   ��   ��: Jiang.Taijin (��̫��)
**
** �ļ���������: 2010 �� 04 �� 17 ��
**
** ��        ��: ʵ�� ARM ��ϵ�ṹ�� ELF �ļ��ض�λ.
*********************************************************************************************************/

#ifndef __ARCH_ARMELF_H
#define __ARCH_ARMELF_H

#ifdef LW_CFG_CPU_ARCH_ARM                                              /*  ARM ��ϵ�ṹ                */

#define ELF_CLASS       ELFCLASS32
#define ELF_ARCH        EM_ARM

#define PT_ARM_EXIDX    0x70000001                                      /* .ARM.exidx segment           */

#endif                                                                  /*  LW_CFG_CPU_ARCH_ARM         */
#endif                                                                  /*  __ARCH_ARMELF_H             */
/*********************************************************************************************************
  END
*********************************************************************************************************/