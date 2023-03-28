#ifndef __ALL_DEFINE_H__
#define __ALL_DEFINE_H__

#include "MyCommon.h"

uint32_t SysTick_count; //ϵͳʱ�����

_st_Mpu MPU6050;   //MPU6050ԭʼ����
_st_Mag AK8975;
_st_AngE Angle;    //��ǰ�Ƕ���ֵ̬
_st_Remote Remote; //ң��ͨ��ֵ

_st_ALL_flag ALL_flag; //ϵͳ��־λ������������־λ��

PidObject pidRateX; //�ڻ�PID����
PidObject pidRateY;
PidObject pidRateZ;

PidObject pidPitch; //�⻷PID����
PidObject pidRoll;
PidObject pidYaw;

PidObject pidHeightRate;
PidObject pidHeightHigh;

void PID_ParamInit(void);

void ALL_Init(void)
{
	IIC_Init();				//I2C��ʼ��
	PID_ParamInit();		//PID������ʼ��
	LEDInit();				//LED���Ƴ�ʼ��
	MpuInit();              //MPU6050��ʼ��
	USART1_Config();        //��λ�����ڳ�ʼ��
	NRF24L01_init();		//2.4Gң��ͨ�ų�ʼ��
	TIM2_PWM_Config();		//4·PWM��ʼ��
	TIM3_PWM_Config();		//4·PWM��ʼ��	
	TIM4_Config();			//ϵͳ�������ڳ�ʼ�� 
}

void PID_ParamInit(void)
{
	pidRateX.kp = 2.0f;
	pidRateY.kp = 2.0f;
	pidRateZ.kp = 4.0f;

	pidRateX.ki = 0.0f;
	pidRateY.ki = 0.0f;
	pidRateZ.ki = 0.0f;	

	pidRateX.kd = 0.08f;
	pidRateY.kd = 0.08f;
	pidRateZ.kd = 0.5f;	

	pidPitch.kp	= 7.0f;
	pidRoll.kp	= 7.0f;
	pidYaw.kp	= 7.0f;	
}






#endif

