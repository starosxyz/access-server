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


#ifndef CPPPOE_H
#define CPPPOE_H

#include "aceinclude.h"
#include "CReferenceControl.h"
#include "STDInclude.h"
#include "pppoe.h"
#include "BaseDefines.h"
#include "CPPPOEDiscoveryHandler.h"
#include "CPPPOESessionHandler.h"
#include "CEtherIntf.h"
#include <string>
#include "ISessionManager.h"
#include "CAddSessionId.h"

#ifndef DEBUG_CONFIG_INTERFACE
#define DEBUG_CONFIG_INTERFACE
#endif 

#ifdef DEBUG_INTERFACE
#undef DEBUG_INTERFACE
#endif

#ifndef DEBUG_INTERFACE
#include "IAuthManager.h"
#endif

#ifdef DEBUG_INTERFACE
typedef struct
{
    WORD16 sessionId;
    CHAR userName[100];
    CHAR passwd[100];
} AUTH_REQUEST;

typedef struct
{
    WORD16 sessionId;
    WORD32 result;
    WORD32 subscriberIP;
    WORD32 primaryDNS;
    WORD32 secondaryDNS;
    CHAR reason[100];
} AUTH_RESPONSE;
#endif

#ifndef DEBUG_PKT // ���Ĵ�ӡ����
#define DEBUG_PKT
#endif



#ifndef DEBUG_PAP
#define DEBUG_PAP
#undef DEBUG_CHAP
#endif

#ifndef DEBUG_CHAP
#define DEBUG_CHAP
#undef DEBUG_PAP
#endif


class CClientDiscovery;
class CSession;
class CPPPOEEventHandler;
class CAddSessionId;

typedef std::unordered_map<WORD64, CCmAutoPtr<CClientDiscovery>> ClientDiscoveryType;
typedef std::unordered_map<WORD16, CCmAutoPtr<CSession>> SESSIONType;

/* ��MRU\MTU��˵��������PPPOE��PPP���ü�Э�̹������漰��3��MTU\MRUֵ:
 * 1.ȫ������VBUI��MTU��
 * 2.PPPOEЭ����ص�MRU(�������˵ĳ�ʼֵ������Э�̵õ���ֵ������PPPͬ)��RFC2516�涨���ó���1492��RFC4638�����˴���
 *   ����1492���㷨.
 * 3.PPPЭ����ص�MRU
 * ������Ϊ��Ϊ����ġ���ַ���RFC�Ĵ���:
 * ��ȫ�����õ�MTU��Ϊ��2��(����)PPPOEЭ�̵ĳ�ʼֵ������RFC4638���㷨����Э��(��ֲ��rp-pppoe������ʵ����RFC4638)��
 * ��Э�̵õ�������ֵ��Ϊ��3���ĳ�ʼֵ���ٽ���PPP MRU��ص�Э�̡�
 * ӡ���а���RFC4638�涨�����PPPЭ�̵õ���MRU����1492����Ҫ�ü�����ô���ı���Ľ�����֤��
 * �����ִ������£����client��pppoe tag��û��PPP-Max-Payload�����1��ֵ(VBUI MTU)����Ϊ��3���ĳ�ʼ��ֵ���������
 * ��Э�̾���PPP��صĴ����ˡ�
 *
 * ���е�ʵ�ַ���:
 * 1��2��������3��û��ʵ�֣���PPPOEЭ�̵���PPP-Max-Payloadû�д���PPP��Э��(ӡ����rp-pppoe�ǽ�Э�̵õ��Ĵ�ֵ����
 * pppd�ġ�)��3����1492��Ϊ��ʼ��ֵ��
 * ��ô�������ɣ�����PPP MRUЭ�̣����Э��˫���е���һ�����ڻ�С��1500��Э�̻���1500������������Ϊ1500�Ǳ���֧�ֵġ�
 * �˴�ѡ��1492�������ơ���Ϊ1492ҲӦ���Ǳ���֧�ֵġ�
 */
 
// Configurations
class IPPPOECfgInterface
{
public:    
    virtual ~IPPPOECfgInterface() {}

    // PPPoE related configurations
    virtual CHAR *GetACName() = 0;
    virtual CHAR *GetSvcName() = 0;
    virtual void SetACName(const CHAR acName[PPPOE_MAX_ACNAME_LENGTH]) = 0;
    virtual void SetSvcName(const CHAR svcName[PPPOE_MAX_SERVICE_NAME_LENGTH]) = 0;
    
    // VBUI related configurations
    virtual void GetVBUIIntfName(CHAR acIntfName[ETHER_INTF_NAME_SIZE+1]) = 0;
    virtual void SetVBUIIntfName(const CHAR acIntfName[ETHER_INTF_NAME_SIZE+1]) = 0;
    virtual WORD16 GetVBUIIntfMtu() = 0;
    virtual void SetVBUIIntfMtu(WORD16 mtu) = 0;
    virtual WORD32 GetVBUIIntfIP() = 0;  // In network byte order
    virtual void SetVBUIIntfIP(WORD32 ipAddr) = 0;
    virtual void SetVBUIIntfIP(std::string &ipAddr) = 0;

    // Authetication related configurations
    virtual void SetAuthType(uint16_t authType) = 0;  // The value of authType is the enumeraton of AIM_AUTH_TYPE in aim_ex.h
    virtual void SetHostName(std::string &hostName) = 0;    
};

class CPPPOE : public IAuthManagerSink, public ISessionManagerSink, public IPPPOECfgInterface
{
public:
    CPPPOE();
    virtual ~CPPPOE();

    void Init(const ACE_CString &ifname, const ACE_CString &ifip);
    int Start();

    CPPPOEDiscoveryHandler &GetPppoeDiscoveryHndl();
    CPPPOESessionHandler &GetPppoeSessionHndl();
    CEtherIntf &GetEtherIntf();
		
    // m_clientDiscoveries related operations
    int CreateClientDiscovery(BYTE clientMac[ETH_ALEN], CCmAutoPtr<CClientDiscovery> &discovery);
    int AddClientDiscovery(WORD64 clientMac, CCmAutoPtr<CClientDiscovery> &discovery);
    int RemoveClientDiscovery(WORD64 clientMac);
    int RemoveClientDiscovery(CCmAutoPtr<CClientDiscovery> &discovery);

    // m_sessionMgr related operations
    int CreateSession(CCmAutoPtr<CSession> &session);
    int AddSession(WORD16 sessionid, CCmAutoPtr<CSession> &session);
    int RemoveSession(WORD16 sessionid);
    int RemoveSession(CCmAutoPtr<CSession> &session);
    CSession * FindSession(WORD16 sessionid);

    WORD16 AllocId();
     void FreeId(uint16_t id);
     WORD32 GetIp();

    void OnLCPDown(WORD16 sessionId, const std::string &reason);
    void OnPPPOEAuthRequest(Auth_Request &authReq);
    int HandleAuthResponse(const Auth_Response *response);

    // For interface IAuthManagerSink
    int OnAuthResponse(const Auth_Response *response);

    // For interface ISessionManagerSink
    int OnAddUserResponse(const UM_RESPONSE &response);     // ��ɾ���û���Ӧ��ʱ����ʵ��
    int OnDeleteUserResponse(const UM_RESPONSE &response);
    int OnModifyUserResponse(const UM_RESPONSE &response);
	int OnKickUserNotify(const Sm_Kick_User* kickInfo);

    SWORD32 AddSubscriber(Session_User_Ex &sInfo);
    SWORD32 DelSubscriber(Session_Offline &sInfo);
    int HandleKickUserNotify(const Sm_Kick_User* kickInfo);

    // For interface IPPPOECfgInterface
    // PPPoE related configurations
    CHAR *GetACName();
    CHAR *GetSvcName();
    void SetACName(const CHAR acName[PPPOE_MAX_ACNAME_LENGTH]);
    void SetSvcName(const CHAR svcName[PPPOE_MAX_SERVICE_NAME_LENGTH]);
    // VBUI related configurations
    void GetVBUIIntfName(CHAR acIntfName[ETHER_INTF_NAME_SIZE+1]);
    void SetVBUIIntfName(const CHAR acIntfName[ETHER_INTF_NAME_SIZE+1]);
    WORD16 GetVBUIIntfMtu();
    void SetVBUIIntfMtu(WORD16 mtu);
    WORD32 GetVBUIIntfIP();
    void SetVBUIIntfIP(WORD32 ipAddr);
    void SetVBUIIntfIP(std::string &ipAddr);
    // Authetication related configurations
    void SetAuthType(uint16_t authType);  // The value of authType is the enumeraton of AIM_AUTH_TYPE in aim_ex.h
    void SetHostName(std::string &hostName);  
   

protected:
    void ClearAllClientDiscovery();
    void ClearAllSession();
                    
private:
    CPPPOEDiscoveryHandler m_DiscoveryHandle;
    CPPPOESessionHandler m_SessionHandle;
    ClientDiscoveryType m_clientDiscoveryMgr;  // ��ʱ��ʹ��m_clientDiscoveryMgr�����Ŀ��������m_sessionMgr������ÿ��
                                               // client���й�����������ֲ�����з�����ʱû�б�Ҫ
    SESSIONType m_sessionMgr;
    WORD16 m_nSessionIdBase;
    CEtherIntf m_etherIntf;
    CAddSessionId m_psessionid;
    // Configurations
    CHAR m_acName[PPPOE_MAX_ACNAME_LENGTH];         /* AC Name for pppoe negotiation */
    CHAR m_svcName[PPPOE_MAX_SERVICE_NAME_LENGTH];  /* Service Name for pppoe negotiation */
    uint16_t m_authType;
    std::string m_hostName;
    ACE_Thread_Mutex m_mutex4ClientDiscoveryMgr;  // Mutex for m_clientDiscoveryMgr
    ACE_Thread_Mutex m_mutex4SessionMgr;            // Mutex for m_sessionMgr
};

#endif//CPPPOE_H

