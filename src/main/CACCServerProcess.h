/***********************************************************************
 * Copyright (C) 2016-2023, StarOS.xyz
**********************************************************************/
#ifndef CACCSERVER_PROCESS_H
#define CACCSERVER_PROCESS_H
#include <dipc/dipc.h>

using namespace dipc;

class CAccServerProcess : public IDIPCProcessSink
{
public:
	CAccServerProcess();
	virtual ~CAccServerProcess();
	CAWResult Init();
public:
	/* dipc interace */
	virtual void OnProcessUpdateState(const CDIPCProcess& updateprocess);
	virtual void OnProcessShutdown();
	virtual void OnBootOK();
	virtual void OnProcessRun(int argc, char** argv, IDIPCProcess* dipcProcess);
	virtual void OnNodeConnected(const CDIPCNode& node);
	virtual void OnPeerConnected(const CDIPCApp& peeraddr);
	virtual void OnNodeDisconnect(const CDIPCNode& node);
	virtual void OnPeerDisconnect(const CDIPCApp& peeraddr);
	virtual void OnPacketRcv(CAWMessageBlock& msg, const CDIPCAddress& fromaddr);
	virtual void OnPacketRcvFromPeerNode(CAWMessageBlock& msg, const CDIPCAddress& fromaddr);
	virtual void OnPersistentdataChange(const CAWString& strurl);
    CAWResult SendToPeer(const char *pdata, size_t datasize, const CDIPCAddress peeraddr);
	inline IDIPCProcess *GetProcess() { return m_process; }
	void OnDataRcvFromPeer(CAWMessageBlock& msg, const CDIPCAddress& fromaddr);
private:
	IDIPCProcess* m_process;
};

#endif//CACCSERVER_PROCESS_H