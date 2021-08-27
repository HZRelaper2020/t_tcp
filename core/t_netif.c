#include "t_common.h"
#include "t_netif.h"

struct t_netif *netif_list = NULL;
struct t_netif *netif_default = NULL;

void t_netif_init()
{
	netif_list = netif_default = NULL;
}

void t_netif_set_ipaddr(struct t_netif* netif,struct t_ip_addr *ip)
{
	netif->ip_addr.addr = ip->addr;
}

void t_netif_set_netmask(struct t_netif* netif,struct t_ip_addr *mask)
{
	netif->netmask.addr = mask->addr;
}

void t_netif_set_gw(struct t_netif* netif,struct t_ip_addr *gw)
{
	netif->gw.addr = gw->addr;
}

int t_netif_set_addr(struct t_netif* netif,struct t_ip_addr *ip,struct t_ip_addr *mask,struct t_ip_addr *gw)
{
	t_netif_set_ipaddr(netif,ip);
	t_netif_set_netmask(netif,mask);
	t_netif_set_gw(netif,gw);
	return 0;
}

int t_netif_set_mac(struct t_netif* netif,uint8_t* mac)
{
	for (int i=0;i<6;i++){
		netif->hwaddr[i] = *(mac+i);
	}
	netif->hwaddr_len = 6;
	return 0;
}

int t_netif_add(struct t_netif *netif,\
		t_err_t (*init)(struct t_netif* netif), \
		t_err_t (*output)(struct t_netif* netif,struct t_pbuf *p))
{
	static int netifnum = 0;
	netifnum += 1;

	netif->next = netif_list;
	netif_list = netif;

	netif_default = netif;

	init(netif);
	netif->output = output;

	return 0;	
}

void t_netif_remove(struct t_netif * netif)
{
	
}
