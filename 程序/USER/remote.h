#ifndef _REMOTE_H
#define _REMOTE_H


#define SUCCESS 0
#undef FAILED
	#define FAILED  1

#include <stdbool.h>
#include <stdint.h>

typedef struct tagRemoteConnect
{
	uint16_t	ConnectTimeCnt;		//����ʱ���ʱ��ÿ3ms�Լӣ�	
	uint8_t		ConnectState;		//1-���ӣ�0-δ����
	uint8_t		RemoteSignalLost;
	uint16_t		RemSigLostTimeCnt;

}strRemoteConnect;

extern strRemoteConnect RemoteConnect;		//����ʱ���ʱ��ÿ3ms�Լӣ�

extern void RC_Analy(void);

#endif

