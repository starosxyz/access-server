/***********************************************************************
 * Copyright (C) 2016-2023, StarOS.xyz
**********************************************************************/
#include "star5gcore_extos.h"
#include <voskern/voskern.h>
#include "s5gc-app.h"
#include "s5gc_xos_thread.h"
#include "xos-tun.h"
extern uint32_t g_extvport_ip;
extern uint32_t g_extvport_ip_mask;
extern char g_extvportmac[6];
extern char g_extvport_vosif[16];
extern char g_extvport_ostap[16];
extern bool g_extvporenable;
extern bool g_extvport_app_main_secondary;
struct vos_veth* extosveth;

int extos_tap_fd;

xos_poll_t* extos_poll;

void extos_vport_input_eth_cb(const char* data, size_t datasize)
{
    printf("extos_vport_input_eth_cb, datasize=%d\n",datasize);
    if (extosveth)
    {
        vos_veth_input(extosveth, data, datasize, 0);
    }
}

int extos_vport_output_eth_cb(struct vos_veth* veth, void* m)
{
    char buffer[1024 * 2];

    xos_debug("vnet_output_eth_cb fd=%d\n", extos_tap_fd);
    if (extos_tap_fd == -1)
        return 0;

    void* data = (void*)buffer;
    int len = vos_mbuf_getpktlen(m);
    int ret = vos_mbuf_copydata(m, data, 0, len);
    if (ret < 0) {
        vos_mbuf_free(m);
        return -1;
    }
    //printf("extos_vport_output_eth_cb, datasize=%d\n",len);

    char *tmp = (char *)data+34;

    unsigned char type=0;
    unsigned char code=0;

    memcpy(&type,tmp,1);
    memcpy(&code,tmp+1,1);


    //printf("extos_vport_output_eth_cb  icmp type=%d,code=%d\n",type,code);
    
    if (write(extos_tap_fd, data, len) <= 0) {
        printf("extos_vport_output_eth_cb xos_write error\n");
    }
    return 0;

}
void extos_tun_recv_eth_cb(short when, xos_socket_t fd, void* data)
{
    char buffer[1024 * 2] = { 0 };
    ssize_t n = read(fd, buffer, sizeof(buffer));
    //printf("extos_tun_recv_eth_cb, sizeof(buffer)=%lld\n",n);
    if (n <= 0) {
        xos_log_message(XOS_LOG_WARN, xos_socket_errno, "xos_read() fd=%d n=%d failed",fd,n);
        return;
    }
    char *tmp = (char *)buffer+34;

    unsigned char type=0;
    unsigned char code=0;

    memcpy(&type,tmp,1);
    memcpy(&code,tmp+1,1);


    //printf("extos_tun_recv_eth_cb  icmp type=%d,code=%d\n",type,code);
    if (extosveth)
    {
        vos_veth_input(extosveth, buffer, n, 0);
    }

}
int extos_vport_open(void)
{
    char sub1[XOS_ADDRSTRLEN];
    xos_info("extos_vport_open\n");
    char* ifname = (char *)g_extvport_vosif;
    char *ostapname=(char *)g_extvport_ostap;
    if (g_extvport_app_main_secondary==true)
	{
        printf("g_extvport_app_main_secondary=%d\n",g_extvport_app_main_secondary);
		return 0;
	}
	if (g_extvporenable==false)
	{
		return 0;
	}

    vos_in_addr_t ipaddr = g_extvport_ip;
    vos_in_addr_t netmask = g_extvport_ip_mask;
    vos_in_addr_t broadcast = 0;
    vos_in_addr_t gw = g_extvport_ip;

    xos_sockaddr_t maskaddr;
    maskaddr.xos_sa_family = AF_INET;
    maskaddr.sin.sin_addr.s_addr = netmask;
    xos_info("upf_uevport_open netmask=%d[%s]\n",
        netmask, XOS_ADDR(&maskaddr, sub1));


    char macaddr[VOS_MAC_LEN];
    memcpy(macaddr, g_extvportmac, sizeof(g_extvportmac));

    xos_info("extos_vport_open %s ip=%d[%s]\n", ifname,
        netmask, XOS_ADDR(&maskaddr, sub1));
    extosveth = create_vos_veth(&ipaddr, &netmask, &broadcast, &gw, macaddr, ifname, extos_vport_output_eth_cb, NULL);
    
    
    /* Open Tun interface */
    extos_tap_fd = xos_tun_open(ostapname, XOS_MAX_IFNAME_LEN, 1);
    if (extos_tap_fd == INVALID_SOCKET) {
        xos_error("tun_open(dev:%s) failed", ostapname);
        return -1;
    }

    extos_poll = xos_os_pollset_add(s5gc_xos_thread()->os_pollset,
        XOS_POLLIN, extos_tap_fd, extos_tun_recv_eth_cb, NULL);
    xos_assert(extos_poll);

    xos_info("tun_open(dev:%s,fd:%d) ok", ostapname, extos_tap_fd);


    return 0;
}
int extos_vport_close(void)
{
    return 0;
}

