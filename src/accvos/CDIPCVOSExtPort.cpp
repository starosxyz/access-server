/***********************************************************************
 * Copyright (C) 2016-2023, StarOS.xyz
**********************************************************************/
#include "CDIPCVOSExtPort.h"
#include "fstack.h"
using namespace starbase;

static int extos_vport_output_eth_cb(struct vos_veth* veth, void* m)
{
    char buffer[1024 * 2];

    void* data = (void*)buffer;
    int len = vos_mbuf_getpktlen(m);
    int ret = vos_mbuf_copydata(m, data, 0, len);
    if (ret < 0) {
        vos_mbuf_free(m);
        return -1;
    }
    char *tmp = (char *)data+34;

    unsigned char type=0;
    unsigned char code=0;

    memcpy(&type,tmp,1);
    memcpy(&code,tmp+1,1);

	CDIPCVOSExtPort *pport= (CDIPCVOSExtPort*)vos_veth_get_cookie(veth);
	if(pport)
	{
        pport->ExtTapOutput(data,len);
	}
    return 0;
}


CDIPCVOSExtPort::CDIPCVOSExtPort()
	:m_extosveth(NULL),m_ext_tap_name(" "),m_ext_tap_gateway("0.0.0.0")
	,m_ext_tap_mask("255.255.0.0"),m_ext_tap_ip("0.0.0.0")
{
    CAW_INFO_TRACE_THIS("CDIPCVOSExtPort::CDIPCVOSExtPort");
}
CDIPCVOSExtPort::~CDIPCVOSExtPort()
{
    CAW_INFO_TRACE_THIS("CDIPCVOSExtPort::~CDIPCVOSExtPort");
}

CAWResult CDIPCVOSExtPort::ExtTapInit(const CAWString &ext_tap_name,
        const CAWString &ext_tap_gateway,
        CAWString ext_tap_mask,
        CAWString ext_tap_ip)
{
    CAW_INFO_TRACE("CDIPCVOSExtPort::ExtTapInit()");
    m_ext_tap_name(ext_tap_name);
    m_ext_tap_gateway(ext_tap_gateway);
	m_ext_tap_mask(ext_tap_mask);
    m_ext_tap_ip(ext_tap_ip);
    
    char* extname = m_ext_tap_name.c_str();
    uint64_t extos_ip = 0;
    uint64_t extos_mask = 0;
    uint64_t extos_gateway = 0;

    if((FALSE == CAWInetAddr::IpAddrStringTo4Bytes(m_ext_tap_ip,extos_ip))
        ||(FALSE == CAWInetAddr::IpAddrStringTo4Bytes(m_ext_tap_mask,extos_mask))
        ||(FALSE == CAWInetAddr::IpAddrStringTo4Bytes(m_ext_tap_gateway,extos_gateway))
        )
    {
        CAW_ERROR_TRACE("CDIPCVOSExtPort::ExtTapInit() Ip or Mask or Gateway address translation failed");
        return CAW_ERROR_FAILURE;
    }

    vos_in_addr_t ipaddr = (uint32_t)extos_ip;
    vos_in_addr_t netmask = (uint32_t)extos_mask;
    vos_in_addr_t broadcast = 0;
    vos_in_addr_t gateway = (uint32_t)extos_gateway;
    
    char macoctect[6] = {0};
    uint32_t mac_one;
    char str[128] = {0};
    uint32_t count = 0;
    srand((uint32_t)time(NULL));
    for(int i=0; i<6; i++)
    {
        mac_one = (uint32_t)rand() % (uint32_t)256;
        macoctect[i] = (char)mac_one;
        count = count+sprintf(&str[count],"%x%x:",(mac_one/(uint32_t)16),(mac_one%(uint32_t)16));
    }
    str[count-1] = '\0';


    m_extosveth = create_vos_veth(&ipaddr, &netmask, &broadcast, &gateway, macoctect, extname, extos_vport_output_eth_cb, this);
    if(m_extosveth)
        CAW_INFO_TRACE("CDIPCVOSExtPort::ExtTapInit() Successful");
    return 0;
}

void CDIPCVOSExtPort::ExtTapInput(const char* data, size_t datasize)
{
    CAW_INFO_TRACE("CDIPCVOSExtPort::ExtTapInput, datasize="<<datasize);
    if (m_extosveth)
    {
        vos_veth_input(m_extosveth, data, datasize, 0);
    }
}

