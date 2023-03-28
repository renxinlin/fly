#include "stm32f10x.h"
#include "misc.h"

#include "MyCommon.h"


static volatile uint32_t usTicks = 0;

void CycleCounterInit(void)
{
    RCC_ClocksTypeDef clocks;
    RCC_GetClocksFreq(&clocks);
    usTicks = clocks.SYSCLK_Frequency / 1000000;
}

uint32_t GetSysTime_us(void) 
{
    register uint32_t ms, cycle_cnt;
	do 
	{
		ms = SysTick_count;
		cycle_cnt = SysTick->VAL;
	}while (ms != SysTick_count);
	
    return (ms * 1000) + (usTicks * 1000 - cycle_cnt) / usTicks;
}

//    ���뼶��ʱ����	 
void delay_ms(uint16_t nms)
{
	uint32_t t0=GetSysTime_us();
	while(GetSysTime_us() - t0 < nms * 1000);	  	  
}

void delay_us(unsigned int i)
 {  
	char x=0;   
    while( i--)
    {	
       for(x=1;x>0;x--);
    }
 }		  
