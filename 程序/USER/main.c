
#include "MyCommon.h"

void ALL_Init(void);
	
//�ر��������������˿տ��ĵش��������ڽ��з��С������ǳ�������������ɽ����ر�ң�ء�
int main(void)
{		
	CycleCounterInit();  								//�õ�ϵͳÿ��us��ϵͳCLK������Ϊ�Ժ���ʱ�������͵õ���׼�ĵ�ǰִ��ʱ��ʹ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//2��bit����ռ���ȼ���2��bit�������ȼ�
	SysTick_Config(SystemCoreClock / 1000);				//ϵͳ�δ�ʱ��

	ALL_Init();				//ϵͳ����Ӳ���ĳ�ʼ��

	while(1)
	{
		ANTO_polling();		//������λ��
		PilotLED();			//LEDˢ��
		if(ALL_flag.unlock == 1)
		{
			if((fabs(Angle.pitch) > 70)||(fabs(Angle.roll) > 70))		//���������ֹ�����б��ֹͣ����
			{
				RemoteConnect.RemoteSignalLost	= 0;
				RemoteConnect.RemSigLostTimeCnt = 0;
				RemoteConnect.ConnectTimeCnt	= 0;
				RemoteConnect.ConnectState		= 0;
				ALL_flag.unlock = 0; 
				LED.FlashTime = 100;
				LED.status = AllFlashLight;
				NRF24L01_init();	
				TIM2_PWM_Config();		//4·PWM��ʼ��
				TIM3_PWM_Config();		//4·PWM��ʼ��				
			}
		}
		if((RemoteConnect.ConnectTimeCnt > 200) && (RemoteConnect.ConnectState == 1))		//����״̬�£����ң��ʧ����300ms
		{
			//���3��û�յ�ң�����ݣ����ж�ң���źŶ�ʧ���ɿ����κ�ʱ��ֹͣ���У��������ˡ�
			//���������ʹ���߿ɽ����ر�ң�ص�Դ������������3��������رգ��������ˡ�
			//�����ر�ң�أ�����ڷ����л�ֱ�ӵ��䣬���ܻ��𻵷�������
			RemoteConnect.RemoteSignalLost	= 1;
			if(Remote.thr >= 1350) Remote.thr = 1350;
			else Remote.thr = 1000;			
		}		
	}
}

void TIM4_IRQHandler(void)   //TIM4�ж�3ms
{	
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		static uint8_t cnt_3ms = 0;
		static uint8_t cnt_6ms = 0;	 
		SysTick_count++;
		cnt_3ms++;
		cnt_6ms++;
		if(RemoteConnect.RemoteSignalLost == 1) 
		{
			RemoteConnect.RemSigLostTimeCnt++;		//�źŶ�ʧ���ƶ�ʧʱ��
			if(RemoteConnect.RemSigLostTimeCnt > 1200)
			{
				RemoteConnect.RemoteSignalLost	= 0;
				RemoteConnect.RemSigLostTimeCnt = 0;
				RemoteConnect.ConnectTimeCnt	= 0;
				RemoteConnect.ConnectState		= 0;
				ALL_flag.unlock = 0;
				LED.FlashTime = 300;
				LED.status = AllFlashLight;
				NRF24L01_init();	
				TIM2_PWM_Config();		//4·PWM��ʼ��
				TIM3_PWM_Config();		//4·PWM��ʼ��	
			}	
		}
		else RemoteConnect.RemSigLostTimeCnt = 0;
		if(cnt_3ms == 1)  //3ms����һ��
		{
			cnt_3ms = 0;
			MpuGetData();
			RC_Analy();			
			FlightPidControl(0.003f);
			MotorControl();
		}		
		if(cnt_6ms == 2) //6ms����һ��
		{
			cnt_6ms = 0;
			GetAngle(&MPU6050,&Angle,0.00626f);
		}		

		TIM_ClearITPendingBit(TIM4, TIM_IT_Update );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
	}
}








