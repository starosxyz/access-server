/***********************************************************************
 * Copyright (C) 2016-2023, StarOS.xyz
**********************************************************************/

#ifndef CACCSERVERCONFIG_H
#define CACCSERVERCONFIG_H
#include <starbase/CAWACEInclude.h>
#include <wface/CAWACEWrapper.h>

#include <starbase/CConfigFile.h>
#include <starbase/CAWString.h>


using namespace wface;
using namespace starbase;

#define JNO_MAX 32

class CAccServerConfig 
{

public:
    static CAccServerConfig *Instance();
    CAccServerConfig();
    ~CAccServerConfig();

    inline const CAWString &GetDipcMode() const { return m_dipc_mode; }    
    inline const CAWString &GetDipcZone() const { return m_dipc_zone; }
    inline uint32_t GetDipcJno() const { return m_dipc_jno; }
    inline int32_t* GetDipcPeerJno()  { return m_dipc_peer_jno; }
    inline const CAWString &GetDipcIp() const { return m_dipc_serverip; }
    inline const CAWString &GetDipcHostname() const { return m_dipc_hostname; }
    inline const CAWString &GetDipcPassword() const { return m_dipc_password; }
    inline const CAWString& GetBlackJsonFile() const { return m_strblackjson; }
    inline const CAWString& GeWhitesonFile() const { return m_strwhilejson; }
    inline bool GetAccessEnable() const { return m_isaccessenable; }

    inline uint32_t GetPolicy() const { return m_policy; }

    inline const CAWString &GetPPPNetaddr() const { return m_ppp_netaddr; }
    inline const CAWString &GetPPPNetmask() const { return m_ppp_netmask; }
    inline const CAWString &GetPPPGateway() const { return m_ppp_gateway; }
    inline uint32_t GetPPPPeerjno() const { return m_ppp_peerjno; }

    inline const CAWString &GetMPPPNetaddr() const { return m_mppp_netaddr; }
    inline const CAWString &GetMPPPNetmask() const { return m_mppp_netmask; }
    inline const CAWString &GetMPPPGateway() const { return m_mppp_gateway; }

    inline const CAWString &GetVswitchNetaddr() const { return m_vswitch_netaddr; }
    inline const CAWString &GetVswitchNetmask() const { return m_vswitch_netmask; }
    inline const CAWString &GetVswitchGateway() const { return m_vswitch_gateway; }

    inline const CAWString &GetDataChannelIp() const { return m_data_channel_ip; }

    inline bool IsOsTapRandomMac() {return m_random_mac;}
    inline const CAWString &GetOsTapMac() const { return m_ostap_mac; }
    inline const CAWString &GetOsTapIp() const { return m_ostap_ip; }
    inline const CAWString &GetOsTapMask() const { return m_ostap_mask; }
    inline const CAWString &GetOsTapGateway() const { return m_ostap_gateway; }
    inline const CAWString &GetOsTapName() const { return m_ostap_name; }

    inline const CAWString &GetVosTapMac() const { return m_vostap_mac; }
    inline const CAWString &GetVosTapIp() const { return m_vostap_ip; }
    inline const CAWString &GetVosTapMask() const { return m_vostap_mask; }
    inline const CAWString &GetVosTapGateway() const { return m_vostap_mask; }
    inline const CAWString &GetVosTapName() const { return m_vostap_name; }
protected:
    CAWResult readconfig();
private:
    CAWString m_dipc_mode;
    CAWString m_dipc_serverip;
    CAWString m_dipc_zone;
    uint32_t m_dipc_jno;
    int32_t m_dipc_peer_jno[JNO_MAX];
    CAWString m_dipc_hostname;
    CAWString m_dipc_password;
    /* b/w list*/
    CAWString m_strblackjson;
    CAWString m_strwhilejson;
    bool m_isaccessenable;

    uint32_t m_policy;

    CAWString m_ppp_netaddr;
    CAWString m_ppp_netmask;
    CAWString m_ppp_gateway;
    uint32_t m_ppp_peerjno;

    CAWString m_mppp_netaddr;
    CAWString m_mppp_netmask;
    CAWString m_mppp_gateway;

    CAWString m_vswitch_netaddr;
    CAWString m_vswitch_netmask;
    CAWString m_vswitch_gateway;

    CAWString m_data_channel_ip;

    bool m_random_mac;
    CAWString m_vostap_mac;
    CAWString m_vostap_ip;
    CAWString m_vostap_mask;
    CAWString m_vostap_gateway;
	CAWString m_vostap_name;
    
    CAWString m_ostap_mac;
    CAWString m_ostap_ip;
    CAWString m_ostap_mask;
    CAWString m_ostap_gateway;
	CAWString m_ostap_name;
};



#endif//CCPROXYCONFIG_H
