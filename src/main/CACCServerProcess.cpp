/***********************************************************************
 * Copyright (C) 2016-2023, StarOS.xyz
**********************************************************************/
#include "CAccServerProcess.h"


CAccServerProcess::CAccServerProcess()
    :m_process(NULL)
{

}
CAccServerProcess::~CAccServerProcess()
{

}
CAWResult CAccServerProcess::Init()
{
    return CAW_OK;
}
void CAccServerProcess::OnProcessUpdateState(const CDIPCProcess& updateprocess)
{

}
void CAccServerProcess::OnProcessShutdown()
{
}
void CAccServerProcess::OnBootOK()
{
}

/* main */
void CAccServerProcess::OnProcessRun(int argc, char** argv, IDIPCProcess* dipcProcess)
{
	m_process = dipcProcess;
	m_process->ProcessRunFinishNotify();
}


void CAccServerProcess::OnNodeConnected(const CDIPCNode& node)
{
    CAW_INFO_TRACE("CAccServerProcess::OnNodeConnected");
}

void CAccServerProcess::OnPeerConnected(const CDIPCApp& peeraddr)
{
    CDIPCAddress peer_address = peeraddr.GetDIPCAddress();
    CAW_INFO_TRACE("CAccServerProcess::OnPeerConnected, peer="<<peer_address.ToString());
}


void CAccServerProcess::OnNodeDisconnect(const CDIPCNode& node)
{
    CDIPCAddress peeraddr = node.GetNodeAddress();
    CAW_INFO_TRACE("CAccServerProcess::OnNodeDisconnect");
}

void CAccServerProcess::OnPeerDisconnect(const CDIPCApp& peeraddr)
{
    CDIPCAddress peer_address = peeraddr.GetDIPCAddress();
	CAW_INFO_TRACE("CAccServerProcess::OnPeerDisconnect, peer="<<peer_address.ToString());
}
void CAccServerProcess::OnDataRcvFromPeer(CAWMessageBlock& msg, const CDIPCAddress& fromaddr)
{
    size_t datasize = msg.GetChainedLength();
    const char* pdata = msg.GetTopLevelReadPtr();
}

void CAccServerProcess::OnPacketRcv(CAWMessageBlock& msg, const CDIPCAddress& fromaddr)
{
    size_t datasize = msg.GetChainedLength();
    const char* pdata = msg.GetTopLevelReadPtr();
}


void CAccServerProcess::OnPacketRcvFromPeerNode(CAWMessageBlock& msg, const CDIPCAddress& fromaddr)
{
    CAW_INFO_TRACE("CAccServerProcess::OnPacketRcvFromPeerNode");
}


void CAccServerProcess::OnPersistentdataChange(const CAWString& strurl)
{

}

CAWResult CAccServerProcess::SendToPeer(const char *pdata, size_t datasize, const CDIPCAddress peeraddr)
{
    if (m_process==NULL)
    {
        CAW_ERROR_TRACE("CStar5GCore::SendToPeer,m_process==NULL");
        return CAW_ERROR_FAILURE;
    }
    
    CDIPCAddress localdipcaddr;
    m_process->GetLocalDIPCAddr(localdipcaddr);
    CAW_INFO_TRACE("CAccServerProcess::SendToPeer,localaddr "<<localdipcaddr.ToString()
        <<" >>> to peeraddr "<<peeraddr.ToString()
        <<" ,datasize="<<datasize);

    CAWMessageBlock msgblock(
         datasize,
         (char *)pdata,
         CAWMessageBlock::DONT_DELETE,
         datasize
    );
    CAWResult rv = m_process->SendPacket(msgblock,peeraddr);
    if(CAW_FAILED(rv))
    {
        CAW_FATAL_TRACE("CAccServerProcess::SendToPeer Failed");
        return rv;
    }

    return CAW_OK;
}

