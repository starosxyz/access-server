/***********************************************************************
	Copyright (c) 2017, The OpenBRAS project authors. All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are
	met:

	  * Redistributions of source code must retain the above copyright
		notice, this list of conditions and the following disclaimer.

	  * Redistributions in binary form must reproduce the above copyright
		notice, this list of conditions and the following disclaimer in
		the documentation and/or other materials provided with the
		distribution.

	  * Neither the name of OpenBRAS nor the names of its contributors may
		be used to endorse or promote products derived from this software
		without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

**********************************************************************/


#include "CRadiusConnector.h"
#include "radius.h"
#include "CRadiusData.h"
#include <arpa/inet.h>
#include "CRadiusScheme.h"


CRadiusConnector::CRadiusConnector(CRadiusScheme *scheme)
    :m_handler(ACE_INVALID_HANDLE)
    ,m_pscheme(scheme)
    ,m_accessRequest(0)
    ,m_accessAccept(0)
    ,m_accessReject(0)
    ,m_accountingRequest(0)
    ,m_accountingResponse(0)
    ,m_nIdentifierBase(0)
{
    ACE_DEBUG ((LM_DEBUG,"(%P|%t) CRadiusConnector::CRadiusConnector\n"));
}

CRadiusConnector::~CRadiusConnector()
{
    ACE_DEBUG ((LM_DEBUG,"(%P|%t) CRadiusConnector::~CRadiusConnector\n"));
}

//Start Connect
int CRadiusConnector::StartConnect(const ACE_INET_Addr &peeraddr)
{
    m_peeraddr = peeraddr;
    
    ACE_DEBUG ((LM_DEBUG, "CRadiusConnector::StartConnect\n"));
    
    ACE_TCHAR localaddr_str[BUFSIZ]={0};
    m_peeraddr.addr_to_string (localaddr_str, sizeof localaddr_str);
    ACE_DEBUG ((LM_DEBUG,"(%P|%t) CRadiusConnector::StartConnect, peeraddr=%s\n", localaddr_str));
    
    m_handler = ACE_OS::socket (AF_INET, SOCK_DGRAM, 0);
    if (m_handler == ACE_INVALID_HANDLE)
    {
        ACE_DEBUG ((LM_ERROR,"(%P|%t) CRadiusConnector::StartConnect, handle error\n"));
        return -1;
    }

    int one = 1;
    if (ACE_OS::setsockopt (m_handler,
                            SOL_SOCKET,
                            SO_REUSEADDR,
                            (const char*) &one,
                            sizeof one) == -1)
      {
          ACE_OS::closesocket(m_handler);
          m_handler = ACE_INVALID_HANDLE;
          ACE_DEBUG ((LM_ERROR,"(%P|%t) CRadiusConnector::StartConnect, setsockopt SO_REUSEADDR error\n"));
          return -1;
      }

#if 0
    if ((ACE_Addr&)m_localaddr == ACE_Addr::sap_any)
    {
        if (ACE::bind_port (m_handler,
            INADDR_ANY,
            AF_INET) == -1)
        {
            ACE_OS::closesocket(m_handler);
            m_handler = ACE_INVALID_HANDLE;
            ACE_DEBUG ((LM_ERROR,"(%P|%t) CPortalClient::StartListen, bind_port  error\n"));
            return -1;
        }
    }
    else
    {
        if (ACE_OS::bind (m_handler,
                                 reinterpret_cast<sockaddr *> (m_localaddr.get_addr ()),
                                 m_localaddr.get_size ()) == -1)
        {
            ACE_OS::closesocket(m_handler);
            m_handler = ACE_INVALID_HANDLE;
            ACE_ERROR((LM_ERROR,
                     ACE_TEXT ("(%P|%t) CPortalClient::StartListen: %p\n"),
                     ACE_TEXT ("bind error")));

            return -1;
        }
    }
#endif
    int size = 262144; // 256 * 1024 = 262144
    
    if (ACE_OS::setsockopt(m_handler, SOL_SOCKET,
                                    SO_RCVBUF,
                                    (char *) &size,
                                    sizeof (size)) == -1)
    {
        ACE_OS::closesocket(m_handler);
        m_handler = ACE_INVALID_HANDLE;
        ACE_DEBUG ((LM_ERROR,"(%P|%t) CRadiusConnector::StartConnect, setsockopt SO_RCVBUF error. size = %d\n", size));
        return -1;
    }

    if (ACE_OS::setsockopt(m_handler, SOL_SOCKET,
                                    SO_SNDBUF,
                                    (char *) &size,
                                    sizeof (size)) == -1)
    {
        ACE_OS::closesocket(m_handler);
        m_handler = ACE_INVALID_HANDLE;
        ACE_DEBUG ((LM_ERROR,"(%P|%t) CRadiusConnector::StartConnect, setsockopt SO_SNDBUF error. size = %d\n", size));
        return -1;
    }

    ACE_DEBUG ((LM_DEBUG,"(%P|%t) CRadiusConnector::StartConnect, register_handler\n"));

    // Register with the reactor for input.
    if (ACE_Reactor::instance()->register_handler (this,ACE_Event_Handler::READ_MASK) == -1)
    {
        ACE_OS::closesocket(m_handler);
        m_handler = ACE_INVALID_HANDLE;
        ACE_ERROR_RETURN ((LM_ERROR,
                 ACE_TEXT ("(%P|%t) CRadiusConnector::StartConnect: %p\n"),
                 ACE_TEXT ("register_handler for input")),
                -1);
    }
    return 0;
}

//Stop Connect
int CRadiusConnector::StopConnect()
{
    ACE_DEBUG ((LM_DEBUG,"(%P|%t) CRadiusConnector::StopConnect\n"));

    ACE_DEBUG ((LM_DEBUG,ACE_TEXT ("(%P|%t) CRadiusConnector::StopConnect \n")));

    if (ACE_Reactor::instance())
    {
        ACE_DEBUG ((LM_DEBUG,ACE_TEXT ("(%P|%t) CRadiusConnector::StopConnect remove_handler\n")));
        ACE_Reactor::instance()->remove_handler (this,
                                                ACE_Event_Handler::ALL_EVENTS_MASK |
                                                ACE_Event_Handler::DONT_CALL);
    }
    
    if (m_handler != ACE_INVALID_HANDLE)
    {
        ACE_DEBUG ((LM_DEBUG,ACE_TEXT ("(%P|%t) CRadiusConnector::StopConnect close socket \n"))); 
        ACE_OS::closesocket(m_handler);
        m_handler = ACE_INVALID_HANDLE;
    }

    return 0;
}

//Timeout Handle
int CRadiusConnector::handle_timeout (const ACE_Time_Value &current_time,
                          const void *act)
{
    ACE_DEBUG ((LM_DEBUG,"(%P|%t) CRadiusConnector::handle_timeou"));


    return 0;
}
//Find Transaction                          
int CRadiusConnector::FindTransaction(uint8_t id, CCmAutoPtr<CRadiusTransaction> &trans)
{
    ACE_GUARD_RETURN (ACE_Thread_Mutex, g, m_mutex, -1);
    std::unordered_map<uint8_t, CCmAutoPtr<CRadiusTransaction>>::iterator it = m_trans.find(id);
    if (it == m_trans.end())
    {
        return -1;
    }

    trans = it->second;

    return 0;
}

//Input Handle
int CRadiusConnector::handle_input (ACE_HANDLE fd)
{
    int result = -1;
    size_t temp_len = 0;

    RawMessage rawMsg;

    ACE_INET_Addr addrRecv;
    int addr_len = addrRecv.get_size ();
    ssize_t rval_recv = ACE_OS::recvfrom(fd, reinterpret_cast<char *>(rawMsg.buffer), RadiusMaxPacketSize,0, (sockaddr *)addrRecv.get_addr(),(int*)&addr_len);
    addrRecv.set_size (addr_len);
    ACE_TCHAR remote_str[80]={0};
    addrRecv.addr_to_string (remote_str, 80);

    ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("(%P|%t) CRadiusConnector::handle_input - ")
              ACE_TEXT ("activity occurred on handle %d!,%s\n"),
              m_handler,
              remote_str));
    
    ACE_DEBUG ((LM_INFO,
            ACE_TEXT ("(%P|%t) CRadiusConnector::handle_input - ")
            ACE_TEXT ("message from %d bytes received.\n"),
            rval_recv));

    if ((rval_recv == -1) || (rval_recv == 0))
    {
          ACE_ERROR_RETURN ((LM_ERROR,
                             ACE_TEXT ("(%P|%t) CRadiusConnector::handle_input - ")
                             ACE_TEXT ("closing daemon (fd = %d)\n"),
                             this->get_handle ()),
                            0); 
    }

    std::string shared_key = GetSharedKey();
    const char *secret = shared_key.c_str();
    
    CRadiusData msgRecvd(rawMsg.buffer,ntohs(rawMsg.msgHdr.length));

    uint8_t id=rawMsg.msgHdr.identifier;

    CCmAutoPtr<CRadiusTransaction> transaction;
    FindTransaction(id, transaction);
    if (transaction.Get() == NULL)
    {
        return 1;
    }
    const uint8_t* reqAuth = 0;//transaction->getAuthenticator();
    CRadiusMessage accessResp(msgRecvd,secret);
    if(!accessResp.verifyResponseAuthenticator(reqAuth,secret))
    {
        return 0;
    }

    transaction->RecvResponse(&accessResp);

    //RemoveTransaction(id);

    return 0;
}

//Remove Transaction
int CRadiusConnector::RemoveTransaction(uint8_t id)
{
    ACE_GUARD_RETURN (ACE_Thread_Mutex, g, m_mutex, -1); 
    m_trans.erase(id);
    return 0;
}

//Add Transaction
int CRadiusConnector::AddTransaction(uint8_t id, CCmAutoPtr<CRadiusTransaction> &trans)
{
    ACE_GUARD_RETURN (ACE_Thread_Mutex, g, m_mutex, -1); 
    std::unordered_map<uint8_t, CCmAutoPtr<CRadiusTransaction>>::iterator it = m_trans.find(id);
    if (it != m_trans.end())
    {
        return -1;
    }

    m_trans[id]=trans;

    return 0;   
}

//Get Handle
ACE_HANDLE CRadiusConnector::get_handle (void) const
{
    return m_handler;
}

//Close Handle
int CRadiusConnector::handle_close (ACE_HANDLE handle,
                        ACE_Reactor_Mask close_mask)
{
    return 0;
}

//Send Data
int CRadiusConnector::SendData(const char *data, size_t datasize)
{
    ACE_TCHAR remote_str[80]={0};

    m_peeraddr.addr_to_string (remote_str, 80);

    ssize_t result=ACE_OS::sendto (get_handle(),
                         (char const *)data,
                         datasize,
                         0,
                         (sockaddr const *) m_peeraddr.get_addr (),
                         m_peeraddr.get_size ());

    ACE_DEBUG ((LM_DEBUG,
            ACE_TEXT ("(%P|%t)  CPortalConnector::SendData ")
            ACE_TEXT ("to %s,fd=%d,datasize=%d,result=%d\n"),
            remote_str,
            get_handle(),
            datasize,
            result));
    if (result<0)
    {
        return -1;
    }
    return 0;
}

//Get Share Key
std::string CRadiusConnector::GetSharedKey()
{
    if (m_pscheme)
    {
        return m_pscheme->GetRadiusManager().GetConfig().GetSharedKey();
    }
    else
    {
        return std::string("");
    }
}

//Get Timeout
int CRadiusConnector::GetTimeOut()
{
    if (m_pscheme)
    {
        return m_pscheme->GetRadiusManager().GetConfig().GetResponseTimeout();
    }
    else
    {
        return 0;
    }
}

//Get Retrans
int CRadiusConnector::GetRetrans()
{
    if (m_pscheme)
    {
        return m_pscheme->GetRadiusManager().GetConfig().GetRetrans();
    }
    else
    {
        return 0;
    }
}

//Send Message
int CRadiusConnector::SendMessage(CRadiusMessage &accessReqMsg,TransactionResponse callback)
{
    char secret[256];
    strncpy(secret, GetSharedKey().c_str(), GetSharedKey().size()+1);

    uint8_t id = ++m_nIdentifierBase;
    accessReqMsg.setIdentifier(id);
    accessReqMsg.calcAuthenticator(secret);
    //const uint8_t* reqAuth = accessReqMsg.getAuthenticator();
    CCmAutoPtr<CRadiusTransaction> trans(new CRadiusTransaction(id,this,accessReqMsg, callback));
    if (AddTransaction(id,trans) == -1)
    {
        return -1;
    }

    return  SendData(reinterpret_cast<const char *>(accessReqMsg.data().buffer),
        ntohs(accessReqMsg.data().msgHdr.length));
}




