#include "t_common.h"

static struct t_udp_pcb* t_udp_pcbs = NULL;

int t_udp_init()
{
	t_udp_pcbs = NULL;
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
