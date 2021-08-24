#include "t_common.h"

static struct t_udp_pcb* t_udp_pcbs = NULL;

int t_udp_init()
{
	t_udp_pcbs = NULL;
	return 0;
}

struct t_udp_pcb* t_udp_new()
{
	struct t_udp_pcb* udp = t_memp_malloc(T_MEMP_UDP_PCB);
	if (udp == NULL){
		ERROR(("t_udp_new failed no memory"));
	}else{
		memset(udp,0,sizeof(struct t_udp_pcb));
		udp->ttl = T_UDP_TTL;
	}
	return udp;
}

err_t
t_udp_bind(struct t_udp_pcb *pcb, struct t_ip_addr *ipaddr, u16_t port)
{
	struct t_udp_pcb *ipcb;
	u8_t rebind;
	static int start_count = 0;
	rebind = 0;
	/* Check for double bind and rebind of the same pcb */
	for (ipcb = t_udp_pcbs; ipcb != NULL; ipcb = ipcb->next) {
	/* is this UDP PCB already on active list? */
		if (pcb == ipcb) {
	/* pcb already in list, just rebind */
			rebind = 1;
			break;
		}
	}

	t_ip_addr_set(&pcb->local_ip, ipaddr);


	if (port == 0) {
#ifndef UDP_LOCAL_PORT_RANGE_START
#define UDP_LOCAL_PORT_RANGE_START 4096
#define UDP_LOCAL_PORT_RANGE_END   0x7fff
#endif
		port = UDP_LOCAL_PORT_RANGE_START + start_count;
		start_count += 1;
		ipcb = t_udp_pcbs ;

		pcb->local_port = port;
		/* pcb not active yet? */
		if (rebind == 0) {
		/* place the PCB on the active list if not already there */
			pcb->next = t_udp_pcbs;
			t_udp_pcbs = pcb;
		}
	}

	return 0;
}

int t_udp_input(struct t_netif* inp,struct t_pbuf* p)
{
	int ret = 0;
	struct t_ip_hdr * iphdr = (struct t_ip_hdr*)p->payload;

	if (p->tot_len < T_UDP_HLEN + T_IPH_HL(iphdr)*4){
		ERROR(("t_udp_input:p->tot_len < T_UDP_HLEN + T_IPH_HL(iphdr)*4"));
		ret = -1;
	}else{
		t_pbuf_header(p,T_IPH_HL(iphdr)*4);
		struct t_udp_hdr* udphdr = (struct t_udp_hdr*) p->payload;

		if (p->tot_len < htons(udphdr->len)){
			ERROR(("t_udp_input:p->tot_len < udphdr->len"));
			ret = -1;
		}else{
			struct t_udp_pcb* pcb = t_udp_pcbs;
			while( pcb != NULL){
				pcb = pcb->next;
			}
		}

	}

	return ret;
}
