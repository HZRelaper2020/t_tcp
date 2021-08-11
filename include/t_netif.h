#ifndef SOURCE_T_NETIF__H
#define SOURCE_T_NETIF__H

struct t_netif{
        struct t_netif* next;
        struct t_ip_addr ip_addr;
	struct t_ip_addr netmask;
	struct t_ip_addr gw;

	t_err_t (*input)(struct t_pbuf *p,struct t_netif* inp);

	t_err_t (*output)(struct t_netif *nif,struct t_pbuf* p);

//	uint32_t state;
	uint8_t hwaddr[6];
	uint16_t mtu;
	uint8_t flags;
	uint8_t link_type;
	uint8_t name[2];
	uint8_t num;
};

#define NETIF_FLAG_UP 			0x1U
#define NETIF_FLAG_BROADCAST 		0x2U
#define NETIF_FLAG_DHCP			0x8U
#define NETIF_FLAG_LINK_UP		0x10U

void t_netif_init();

int t_netif_set_mac(struct t_netif* netif,uint8_t* mac);

int t_netif_set_addr(struct t_netif* netif,struct t_ip_addr *ip,struct t_ip_addr *mask,struct t_ip_addr *gw);

int t_netif_add(struct t_netif *netif,t_err_t (*init)(struct t_netif* netif), \
                t_err_t (*output)(struct t_netif* netif,struct t_pbuf *p));

#endif
