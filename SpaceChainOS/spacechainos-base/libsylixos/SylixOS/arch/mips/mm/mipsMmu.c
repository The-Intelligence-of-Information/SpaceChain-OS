/*********************************************************************************************************
**
**                                    �й�������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                       SylixOS(TM)
**
**                               Copyright  All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: mipsMmu.c
**
** ��   ��   ��: Ryan.Xin (�Ž���)
**
** �ļ���������: 2015 �� 09 �� 01 ��
**
** ��        ��: MIPS ��ϵ���� MMU ����.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
/*********************************************************************************************************
  �ü�֧��
*********************************************************************************************************/
#if LW_CFG_VMM_EN > 0
#include "mmu/mips32/mips32Mmu.h"
/*********************************************************************************************************
** ��������: archCacheInit
** ��������: ��ʼ�� CACHE
** �䡡��  : uiInstruction  ָ�� CACHE ����
**           uiData         ���� CACHE ����
**           pcMachineName  ��������
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  archMmuInit (CPCHAR  pcMachineName)
{
    LW_MMU_OP  *pmmuop = API_VmmGetLibBlock();

    _DebugFormat(__LOGMESSAGE_LEVEL, "%s %s MMU initialization.\r\n", 
                 LW_CFG_CPU_ARCH_FAMILY, pcMachineName);

    if ((lib_strcmp(pcMachineName, MIPS_MACHINE_LS1X)   == 0) ||
        (lib_strcmp(pcMachineName, MIPS_MACHINE_LS2X)   == 0) ||
        (lib_strcmp(pcMachineName, MIPS_MACHINE_LS3X)   == 0) ||
        (lib_strcmp(pcMachineName, MIPS_MACHINE_24KF)   == 0) ||
        (lib_strcmp(pcMachineName, MIPS_MACHINE_JZ47XX) == 0)) {
        mips32MmuInit(pmmuop, pcMachineName);

    } else {
        _DebugHandle(__ERRORMESSAGE_LEVEL, "unknown machine name.\r\n");
    }
}

#endif                                                                  /*  LW_CFG_VMM_EN > 0           */
/*********************************************************************************************************
  END
*********************************************************************************************************/