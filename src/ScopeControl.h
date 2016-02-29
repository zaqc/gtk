/*
 * ScopeControl.h
 *
 *  Created on: Feb 21, 2016
 *      Author: zaqc
 */

#ifndef SCOPECONTROL_H_
#define SCOPECONTROL_H_

#include <netinet/in.h>

#define	CMD_SET_VRC		0xAA550012
#define	CMD_SET_SYNC	0x5AA53FD1

#define CMD_PULSE		0x5A5A34DE

class ScopeControl {
private:
	int m_CtrlSock;
	sockaddr_in m_CtrlAddr;

	int m_RecvSock;
	bool m_ThreadRunning;
	pthread_t m_RecvThread;
	pthread_mutex_t m_Lock;

	char m_RecvData[0x4000];
	int m_DataSize;
	bool m_DataReady;

public:
	ScopeControl();
	virtual ~ScopeControl();

	void RecvData(void);
	int GetRecvData(char *aBuf, int aBufSize);

	void InitReceiver(void);
	void DoneReceiver(void);

	void InitCtrlSocket(void);

	void CMD_SetVRC(int aCh, int aLoopNum, int aA1, int aA2, int aLen);
	void CMD_SetSync(int aSync);	// 0-disable 1-external 2..50000-from 0.2Hz to 5000Hz

	void CMD_Pulse();	// use it only when Sync is disabled
};

#endif /* SCOPECONTROL_H_ */
