/**
 ****************************************************************************************
 *
 * @file ir.h
 *
 * @brief ir Driver for ir operation.
 *
 * Copyright (C) Beken 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef _IR_H_
#define _IR_H_

/**
 ****************************************************************************************
 * @defgroup IR 
 * @ingroup DRIVERS
 * @brief IR driver
 *
 * @{
 *
 ****************************************************************************************
 */
 
#define  IR_SEND  1
#define  IR_RECEIVE  0

#define IR_ENABLE				0

 //=====================================================================================
// IR && RNG
//=====================================================================================

#define APB9_RGN_IR_BASE							  	0x00806900
#define REG_APB9_RNG_CFG               (*((volatile unsigned long *) (APB9_RGN_IR_BASE + 0X0 * 0X04)))
#define REG_APB9_RNG_DATA               (*((volatile unsigned long *) (APB9_RGN_IR_BASE + 0X1 * 0X04)))
#define REG_APB9_IR_CFG               (*((volatile unsigned long *) (APB9_RGN_IR_BASE + 0X2 * 0X04)))

#define XVR_REG0B               					(*((volatile unsigned int *)   (APB_XVER_BASE+0x2C)))

#define IR_DATA_IN_BIT										  (0X04)
#define IR_DATA_OUT_BIT											(0x02)
#define	IR_SEN_BIT												(17)

#define	port_ir_high()		REG_APB5_GPIOA_DATA |= 0x08
#define	port_ir_low()		REG_APB5_GPIOA_DATA &= (~0x08)


//ir_zaibo �����ز��iir_rec_overtimer ����ʱ ir_return_count ������ir_rec_buf �յ�������
extern	uint32_t	ir_zaibo, ir_rec_overtimer, ir_return_count;
extern	uint32_t	ir_rec_buf[200];


enum ir_ret
{
	IR_REC_ERR =0,
	IR_OVERTIMER =1,
	IR_REC_OK =2,
	
};

//power =0 ��IR ��Դ��power =1  ��IR ��Դ
void			ir_power(uint8_t	 power);
// ���ⷢ��ܵ���ѡ�� 0= 50MA ; 1= 100MA ;2 =200MA Ĭ����100MA
void 		ir_send_power(uint8_t power);
// ����ѧϰ���ܵ�DEMO 
void 		ir_demo_test(void);
// �������������ã�����ֻʹ��ir_sensitivity_regb_set  sen ֵ0-3   0  ��������� 
void 		ir_sensitivity_regb_set(uint8_t sen);
// 			�����Ʋ����� zaibo =�ز�ʱ��( ��λ΢��)  send_timer =����ʱ���(��λ΢��)
void			ir_modulation_send(uint32_t	zaibo, uint32_t  send_timer);
// 			�������Ʋ�����   send_timer =����ʱ���(��λ΢��)  send_level =0,���������ͣ�send_level =1,����������
void			ir_no_modulation_send(uint32_t  send_timer , uint32_t  send_level);
//           ����һ�����ݣ�zaibo =�ز�ʱ��( ��λ΢��ز�=0�������ز����ͣ���0 �����ز�����)��
//			send_buf �������� ���飬send_count�����䳤��
//			��һ�����ݴ��ز����ڶ������������η���
void			ir_send(uint32_t	zaibo , uint32_t	*send_buf, uint32_t	send_count);
//          �������ģʽѡ =0 ����ģʽ  =1  ����ģʽ
void			ir_mode(uint8_t	mode);
//			���պ���,overtimer =���ճ�ʱ(��λ����) ��ʱ���˳� , zaibo_phase =�ز�ʱ�� (��λ΢��)
//           zaibo_phase =0 �������ز�ѧϰ����0 �������ز���ѧϰ
//          rec_buf = ��������   com_count =ָ���յ����ٸ��ߵͲ��κ��˳���return_count �˳�ʱ�յ����ٸߵͲ���
uint8_t		ir_receive(uint32_t overtimer,  uint32_t *zaibo_phase ,uint32_t *rec_buf , uint32_t  com_count ,uint32_t  *return_count);




#endif // 

