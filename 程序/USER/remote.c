#include "MyCommon.h"
#include "Remote.h"
/*****************************************************************************************
 *  ͨ�����ݴ���
 * @param[in] 
 * @param[out] 
 * @return     
 ******************************************************************************************/	
	
uint8_t RC_rxData[32];
void remote_unlock(void);	

strRemoteConnect RemoteConnect;
void RC_Analy(void)  
{
	static uint16_t cnt;
	if(RemoteConnect.ConnectState == 1) RemoteConnect.ConnectTimeCnt++;
	
	//Receive  and check RC data 
	if(NRF24L01_RxPacket(RC_rxData)==SUCCESS)
	{ 
		RemoteConnect.ConnectState = 1;
		RemoteConnect.ConnectTimeCnt= 0 ;	
		uint8_t i;
		uint8_t CheckSum=0;
		cnt = 0;
		for(i=0;i<31;i++)
		{
			CheckSum +=  RC_rxData[i];
		}
		if(RC_rxData[31]==CheckSum && RC_rxData[0]==0xAA && RC_rxData[1]==0xAF)  //������յ���ң��������ȷ
		{
			Remote.roll		= ((uint16_t)RC_rxData[4]<<8) | RC_rxData[5];		//ͨ��1
			Remote.pitch	= ((uint16_t)RC_rxData[6]<<8) | RC_rxData[7];		//ͨ��2
			Remote.thr		= ((uint16_t)RC_rxData[8]<<8) | RC_rxData[9];		//ͨ��3
			Remote.yaw		= ((uint16_t)RC_rxData[10]<<8) | RC_rxData[11];		//ͨ��4
			Remote.AUX1		= ((uint16_t)RC_rxData[12]<<8) | RC_rxData[13];		//ͨ��5  ���Ͻǰ���������ͨ��5  
			Remote.AUX2		= ((uint16_t)RC_rxData[14]<<8) | RC_rxData[15];		//ͨ��6  ���Ͻǰ���������ͨ��6 
			Remote.AUX3		= ((uint16_t)RC_rxData[16]<<8) | RC_rxData[17];		//ͨ��7  ���±߰���������ͨ��7 
			Remote.AUX4		= ((uint16_t)RC_rxData[18]<<8) | RC_rxData[19];		//ͨ��8  ���±߰���������ͨ��6  
			LIMIT(Remote.AUX2	,1000,2000);		
			LIMIT(Remote.roll	,1000,2000);
			LIMIT(Remote.pitch	,1000,2000);
			LIMIT(Remote.thr	,1000,2000);
			LIMIT(Remote.yaw	,1000,2000);
			LIMIT(Remote.AUX1	,1000,2000);
			LIMIT(Remote.AUX2	,1000,2000);
			LIMIT(Remote.AUX1	,1000,2000);

			const float roll_pitch_ratio = 0.04f;
//				const float yaw_ratio =  0.0015f;    
			pidPitch.desired	= -(Remote.pitch-1500)*roll_pitch_ratio;	 //��ң��ֵ��Ϊ���нǶȵ�����ֵ
			pidRoll.desired		= -(Remote.roll-1500)*roll_pitch_ratio;
			if(Remote.yaw>1820)
			{
				pidYaw.desired += 0.75f;	
			}
			else if(Remote.yaw <1180)
			{
				pidYaw.desired -= 0.75f;	
			}						
			remote_unlock();	
		}
  }
}
uint8_t status = WAITING_1;
/*****************************************************************************************
 *  �����ж�
 * @param[in] 
 * @param[out] 
 * @return     
 ******************************************************************************************/	
void remote_unlock(void)
{
//volatile static uint8_t status = WAITING_1;
	static uint16_t cnt = 0;
	
	//����Ͽ�����ʱ���������ң��״̬�����˳�ң��״̬��ת�����״̬
	if((status == PROCESS_31) && (RemoteConnect.ConnectState == 0)) status = WAITING_1;
	
	if(Remote.thr<1200 &&Remote.yaw<1200)                         //����ң�����½�����
	{
		status = EXIT_255;
	}
	
	switch(status)
	{
		case WAITING_1:					//�ȴ�����
						{
							if(Remote.thr<1150) status = WAITING_2;         //���������࣬�������->�������->������� ����LED�Ʋ����� ����ɽ���		
						}
						break;
		case WAITING_2:					//������Ÿ�����
						{
							if(Remote.thr>1800)          
							{		
								cnt++;		
								if(cnt>5) //��������豣��200ms���ϣ���ֹң�ؿ�����ʼ��δ��ɵĴ�������
								{	
									cnt=0;
									status = WAITING_3;
								}
							}	
						}	
						break;
		case WAITING_3:					//������Ÿ���������������
						{
							if(Remote.thr<1150) 
							{
								ALL_flag.unlock = 1;	//���ý�����־
								LED.status = AlwaysOn;	//����LED����˸״̬
								status = PROCESS_31;								
							}								
						}			
						break;		
		case PROCESS_31:				//�������״̬
						{
							if(Remote.thr<1020)
							{
								if(cnt++ > 20000)							// ����ң�˴������60S�Զ�����
								{								
									status = EXIT_255;								
								}
							}
							else if(!ALL_flag.unlock) status = EXIT_255;	//Other conditions lock 
							else cnt = 0;
						}
						break;
		case EXIT_255:					//��������
						{
							cnt = 0;							
							LED.status = AllFlashLight;	                                 //exit
							LED.FlashTime = 100; //100*3ms		
							ALL_flag.unlock = 0;							
							status = WAITING_1;
						}
						break;
		default: status = EXIT_255;break;
	}
}
/***********************END OF FILE*************************************/







