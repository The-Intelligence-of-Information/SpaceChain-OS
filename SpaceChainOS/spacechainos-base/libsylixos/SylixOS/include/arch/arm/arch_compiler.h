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
** ��   ��   ��: arch_compiler.h
**
** ��   ��   ��: Han.Hui (����)
**
** �ļ���������: 2013 �� 05 �� 04 ��
**
** ��        ��: ARM ���������.
*********************************************************************************************************/

#ifndef __ARM_ARCH_COMPILER_H
#define __ARM_ARCH_COMPILER_H

/*********************************************************************************************************
  fast variable
*********************************************************************************************************/

#ifndef REGISTER
#define	REGISTER                register                                /*  �Ĵ�������                  */
#endif

/*********************************************************************************************************
  inline function
*********************************************************************************************************/

#ifdef __GNUC__
#define LW_INLINE               inline                                  /*  ������������                */
#else
#define LW_INLINE               __inline
#endif

/*********************************************************************************************************
  weak function
*********************************************************************************************************/

#ifdef __SYLIXOS_KERNEL
#ifdef __GNUC__
#define LW_WEAK                 __attribute__((weak))                   /*  ������                      */
#else
#define LW_WEAK
#endif
#endif                                                                  /*  __SYLIXOS_KERNEL            */

/*********************************************************************************************************
  shared object function call prepare
*********************************************************************************************************/

#define LW_SOFUNC_PREPARE(func)

/*********************************************************************************************************
  �������ṹ����������� (__CC_ARM(armcc) __GNUC__(gcc) ...)
*********************************************************************************************************/

#if defined(__CC_ARM)
#define LW_STRUCT_PACK_FIELD(x)     x
#define LW_STRUCT_PACK_STRUCT       
#define LW_STRUCT_PACK_BEGIN        __packed                            /*  ���ֽ����Žṹ��            */
#define LW_STRUCT_PACK_END                                              /*  �������ֽ����Žṹ��        */

#else
#define LW_STRUCT_PACK_FIELD(x)     x
#define LW_STRUCT_PACK_STRUCT       __attribute__((packed))
#define LW_STRUCT_PACK_BEGIN                                            /*  ���ֽ����Žṹ��            */
#define LW_STRUCT_PACK_END                                              /*  �������ֽ����Žṹ��        */
#endif                                                                  /*  !__CC_ARM                   */

/*********************************************************************************************************
  ������ ABI �Ƿ������ں�ʹ�ø�������
*********************************************************************************************************/

#define LW_KERN_FLOATING            1                                   /*  �Ƿ������ں�ʹ�ø�������    */

#endif                                                                  /*  __ARM_ARCH_COMPILER_H       */
/*********************************************************************************************************
  END
*********************************************************************************************************/