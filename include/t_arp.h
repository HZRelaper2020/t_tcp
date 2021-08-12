#ifndef SOURCE_T_ARP__H
#define SOURCE_T_ARP__H

#define T_ARP_TABLE_SIZE		20


#define T_ARP_REQUEST 			0x0001
#define T_ARP_REPLY			0x0002

#define T_HWTYPE_ETHER			0x1

struct t_etharp_hdr{
	struct t_ether_hdr ethhdr;
	uint16_t hwtype;
	uint16_t proto;
	uint16_t hwlen_protolen;
	uint16_t opcode;
	uint8_t sender_mac[6];
	struct t_ip_addr sender_ip;
	uint8_t target_mac[6];
	struct t_ip_addr target_ip;

};

void t_arp_init();

t_err_t t_arp_input(struct t_pbuf *p,struct t_netif* inp);

int t_arp_request(struct t_netif* inp,struct t_ip_addr* ip);


#endif
