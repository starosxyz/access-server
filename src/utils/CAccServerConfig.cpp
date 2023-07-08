/***********************************************************************
 * Copyright (C) 2016-2023, StarOS.xyz
**********************************************************************/
#include "CAccServerConfig.h"
#include "starbase/CAWVOS.h"
#include "starbase/CAWInetAddr.h"
#include "starbase/CAWMacAddress.h"
#include <vos/vos_base.h>
#include <json/jsoncpp.h>
#include "utils/dipcvnetutils.h"

#ifdef CAW_RELEASE
#undef PRINT_INFO
#define PRINT_INFO
#endif

using namespace starbase;

static bool IsJson(const CAWString &strjson,Json::Value &json)
{
    Json::Reader reader;  

    if (strjson.size() == 0)
    {
        return false;
    }
    if (!reader.parse(strjson.c_str(), json))
    {
        return false;
    }
    if(!json.isObject())
    {
        return false;
    }
    
    return true;
}


CAccServerConfig *CAccServerConfig::Instance()
{
    return CAWSingletonT<CAccServerConfig>::Instance();
}

CAccServerConfig::CAccServerConfig()
{
    readconfig();
}

CAccServerConfig::~CAccServerConfig()
{
}

CAWResult CAccServerConfig::readconfig()
{
    
    CAWString projectspath = GetDIPCVNETHome();
    CAWString conffile = projectspath+"/conf/accserver.cfg";
    CConfigFile::Instance().Open(conffile);

    m_dipc_mode = CConfigFile::Instance().GetStringParam(CAWString("vnet"), CAWString("mode"), "ppp");
    m_dipc_serverip = CConfigFile::Instance().GetStringParam(CAWString("vnet"), CAWString("serverip"), "127.0.0.1:24445");
    m_dipc_zone = CConfigFile::Instance().GetStringParam(CAWString("vnet"), CAWString("zone"), "1.1.2.0");
    m_dipc_jno = CConfigFile::Instance().GetIntParam(CAWString("vnet"), CAWString("localjno"), 333);
    CAWString peer_jno_array =  CConfigFile::Instance().GetStringParam(CAWString("vnet"), CAWString("peerjno"), "333");
    m_dipc_hostname = CConfigFile::Instance().GetStringParam(CAWString("vnet"), CAWString("hostname"), "user2");
    m_dipc_password = CConfigFile::Instance().GetStringParam(CAWString("vnet"), CAWString("password"), "pass2");
    m_strblackjson = CConfigFile::Instance().GetStringParam(CAWString("vnet"), CAWString("blacklist"), "blacklist.json");
    m_strwhilejson = CConfigFile::Instance().GetStringParam(CAWString("vnet"), CAWString("whitelist"), "whitelist.json");
    m_isaccessenable = CConfigFile::Instance().GetBoolParam(CAWString("vnet"), CAWString("access_control"), true);;

    m_policy = CConfigFile::Instance().GetIntParam(CAWString("policy"), CAWString("policy"), 4);

    m_ppp_netaddr = CConfigFile::Instance().GetStringParam(CAWString("ppp"), CAWString("netaddr"), "192.168.1.1");
    m_ppp_netmask = CConfigFile::Instance().GetStringParam(CAWString("ppp"), CAWString("netmask"), "255.255.255.0");
    m_ppp_gateway = CConfigFile::Instance().GetStringParam(CAWString("ppp"), CAWString("gateway"), "192.168.1.2");
    m_ppp_peerjno = CConfigFile::Instance().GetIntParam(CAWString("ppp"), CAWString("peerjno"), 1234);;

    m_mppp_netaddr = CConfigFile::Instance().GetStringParam(CAWString("mppp"), CAWString("netaddr"), "192.168.1.2");
    m_mppp_netmask = CConfigFile::Instance().GetStringParam(CAWString("mppp"), CAWString("netmask"), "255.255.255.0");
    m_mppp_gateway = CConfigFile::Instance().GetStringParam(CAWString("mppp"), CAWString("gateway"), "192.168.1.2");

    m_vswitch_netaddr = CConfigFile::Instance().GetStringParam(CAWString("vswitch"), CAWString("netaddr"), "192.168.1.2");
    m_vswitch_netmask = CConfigFile::Instance().GetStringParam(CAWString("vswitch"), CAWString("netmask"), "255.255.255.0");
    m_vswitch_gateway = CConfigFile::Instance().GetStringParam(CAWString("vswitch"), CAWString("gateway"), "192.168.1.2");

    m_data_channel_ip = CConfigFile::Instance().GetStringParam(CAWString("datachannel"), CAWString("ip"), "192.168.1.1:8089");

    m_random_mac = CConfigFile::Instance().GetBoolParam(CAWString("vos"), CAWString("random_mac"), true);
    m_vostap_mac = CConfigFile::Instance().GetStringParam(CAWString("vos"), CAWString("vostap_mac"), "");
    m_vostap_ip = CConfigFile::Instance().GetStringParam(CAWString("vos"), CAWString("vostap_ip"), "10.1.1.2");
    m_vostap_mask = CConfigFile::Instance().GetStringParam(CAWString("vos"), CAWString("vostap_mask"), "255.255.0.0");
    m_vostap_gateway = CConfigFile::Instance().GetStringParam(CAWString("vos"), CAWString("vostap_gateway"), "10.1.1.1");
	m_vostap_name = CConfigFile::Instance().GetStringParam(CAWString("vos"), CAWString("vostap_name"), "vostap");

    m_ostap_mac = CConfigFile::Instance().GetStringParam(CAWString("vos"), CAWString("ostap_mac"), "");
    m_ostap_ip = CConfigFile::Instance().GetStringParam(CAWString("vos"), CAWString("ostap_ip"), "10.1.1.1");
    m_ostap_mask = CConfigFile::Instance().GetStringParam(CAWString("vos"), CAWString("ostap_mask"), "255.255.0.0");
    m_ostap_gateway = CConfigFile::Instance().GetStringParam(CAWString("vos"), CAWString("ostap_gateway"), "10.1.1.2");
	m_ostap_name = CConfigFile::Instance().GetStringParam(CAWString("vos"), CAWString("ostap_name"), "ostap");

	CConfigFile::Instance().Close();


    size_t found = m_dipc_serverip.find(':');        
    CAWString sub_dipc_ip = m_dipc_serverip.substr(0,found);
    CAW_INFO_TRACE("sub_dipc_ip"<<sub_dipc_ip.c_str()<<" found"<<found);
    if((found == std::string::npos)
        ||(FALSE == CAWInetAddr::IsIpAddress(sub_dipc_ip)))
    {
        CAW_FATAL_TRACE("CAccServerConfig Configuring [vnet] serverip ip is invalid, default format is 127.0.0.1:24445");
        exit(EXIT_FAILURE);
    }
    CAWString sub_dipc_port = m_dipc_serverip.substr(found+1,m_dipc_serverip.size()-found-1);
    int32_t port = atoi(sub_dipc_port.c_str());
    CAW_INFO_TRACE("sub_dipc_port"<<sub_dipc_port.c_str()<<" port"<<port); 
    if((port <= 0) || (port > 65535))
    {
        CAW_FATAL_TRACE("CAccServerConfig Configuring [vnet] serverip port is invalid, default format is 127.0.0.1:24445");
        exit(EXIT_FAILURE);
    }
    m_dipc_serverip = sub_dipc_ip + CAWString(":") + std::to_string(port).c_str();

    found = m_data_channel_ip.find(':');        
    CAWString sub_data_channel_ip = m_data_channel_ip.substr(0,found);
    if((found == std::string::npos)
        ||(FALSE == CAWInetAddr::IsIpAddress(sub_data_channel_ip)))
    {
        CAW_FATAL_TRACE("CAccServerConfig Configuring [datachannel] ip is invalid, default format is 192.168.1.1:8089");
        exit(EXIT_FAILURE);
    }
    CAWString sub_data_channel_port = m_data_channel_ip.substr(found+1,m_data_channel_ip.size()-found-1);
    port = atoi(sub_data_channel_port.c_str());
    if((port <= 0) || (port > 65535))
    {
        CAW_FATAL_TRACE("CAccServerConfig Configuring [datachannel] port is invalid, default format is 192.168.1.1:8089");
        exit(EXIT_FAILURE);
    }
    m_data_channel_ip = sub_data_channel_ip + CAWString(":") + std::to_string(port).c_str();

    if(FALSE == CAWInetAddr::IsIpAddress(m_ppp_netaddr))
    {
        CAW_FATAL_TRACE("CAccServerConfig Configuring [ppp] netaddr is an invalid IPv4 address, Default setting 192.168.1.1");
        m_ppp_netaddr = "192.168.1.1";
    }
    if(FALSE == CAWInetAddr::IsIpAddress(m_ppp_netmask))
    {
        CAW_FATAL_TRACE("CAccServerConfig Configuring [ppp] netmask is an invalid IPv4 address, Default setting 255.255.255.0");
        m_ppp_netmask = "255.255.255.0";
    }
    if(FALSE == CAWInetAddr::IsIpAddress(m_ppp_gateway))
    {
        CAW_FATAL_TRACE("CAccServerConfig Configuring [ppp] gateway is an invalid IPv4 address, Default setting 192.168.1.2");
        m_ppp_gateway = "192.168.1.2";
    }

    if(FALSE == CAWInetAddr::IsIpAddress(m_mppp_netaddr))
    {
        CAW_FATAL_TRACE("CAccServerConfig Configuring [mppp] netaddr is an invalid IPv4 address, Default setting 192.168.1.2");
        m_mppp_netaddr = "192.168.1.2";
    }
    if(FALSE == CAWInetAddr::IsIpAddress(m_mppp_netmask))
    {
        CAW_FATAL_TRACE("CAccServerConfig Configuring [mppp] netmask is an invalid IPv4 address, Default setting 255.255.255.0");
        m_mppp_netmask = "255.255.255.0";
    }
    if(FALSE == CAWInetAddr::IsIpAddress(m_mppp_gateway))
    {
        CAW_FATAL_TRACE("CAccServerConfig Configuring [mppp] gateway is an invalid IPv4 address, Default setting 192.168.1.2");
        m_mppp_gateway = "192.168.1.2";
    }
    
    if(FALSE == CAWInetAddr::IsIpAddress(m_vswitch_netaddr))
    {
        CAW_FATAL_TRACE("CAccServerConfig Configuring [vswitch] netaddr is an invalid IPv4 address, Default setting 192.168.1.2");
        m_vswitch_netaddr = "192.168.1.2";
    }
    if(FALSE == CAWInetAddr::IsIpAddress(m_vswitch_netmask))
    {
        CAW_FATAL_TRACE("CAccServerConfig Configuring [vswitch] netmask is an invalid IPv4 address, Default setting 255.255.255.0");
        m_vswitch_netmask = "255.255.255.0";
    }
    if(FALSE == CAWInetAddr::IsIpAddress(m_vswitch_gateway))
    {
        CAW_FATAL_TRACE("CAccServerConfig Configuring [vswitch] gateway is an invalid IPv4 address, Default setting 192.168.1.2");
        m_vswitch_gateway = "192.168.1.2";
    }

    if(FALSE == CAWInetAddr::IsIpAddress(m_vostap_ip))
    {
        CAW_FATAL_TRACE("CAccServerConfig Configuring [vos] vostap_ip is an invalid IPv4 address, Default setting 10.1.1.1");
        m_vostap_ip = "10.1.1.2";
    }
    if(FALSE == CAWInetAddr::IsIpAddress(m_vostap_mask))
    {
        CAW_FATAL_TRACE("CAccServerConfig Configuring [vos] vostap_mask is an invalid IPv4 address, Default setting 255.255.0.0");
        m_vostap_mask = "255.255.0.0";
    }
    if(FALSE == CAWInetAddr::IsIpAddress(m_vostap_gateway))
    {
        CAW_FATAL_TRACE("CAccServerConfig Configuring [vos] vostap_gateway is an invalid IPv4 address, Default setting 0.0.0.0");
        m_vostap_gateway = "10.1.1.1";
    }

    if(FALSE == CAWInetAddr::IsIpAddress(m_ostap_ip))
    {
        CAW_FATAL_TRACE("CAccServerConfig Configuring [vos] ostap_ip is an invalid IPv4 address, Default setting 10.1.1.2");
        m_ostap_ip = "10.1.1.1";
    }
    if(FALSE == CAWInetAddr::IsIpAddress(m_ostap_mask))
    {
        CAW_FATAL_TRACE("CAccServerConfig Configuring [vos] ostap_mask is an invalid IPv4 address, Default setting 255.255.0.0");
        m_ostap_mask = "255.255.0.0";
    }
    if(FALSE == CAWInetAddr::IsIpAddress(m_ostap_gateway))
    {
        CAW_FATAL_TRACE("CAccServerConfig Configuring [vos] ostap_gateway is an invalid IPv4 address, Default setting 0.0.0.0");
        m_ostap_gateway = "10.1.1.2";
    }

    std::cout<<"vnet mode="<<m_dipc_mode.c_str()<<"\n"
            <<"vnet serverip="<<m_dipc_serverip.c_str()<<"\n"
            <<"vnet zone="<<m_dipc_zone.c_str()<<"\n"
            <<"vnet localjno="<<m_dipc_jno<<"\n"
            <<"vnet peerjno="<<peer_jno_array.c_str()<<"\n"
            <<"vnet hostname="<<m_dipc_hostname.c_str()<<"\n"
            <<"vnet password="<<m_dipc_password.c_str()<<"\n"
            <<"vnet blacklist="<<m_strblackjson.c_str()<<"\n"
            <<"vnet whitelist="<<m_strwhilejson.c_str()<<"\n"
            <<"vnet access_control="<<m_isaccessenable<<"\n"<<std::endl;

    std::cout<<"policy policy="<<m_policy<<"\n"<<std::endl;

    std::cout<<"ppp netaddr="<<m_ppp_netaddr.c_str()<<"\n"
            <<"ppp netmask="<<m_ppp_netmask.c_str()<<"\n"
            <<"ppp gateway="<<m_ppp_gateway.c_str()<<"\n"
            <<"ppp peerjno="<<m_ppp_peerjno<<"\n"<<std::endl;

    std::cout<<"mppp netaddr="<<m_mppp_netaddr.c_str()<<"\n"
            <<"mppp netmask="<<m_mppp_netmask.c_str()<<"\n"
            <<"mppp gateway="<<m_mppp_gateway.c_str()<<"\n"<<std::endl;

    std::cout<<"vswitch netaddr="<<m_vswitch_netaddr.c_str()<<"\n"
            <<"vswitch netmask="<<m_vswitch_netmask.c_str()<<"\n"
            <<"vswitch gateway="<<m_vswitch_gateway.c_str()<<"\n"<<std::endl;

    std::cout<<"datachannel ip="<<m_data_channel_ip.c_str()<<"\n"<<std::endl;

    std::cout<<"vos random_mac="<<m_random_mac<<"\n"
            <<"vos vostap_mac="<<m_vostap_mac.c_str()<<"\n"
            <<"vos vostap_ip="<<m_vostap_ip.c_str()<<"\n"
            <<"vos vostap_mask="<<m_vostap_mask.c_str()<<"\n"
            <<"vos vostap_gateway="<<m_vostap_gateway.c_str()<<"\n"
            <<"vos vostap_name="<<m_vostap_name.c_str()<<"\n"
            <<"\n"
            <<"vos vostap_mac="<<m_ostap_mac.c_str()<<"\n"
            <<"vos vostap_ip="<<m_ostap_ip.c_str()<<"\n"
            <<"vos vostap_mask="<<m_ostap_mask.c_str()<<"\n"
            <<"vos vostap_gateway="<<m_ostap_gateway.c_str()<<"\n"
            <<"vos vostap_name="<<m_ostap_name.c_str()<<"\n"<<std::endl;


    for(int i=0; i<JNO_MAX; i++)
    {
        m_dipc_peer_jno[i] = (int32_t)-1;
    }

    size_t begin = 0;
    size_t end = 0;
    CAWString jnotmp;
    int32_t jnotmpint = 0;
    uint32_t count = 0;
    bool jno_is_have = false;
    while((end != std::string::npos)&&(count<JNO_MAX))
    {
        end = peer_jno_array.find_first_of("\t:,\0",begin);
        
        jnotmp = peer_jno_array.substr(begin,end-begin);
        jnotmpint = atoi(jnotmp.c_str());
        jno_is_have = false;
        if(jnotmpint)
        {
            for(int i=0; i<count; i++)
            {
                if(m_dipc_peer_jno[i] == jnotmpint)
                    jno_is_have = true;
            }
            if(!jno_is_have)
            {
                m_dipc_peer_jno[count] = jnotmpint;
                count++;
            }
        }
        printf("\tpeer_jno_array[%d]=%d\n"
            ,count,jnotmpint);
        begin = end+1;
    }

    return CAW_OK;
}
