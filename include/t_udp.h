#ifndef SOURCE_T_UDP__H
#define SOURCE_T_UDP__H

#define T_UDP_HLEN		8

#ifndef T_UDP_TTL
#define T_UDP_TTL	255
#endif

struct t_udp_hdr{
	uint16_t src;
	uint16_t dst;
	uint16_t len;
	uint16_t chksum;
};


struct t_udp_pcb{
	T_IP_PCB;
	struct t_udp_pcb* next;
	u8_t flags;
	u16_t local_port, remote_port;

	u16_t chksum_len;

	void (* recv)(void *arg, struct t_udp_pcb *pcb, struct t_pbuf *p,
	struct t_ip_addr *addr, u16_t port);
	void *recv_arg;
};

int t_udp_init();

struct t_udp_pcb* t_udp_new();

int t_udp_input(struct t_netif* inp,struct t_pbuf* p);

err_t
t_udp_bind(struct t_udp_pcb *pcb, struct t_ip_addr *ipaddr, u16_t port);

#endif
