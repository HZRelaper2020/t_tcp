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

#define T_UDP_FLAGS_NOCHKSUM 0x01U
#define T_UDP_FLAGS_UDPLITE  0x02U
#define T_UDP_FLAGS_CONNECTED  0x04U

int t_udp_init();

struct t_udp_pcb* t_udp_new();

int t_udp_input(struct t_netif* inp,struct t_pbuf* p);

err_t
t_udp_bind(struct t_udp_pcb *pcb, struct t_ip_addr *ipaddr, u16_t port);

int t_udp_connect(struct t_udp_pcb *pcb, struct t_ip_addr *ipaddr, u16_t port);

err_t
t_udp_send(struct t_udp_pcb *pcb, struct t_pbuf *p);

void
t_udp_remove(struct t_udp_pcb *pcb);

void
t_set_udp_recv(struct t_udp_pcb *pcb,
   void (* recv)(void *arg, struct t_udp_pcb *upcb, struct t_pbuf *p,
           struct t_ip_addr *addr, u16_t port),
   void *recv_arg);

void
t_set_udp_recv(struct t_udp_pcb *pcb,
   void (* recv)(void *arg, struct t_udp_pcb *upcb, struct t_pbuf *p,
           struct t_ip_addr *addr, u16_t port),
   void *recv_arg);

#endif
