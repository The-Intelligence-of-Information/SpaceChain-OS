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
** ��   ��   ��: sparcExc.c
**
** ��   ��   ��: Xu.Guizhou (�����)
**
** �ļ���������: 2017 �� 05 �� 15 ��
**
** ��        ��: SPARC ��ϵ�����쳣����.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
#include "dtrace.h"
#if LW_CFG_VMM_EN > 0
#include "arch/sparc/mm/mmu/sparcMmu.h"
#endif                                                                  /*  LW_CFG_VMM_EN > 0           */
/*********************************************************************************************************
  �ⲿ���ñ�������
*********************************************************************************************************/
extern LW_CLASS_CPU _K_cpuTable[];                                      /*  CPU ��                      */
extern LW_STACK     _K_stkInterruptStack[LW_CFG_MAX_PROCESSORS][LW_CFG_INT_STK_SIZE / sizeof(LW_STACK)];
/*********************************************************************************************************
  �ⲿ��������
*********************************************************************************************************/
extern UINT sparcUnalignedHandle(ARCH_REG_CTX  *regs, addr_t  *pulAbortAddr, UINT  *puiMethod);
/*********************************************************************************************************
  �ж����
*********************************************************************************************************/
#if LW_CFG_SMP_EN > 0
addr_t  _G_ulIntSafeStack[LW_CFG_MAX_PROCESSORS];
addr_t  _G_ulIntNesting[LW_CFG_MAX_PROCESSORS];
addr_t  _G_ulCpu[LW_CFG_MAX_PROCESSORS];
#else
addr_t  _G_ulIntSafeStack[1];
addr_t  _G_ulIntNesting[1];
addr_t  _G_ulCpu[1];
#endif
/*********************************************************************************************************
  ����ʹ���������
*********************************************************************************************************/
#if LW_CFG_SMP_EN > 0
#define VECTOR_OP_LOCK()    LW_SPIN_LOCK_IGNIRQ(&_K_slVectorTable)
#define VECTOR_OP_UNLOCK()  LW_SPIN_UNLOCK_IGNIRQ(&_K_slVectorTable)
#else
#define VECTOR_OP_LOCK()
#define VECTOR_OP_UNLOCK()
#endif                                                                  /*  LW_CFG_SMP_EN > 0           */
/*********************************************************************************************************
** ��������: archIntHandle
** ��������: bspIntHandle ��Ҫ���ô˺��������ж� (�ر��ж����������)
** �䡡��  : ulVector         �ж�����
**           bPreemptive      �ж��Ƿ����ռ
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
** ע  ��  : �˺����˳�ʱ����Ϊ�жϹر�״̬.
*********************************************************************************************************/
LW_WEAK VOID  archIntHandle (ULONG  ulVector, BOOL  bPreemptive)
{
    REGISTER irqreturn_t  irqret;

    if (_Inter_Vector_Invalid(ulVector)) {
        return;                                                         /*  �����Ų���ȷ                */
    }

    if (LW_IVEC_GET_FLAG(ulVector) & LW_IRQ_FLAG_PREEMPTIVE) {
        bPreemptive = LW_TRUE;
    }

    if (bPreemptive) {
        VECTOR_OP_LOCK();
        __ARCH_INT_VECTOR_DISABLE(ulVector);                            /*  ���� vector �ж�            */
        VECTOR_OP_UNLOCK();
        KN_INT_ENABLE_FORCE();                                          /*  �����ж�                    */
    }

    irqret = API_InterVectorIsr(ulVector);                              /*  �����жϷ������            */
    
    KN_INT_DISABLE();                                                   /*  �����ж�                    */
    
    if (bPreemptive) {
        if (irqret != LW_IRQ_HANDLED_DISV) {
            VECTOR_OP_LOCK();
            __ARCH_INT_VECTOR_ENABLE(ulVector);                         /*  ���� vector �ж�            */
            VECTOR_OP_UNLOCK();
        }
    
    } else if (irqret == LW_IRQ_HANDLED_DISV) {
        VECTOR_OP_LOCK();
        __ARCH_INT_VECTOR_DISABLE(ulVector);                            /*  ���� vector �ж�            */
        VECTOR_OP_UNLOCK();
    }
}
/*********************************************************************************************************
** ��������: archInstAccessErrHandle
** ��������: A peremptory error exception occurred on an instruction access
**           (for example, a parity error on an instruction cache access).
** �䡡��  : ulRetAddr     ���ص�ַ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  archInstAccessErrHandle (addr_t  ulRetAddr)
{
    PLW_CLASS_TCB   ptcbCur;
    LW_VMM_ABORT    abtInfo;

    LW_TCB_GET_CUR(ptcbCur);

    abtInfo.VMABT_uiType = LW_VMM_ABORT_TYPE_FATAL_ERROR;
    API_VmmAbortIsr(ulRetAddr, ulRetAddr, &abtInfo, ptcbCur);
}
/*********************************************************************************************************
** ��������: archDataStoreErrHandle
** ��������: A peremptory error exception occurred on a data store to memory
**           (for example, a bus parity error on a store from a store buffer).
** �䡡��  : ulRetAddr     ���ص�ַ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  archDataStoreErrHandle (addr_t  ulRetAddr)
{
    PLW_CLASS_TCB   ptcbCur;
    LW_VMM_ABORT    abtInfo;

    LW_TCB_GET_CUR(ptcbCur);

    abtInfo.VMABT_uiType = LW_VMM_ABORT_TYPE_FATAL_ERROR;
    API_VmmAbortIsr(ulRetAddr, ulRetAddr, &abtInfo, ptcbCur);
}
/*********************************************************************************************************
** ��������: archDataAccessErrHandle
** ��������: A peremptory error exception occurred on a load/store data access from/to memory
**           (for example, a parity error on a data cache access, or an uncorrect-able ECC memory error).
** �䡡��  : ulRetAddr     ���ص�ַ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  archDataAccessErrHandle (addr_t  ulRetAddr)
{
    PLW_CLASS_TCB   ptcbCur;
    LW_VMM_ABORT    abtInfo;

    LW_TCB_GET_CUR(ptcbCur);

    abtInfo.VMABT_uiType = LW_VMM_ABORT_TYPE_FATAL_ERROR;
    API_VmmAbortIsr(ulRetAddr, ulRetAddr, &abtInfo, ptcbCur);
}
/*********************************************************************************************************
** ��������: archInstAccessExcHandle
** ��������: A blocking error exception occurred on an instruction access
**           (for example, an MMU indicated that the page was invalid or read-protected).
** �䡡��  : ulRetAddr     ���ص�ַ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  archInstAccessExcHandle (addr_t  ulRetAddr)
{
#if LW_CFG_VMM_EN > 0
    PLW_CLASS_TCB   ptcbCur;
    addr_t          ulAbortAddr;
    UINT32          ulFaultStatus;
    UINT32          uiAT;
    LW_VMM_ABORT    abtInfo;

    LW_TCB_GET_CUR(ptcbCur);

    ulFaultStatus = sparcMmuGetFaultStatus();
    if (ulFaultStatus & FSTAT_FAV_MASK) {
        ulAbortAddr = sparcMmuGetFaultAddr();                           /*  ĳЩʵ��Ϊ����ҳ���ַ      */

    } else {
        ulAbortAddr = (addr_t)-1;
    }

    uiAT = ((ulFaultStatus & FSTAT_AT_MASK) >> FSTAT_AT_SHIFT);
    if (uiAT <= FSTAT_AT_LSD) {
        abtInfo.VMABT_uiMethod = LW_VMM_ABORT_METHOD_READ;

    } else if (uiAT <= FSTAT_AT_LXSI) {
        abtInfo.VMABT_uiMethod = LW_VMM_ABORT_METHOD_EXEC;

    } else {
        abtInfo.VMABT_uiMethod = LW_VMM_ABORT_METHOD_WRITE;
    }

    switch ((ulFaultStatus & FSTAT_FT_MASK) >> FSTAT_FT_SHIFT) {

    case FSTAT_FT_NONE:
    case FSTAT_FT_RESV:
        break;

    case FSTAT_FT_INVADDR:
        abtInfo.VMABT_uiType = LW_VMM_ABORT_TYPE_MAP;
        break;

    case FSTAT_FT_PROT:
        abtInfo.VMABT_uiType = LW_VMM_ABORT_TYPE_PERM;
        break;

    case FSTAT_FT_PRIV:
        abtInfo.VMABT_uiType = LW_VMM_ABORT_TYPE_PERM;
        break;

    case FSTAT_FT_TRANS:
        abtInfo.VMABT_uiType = LW_VMM_ABORT_TYPE_MAP;
        break;

    case FSTAT_FT_ACCESSBUS:
        abtInfo.VMABT_uiType = LW_VMM_ABORT_TYPE_BUS;
        break;

    case FSTAT_FT_INTERNAL:
        abtInfo.VMABT_uiType = LW_VMM_ABORT_TYPE_TERMINAL;
        break;
    }

    if (abtInfo.VMABT_uiType) {
        API_VmmAbortIsr(ulRetAddr, ulAbortAddr, &abtInfo, ptcbCur);
    }
#else
    archInstAccessErrHandle(ulRetAddr);
#endif                                                                  /*  LW_CFG_VMM_EN > 0           */
}
/*********************************************************************************************************
** ��������: archDataAccessExcHandle
** ��������: A blocking error exception occurred on a load/store data access.
**           (for example, an MMU indicated that the page was invalid or write-protected).
** �䡡��  : ulRetAddr     ���ص�ַ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  archDataAccessExcHandle (addr_t  ulRetAddr)
{
    archInstAccessExcHandle(ulRetAddr);
}
/*********************************************************************************************************
** ��������: archDataAccessMmuMissHandle
** ��������: A miss in an MMU occurred on a load/store access from/to memory.
**           For example, a PDC or TLB did not contain a translation for the virtual adddress.
** �䡡��  : ulRetAddr     ���ص�ַ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
LW_WEAK VOID  archDataAccessMmuMissHandle (addr_t  ulRetAddr)
{
}
/*********************************************************************************************************
** ��������: archInstAccessMmuMissHandle
** ��������: A miss in an MMU occurred on an instruction access from memory.
**           For example, a PDC or TLB did not contain a translation for the virtual adddress.
** �䡡��  : ulRetAddr     ���ص�ַ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
LW_WEAK VOID  archInstAccessMmuMissHandle (addr_t  ulRetAddr)
{
}
/*********************************************************************************************************
** ��������: archRRegAccessErrHandle
** ��������: A peremptory error exception occurred on an r register access
**           (for example, a parity error on an r register read).
** �䡡��  : ulRetAddr     ���ص�ַ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  archRRegAccessErrHandle (addr_t  ulRetAddr)
{
    PLW_CLASS_TCB   ptcbCur;
    LW_VMM_ABORT    abtInfo;

    LW_TCB_GET_CUR(ptcbCur);

    abtInfo.VMABT_uiType   = LW_VMM_ABORT_TYPE_BUS;
    abtInfo.VMABT_uiMethod = BUS_OBJERR;
    API_VmmAbortIsr(ulRetAddr, ulRetAddr, &abtInfo, ptcbCur);
}
/*********************************************************************************************************
** ��������: archPrivInstHandle
** ��������: An attempt was made to execute a privileged instruction while S = 0.
** �䡡��  : ulRetAddr     ���ص�ַ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  archPrivInstHandle (addr_t  ulRetAddr)
{
    PLW_CLASS_TCB   ptcbCur;
    LW_VMM_ABORT    abtInfo;

    LW_TCB_GET_CUR(ptcbCur);

    abtInfo.VMABT_uiType = LW_VMM_ABORT_TYPE_UNDEF;
    API_VmmAbortIsr(ulRetAddr, ulRetAddr, &abtInfo, ptcbCur);
}
/*********************************************************************************************************
** ��������: archIllegalInstHandle
** ��������: An attempt was made to execute an instruction with an unimplemented opcode,
**           or an UNIMP instruction
** �䡡��  : ulRetAddr     ���ص�ַ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  archIllegalInstHandle (addr_t  ulRetAddr)
{
    PLW_CLASS_TCB   ptcbCur;
    LW_VMM_ABORT    abtInfo;

    LW_TCB_GET_CUR(ptcbCur);

    abtInfo.VMABT_uiType = LW_VMM_ABORT_TYPE_UNDEF;
    API_VmmAbortIsr(ulRetAddr, ulRetAddr, &abtInfo, ptcbCur);
}
/*********************************************************************************************************
** ��������: archFpDisableHandle
** ��������: An attempt was made to execute an FPop, FBfcc,
**           or a floating-point load/store instruction while EF = 0 or an FPU was not present.
** �䡡��  : ulRetAddr     ���ص�ַ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  archFpDisableHandle (addr_t  ulRetAddr)
{
    PLW_CLASS_TCB   ptcbCur;
    LW_VMM_ABORT    abtInfo;

    LW_TCB_GET_CUR(ptcbCur);

#if LW_CFG_CPU_FPU_EN > 0
    if (archFpuUndHandle(ptcbCur) == ERROR_NONE) {                      /*  ���� FPU ָ��̽��           */
        return;
    }
#endif                                                                  /*  LW_CFG_CPU_FPU_EN > 0       */

    abtInfo.VMABT_uiType   = LW_VMM_ABORT_TYPE_FPE;
    abtInfo.VMABT_uiMethod = FPE_FLTINV;                                /*  FPU ������                  */
    API_VmmAbortIsr(ulRetAddr, ulRetAddr, &abtInfo, ptcbCur);
}
/*********************************************************************************************************
** ��������: archCpDisableHandle
** ��������: An attempt was made to execute an CPop, CBccc,
**           or a coprocessor load/store instruction while EC = 0 or a coprocessor was not present.
** �䡡��  : ulRetAddr     ���ص�ַ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  archCpDisableHandle (addr_t  ulRetAddr)
{
    PLW_CLASS_TCB   ptcbCur;
    LW_VMM_ABORT    abtInfo;

    LW_TCB_GET_CUR(ptcbCur);

    abtInfo.VMABT_uiType = LW_VMM_ABORT_TYPE_UNDEF;
    API_VmmAbortIsr(ulRetAddr, ulRetAddr, &abtInfo, ptcbCur);
}
/*********************************************************************************************************
** ��������: archUnimplFlushHandle
** ��������: An attempt was made to execute a FLUSH instruction,
**           the semantics of which are not fully implemented in hardware.
** �䡡��  : ulRetAddr     ���ص�ַ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  archUnimplFlushHandle (addr_t  ulRetAddr)
{
    PLW_CLASS_TCB   ptcbCur;
    LW_VMM_ABORT    abtInfo;

    LW_TCB_GET_CUR(ptcbCur);

    abtInfo.VMABT_uiType = LW_VMM_ABORT_TYPE_UNDEF;
    API_VmmAbortIsr(ulRetAddr, ulRetAddr, &abtInfo, ptcbCur);
}
/*********************************************************************************************************
** ��������: archWatchPointDectectHandle
** ��������: An instruction fetch memory address or load/store data memory address
**           matched the contents of a pre-loaded implementation-dependent ��watch-point�� register.
**           Whether a SPARC processor generates watchpoint_detected exceptions is
**           implementation-dependent.
** �䡡��  : ulRetAddr     ���ص�ַ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
LW_WEAK VOID  archWatchPointDectectHandle (addr_t  ulRetAddr)
{
}
/*********************************************************************************************************
** ��������: archMemAddrNoAlignHandle
** ��������: A load/store instruction would have generated a memory address that was not
**           properly aligned according to the instruction, or a JMPL or RETT instruction
**           would have generated a non-word-aligned address.
** �䡡��  : ulRetAddr     ���ص�ַ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  archMemAddrNoAlignHandle (addr_t  ulRetAddr)
{
    PLW_CLASS_TCB  ptcbCur;
    LW_VMM_ABORT   abtInfo;
    addr_t         ulAbortAddr;

    LW_TCB_GET_CUR(ptcbCur);

    abtInfo.VMABT_uiType = sparcUnalignedHandle((ARCH_REG_CTX *)ptcbCur->TCB_pstkStackNow,
                                                &ulAbortAddr, &abtInfo.VMABT_uiMethod);
    if (abtInfo.VMABT_uiType) {
        API_VmmAbortIsr(ulRetAddr, ulAbortAddr, &abtInfo, ptcbCur);
    }
}
/*********************************************************************************************************
** ��������: archFpExcHandle
** ��������: an FPop instruction generated an IEEE_754_exception and its correspond-ing
**           trap enable mask (TEM) bit was 1, or the FPop was unimplemented,
**           or the FPop did not complete, or there was a sequence or hardware error in the FPU.
**           The type of floating-point exception is encoded in the FSR��s ftt field.
** �䡡��  : ulRetAddr     ���ص�ַ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  archFpExcHandle (addr_t  ulRetAddr)
{
    PLW_CLASS_TCB   ptcbCur;
    LW_VMM_ABORT    abtInfo;
    UINT32          uiFSR;

    LW_TCB_GET_CUR(ptcbCur);

    abtInfo.VMABT_uiType   = LW_VMM_ABORT_TYPE_FPE;
    abtInfo.VMABT_uiMethod = FPE_FLTINV;

    uiFSR = *(UINT32 *)((addr_t)ptcbCur->TCB_pvStackFP + FSR_OFFSET);
    if ((uiFSR & 0x1c000) == (1 << 14)) {
        if (uiFSR & 0x10) {
            abtInfo.VMABT_uiMethod = FPE_FLTINV;

        } else if (uiFSR & 0x08) {
            abtInfo.VMABT_uiMethod = FPE_FLTOVF;

        } else if (uiFSR & 0x04) {
            abtInfo.VMABT_uiMethod = FPE_FLTUND;

        } else if (uiFSR & 0x02) {
            abtInfo.VMABT_uiMethod = FPE_FLTDIV;

        } else if (uiFSR & 0x01) {
            abtInfo.VMABT_uiMethod = FPE_FLTRES;
        }
    }
    API_VmmAbortIsr(ulRetAddr, ulRetAddr, &abtInfo, ptcbCur);
}
/*********************************************************************************************************
** ��������: archCpExcHandle
** ��������: A coprocessor instruction generated an exception.
** �䡡��  : ulRetAddr     ���ص�ַ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  archCpExcHandle (addr_t  ulRetAddr)
{
    PLW_CLASS_TCB   ptcbCur;
    LW_VMM_ABORT    abtInfo;

    LW_TCB_GET_CUR(ptcbCur);

    abtInfo.VMABT_uiType = LW_VMM_ABORT_TYPE_UNDEF;
    API_VmmAbortIsr(ulRetAddr, ulRetAddr, &abtInfo, ptcbCur);
}
/*********************************************************************************************************
** ��������: archTagOverFlowHandle
** ��������: A TADDccTV or TSUBccTV instruction was executed, and either arith-metic
**           overflow occurred or at least one of the tag bits of the operands was nonzero.
** �䡡��  : ulRetAddr     ���ص�ַ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  archTagOverFlowHandle (addr_t  ulRetAddr)
{
    PLW_CLASS_TCB   ptcbCur;
    LW_VMM_ABORT    abtInfo;

    LW_TCB_GET_CUR(ptcbCur);

    abtInfo.VMABT_uiType = LW_VMM_ABORT_TYPE_TERMINAL;
    API_VmmAbortIsr(ulRetAddr, ulRetAddr, &abtInfo, ptcbCur);
}
/*********************************************************************************************************
** ��������: archDivZeroHandle
** ��������: An integer divide instruction attempted to divide by zero.
** �䡡��  : ulRetAddr     ���ص�ַ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  archDivZeroHandle (addr_t  ulRetAddr)
{
    PLW_CLASS_TCB   ptcbCur;
    LW_VMM_ABORT    abtInfo;

    LW_TCB_GET_CUR(ptcbCur);

    abtInfo.VMABT_uiType   = LW_VMM_ABORT_TYPE_FPE;
    abtInfo.VMABT_uiMethod = FPE_FLTDIV;
    API_VmmAbortIsr(ulRetAddr, ulRetAddr, &abtInfo, ptcbCur);
}
/*********************************************************************************************************
** ��������: archTrapInstHandle
** ��������: A Ticc instruction was executed and the trap condition evaluated to true.
** �䡡��  : ulRetAddr     ���ص�ַ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  archTrapInstHandle (addr_t  ulRetAddr)
{
    PLW_CLASS_TCB   ptcbCur;
    LW_VMM_ABORT    abtInfo;

#if LW_CFG_GDB_EN > 0
    UINT    uiBpType = archDbgTrapType(ulRetAddr, (PVOID)LW_NULL);      /*  �ϵ�ָ��̽��                */
    if (uiBpType) {
        if (API_DtraceBreakTrap(ulRetAddr, uiBpType) == ERROR_NONE) {   /*  ������Խӿڶϵ㴦��        */
            return;
        }
    }
#endif                                                                  /*  LW_CFG_GDB_EN > 0           */

    LW_TCB_GET_CUR(ptcbCur);

    abtInfo.VMABT_uiType = LW_VMM_ABORT_TYPE_SYS;
    API_VmmAbortIsr(ulRetAddr, ulRetAddr, &abtInfo, ptcbCur);
}
/*********************************************************************************************************
** ��������: archTrapInit
** ��������: ��ʼ���쳣����ϵͳ
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  archTrapInit (VOID)
{
    ULONG  i;
    ULONG  ulMaxProcessors = 1;

    /*
     *  ��Ҫ��
     *  ����ÿ�� CPU ���ж�ջ��ַ��ÿ�� CPU ���ж�Ƕ�׼�������ָ�룬
     *  ��Щ��Ϣ�����жϴ������ʹ�ã�������� C ��������ȡ��Щ��Ϣ��
     *  �жϴ���������Ҫʹ�ô�ջ���� C �������л�����
     *  ������Ҫ����ʱջ���� C �������л������ٵ��� C ������ȡջ��ַ��������л�һ��ջ
     *  ע�⣺SPARC �л�ջ���ļ����
     */
#if LW_CFG_SMP_EN > 0
    ulMaxProcessors = LW_CFG_MAX_PROCESSORS;
#endif

    for (i = 0; i < ulMaxProcessors; i++) {
        _G_ulCpu[i]          = (addr_t)&_K_cpuTable[i];
        _G_ulIntNesting[i]   = (addr_t)&_K_cpuTable[i].CPU_ulInterNesting;

#if CPU_STK_GROWTH == 0
        _G_ulIntSafeStack[i] = (addr_t)&_K_stkInterruptStack[i][0];
        _G_ulIntSafeStack[i] = ROUND_UP(_G_ulIntSafeStack[i], 16);
#else
        _G_ulIntSafeStack[i] = (addr_t)&_K_stkInterruptStack[i][(LW_CFG_INT_STK_SIZE / sizeof(LW_STACK)) - 1];
        _G_ulIntSafeStack[i] = ROUND_DOWN(_G_ulIntSafeStack[i], 16);
#endif                                                                  /*  CPU_STK_GROWTH              */
    }
}
/*********************************************************************************************************
  END
*********************************************************************************************************/