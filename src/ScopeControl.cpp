/*
 * ScopeControl.cpp
 *
 *  Created on: Feb 21, 2016
 *      Author: zaqc
 */

#include <unistd.h>
#include <stdlib.h>
#include <memory.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#include "ScopeControl.h"
//----------------------------------------------------------------------------

void * scope_recv_data_thread(void *arg) {

	((ScopeControl*) arg)->RecvData();
	return NULL;
}

//============================================================================
//	ScopeControl
//============================================================================
ScopeControl::ScopeControl() {
	m_DataReady = false;
	m_DataSize = 0;
}
//----------------------------------------------------------------------------

ScopeControl::~ScopeControl() {
}
//----------------------------------------------------------------------------

void ScopeControl::RecvData(void) {

	socklen_t addr_len = sizeof(sockaddr_in);
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(11744);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	char buf[0x4000];
	while (true) {
		int br = recvfrom(m_RecvSock, buf, 0x4000, 0, (sockaddr*) &addr, &addr_len);
		if (br <= 0)
			break;

		pthread_mutex_lock(&m_Lock);
		if (!m_ThreadRunning) {
			pthread_mutex_unlock(&m_Lock);
			break;
		}
		memcpy(m_RecvData, buf, br);
		m_DataSize = br;
		m_DataReady = true;
		pthread_mutex_unlock(&m_Lock);
	}
}
//----------------------------------------------------------------------------

int ScopeControl::GetRecvData(char *aBuf, int aBufSize) {

	int res = 0;
	pthread_mutex_lock(&m_Lock);
	if (m_DataReady && aBufSize >= m_DataSize) {
		memcpy(aBuf, m_RecvData, m_DataSize);
		res = m_DataSize;
		m_DataReady = false;
	}
	pthread_mutex_unlock(&m_Lock);

	return res;
}
//----------------------------------------------------------------------------

void ScopeControl::InitReceiver(void) {

	m_RecvSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_RecvSock <= 0) {
		throw "can't create socket";
	}

	sockaddr_in bind_addr;
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_port = htons(17012);
	bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (0 != bind(m_RecvSock, (sockaddr*) &bind_addr, sizeof(sockaddr_in))) {
		close(m_RecvSock);
		throw "can't bind to socket at port (INADDR_ANY)";
		throw;
	}

	if (0 != pthread_mutex_init(&m_Lock, NULL)) {
		close(m_RecvSock);
		throw "can't create mutex";
	}

	pthread_mutex_lock(&m_Lock);
	if (0 != pthread_create(&m_RecvThread, NULL, &scope_recv_data_thread, this)) {
		pthread_mutex_destroy(&m_Lock);
		close(m_RecvSock);
		m_ThreadRunning = false;
		pthread_mutex_unlock(&m_Lock);
		throw "can't create thread";
	}
	m_ThreadRunning = true;
	pthread_mutex_unlock(&m_Lock);
}
//----------------------------------------------------------------------------

void ScopeControl::DoneReceiver(void) {

	pthread_mutex_lock(&m_Lock);
	bool running = m_ThreadRunning;
	m_ThreadRunning = false;
	shutdown(m_RecvSock, SHUT_RDWR);
	pthread_mutex_unlock(&m_Lock);

	if (running)
		pthread_join(m_RecvThread, NULL);

	pthread_mutex_destroy(&m_Lock);

	close(m_RecvSock);
}
//----------------------------------------------------------------------------

void ScopeControl::InitCtrlSocket(void) {
	m_CtrlSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	m_CtrlAddr.sin_family = AF_INET;
	m_CtrlAddr.sin_port = htons(11744);
	m_CtrlAddr.sin_addr.s_addr = inet_addr("192.168.1.44");
}
//----------------------------------------------------------------------------

void ScopeControl::CMD_SetVRC(int aCh, int aLoopNum, int aA1, int aA2, int aLen) {

	int buf[6] = { (int) CMD_SET_VRC, aCh, aLoopNum, aA1, aA2, aLen };

	sendto(m_CtrlSock, buf, 6 * sizeof(int), 0, (sockaddr*) &m_CtrlAddr, sizeof(sockaddr_in));
}
//----------------------------------------------------------------------------

void ScopeControl::CMD_SetSync(int aSync) {

	int buf[32] = { (int) CMD_SET_SYNC, aSync };

	sendto(m_CtrlSock, buf, 2 * sizeof(int), 0, (sockaddr*) &m_CtrlAddr, sizeof(sockaddr_in));
}
//----------------------------------------------------------------------------

void ScopeControl::CMD_Pulse(void) {

	int buf[32] = { (int) CMD_PULSE };

	sendto(m_CtrlSock, buf, sizeof(int), 0, (sockaddr*) &m_CtrlAddr, sizeof(sockaddr_in));
}
//----------------------------------------------------------------------------

