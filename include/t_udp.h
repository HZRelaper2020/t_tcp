#ifndef SOURCE_T_UDP__H
#define SOURCE_T_UDP__H

#define T_UDP_HLEN		8

struct t_udp_hdr{
	uint16_t src;
	uint16_t dst;
	uint16_t len;
	uint16_t chksum;
};


struct t_udp_pcb{
	struct t_udp_pcb* next;
};

int t_udp_init();

int t_udp_input(struct t_netif* inp,struct t_pbuf* p);

#endif
