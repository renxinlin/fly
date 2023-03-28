#include "MyCommon.h"
#include "control.h"
#include "pid.h"

//------------------------------------------------------------------------------
PidObject *(pPidObject[])={&pidRateX,&pidRateY,&pidRateZ,&pidRoll,&pidPitch,&pidYaw   //结构体数组，将每一个数组放一个pid结构体，这样就可以批量操作各个PID的数据了  比如解锁时批量复位pid控制数据，新手明白这句话的作用就可以了
		,&pidHeightRate
		,&pidHeightHigh
};
/**************************************************************
 *  flight control
 * @param[in] 
 * @param[out] 
 * @return     
 ***************************************************************/
void FlightPidControl(float dt)
{
	volatile static uint8_t status=WAITING_1;
//	status = PROCESS_31;	
	switch(status)
	{		
		case WAITING_1: //等待解锁
						if(ALL_flag.unlock)
						{
							status = READY_11;	
						}			
						break;
		case READY_11:  //准备进入控制
						{
							pidRest(pPidObject,8); //批量复位PID数据，防止上次遗留的数据影响本次控制
							Angle.yaw = pidYaw.desired =  pidYaw.measured = 0;   //锁定偏航角
							status = PROCESS_31;
						}
						break;			
		case PROCESS_31: //正式进入控制
						{
							pidRateX.measured = MPU6050.gyroX * Gyro_G; //内环测量值 角度/秒
							pidRateY.measured = MPU6050.gyroY * Gyro_G;
							pidRateZ.measured = MPU6050.gyroZ * Gyro_G;

							pidPitch.measured = Angle.pitch; //外环测量值 单位：角度
							pidRoll.measured = Angle.roll;
							pidYaw.measured = Angle.yaw;

							pidUpdate(&pidRoll,dt);    //调用PID处理函数来处理外环	横滚角PID		
							pidRateX.desired = pidRoll.out; //将外环的PID输出作为内环PID的期望值即为串级PID
							pidUpdate(&pidRateX,dt);  //再调用内环

							pidUpdate(&pidPitch,dt);    //调用PID处理函数来处理外环	俯仰角PID	
							pidRateY.desired = pidPitch.out;  
							pidUpdate(&pidRateY,dt); //再调用内环

							CascadePID(&pidRateZ,&pidYaw,dt);	//也可以直接调用串级PID函数来处理
						}
						break;
		case EXIT_255:  //退出控制
						{
							pidRest(pPidObject,8);
							status = WAITING_1;//返回等待解锁
						}
						break;
		default:	status = EXIT_255;break;
	}
	//意外情况，请使用遥控紧急上锁，飞控就可以在任何情况下紧急中止飞行，锁定飞行器，退出PID控制
	if(ALL_flag.unlock == EMERGENT) status = EXIT_255;
}

int16_t motor[4];
#define MOTOR1 motor[0] 
#define MOTOR2 motor[1] 
#define MOTOR3 motor[2] 
#define MOTOR4 motor[3] 

void MotorControl(void)
{	
	volatile static uint8_t status=WAITING_1;
		
	if(ALL_flag.unlock == EMERGENT) //意外情况，请使用遥控紧急上锁，飞控就可以在任何情况下紧急中止飞行，锁定飞行器，退出PID控制
		
	status = EXIT_255;

//	status = PROCESS_31;	
	switch(status)
	{		
		case WAITING_1: //等待解锁	
						{
							MOTOR1 = MOTOR2 = MOTOR3 = MOTOR4 = 0;  //如果锁定，则电机输出都为0
							if(ALL_flag.unlock)
							{
								status = WAITING_2;
							}
						}
		case WAITING_2: //解锁完成后判断使用者是否开始拨动遥杆进行飞行控制
						{
							if(Remote.thr>1100)
							{
								status = PROCESS_31;
							}
						}
						break;
		case PROCESS_31:
						{
							int16_t temp;
							
							temp = Remote.thr -1000 + pidHeightRate.out; //油门+定高输出值
							if(Remote.thr<1020)		//油门太低了，则限制输出  不然飞机乱转												
							{
								MOTOR1 = MOTOR2 = MOTOR3 = MOTOR4=0;
								break;
							}
							MOTOR1 = MOTOR2 = MOTOR3 = MOTOR4 = LIMIT(temp,0,900); //留100给姿态控制

							MOTOR1 +=    + pidRateX.out - pidRateY.out - pidRateZ.out;//; 姿态输出分配给各个电机的控制量
							MOTOR2 +=    + pidRateX.out + pidRateY.out + pidRateZ.out ;//;
							MOTOR3 +=    - pidRateX.out + pidRateY.out - pidRateZ.out;
							MOTOR4 +=    - pidRateX.out - pidRateY.out + pidRateZ.out;//;
						}	
						break;
		case EXIT_255:
						{
							MOTOR1 = MOTOR2 = MOTOR3 = MOTOR4 = 0;  //如果锁定，则电机输出都为0
							status = WAITING_1;	
						}
						break;
		default:		break;
	}	
	
	TIM2->CCR1 = LIMIT(MOTOR1,0,1000);  //更新PWM
	TIM2->CCR2 = LIMIT(MOTOR2,0,1000);
	TIM3->CCR1 = LIMIT(MOTOR3,0,1000);
	TIM3->CCR2 = LIMIT(MOTOR4,0,1000);
} 

/************************************END OF FILE********************************************/ 
