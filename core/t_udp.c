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

void
t_udp_remove(struct t_udp_pcb *pcb)
{
  struct t_udp_pcb *pcb2;
  /* pcb to be removed is first in list? */
  if (t_udp_pcbs == pcb) {
    /* make list start at 2nd pcb */
    t_udp_pcbs = t_udp_pcbs->next;
  /* pcb not 1st in list */
  } else for(pcb2 = t_udp_pcbs; pcb2 != NULL; pcb2 = pcb2->next) {
    /* find pcb in udp_pcbs list */
    if (pcb2->next != NULL && pcb2->next == pcb) {
      /* remove pcb from list */
      pcb2->next = pcb->next;
    }
  }
  t_memp_free(T_MEMP_UDP_PCB, pcb);
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

	}

	pcb->local_port = port;
	/* pcb not active yet? */
	if (rebind == 0) {
	/* place the PCB on the active list if not already there */
		pcb->next = t_udp_pcbs;
		t_udp_pcbs = pcb;
	}

	return 0;
}


int t_udp_connect(struct t_udp_pcb *pcb, struct t_ip_addr *ipaddr, u16_t port)
{
  struct t_udp_pcb *ipcb;

  if (pcb->local_port == 0) {
    err_t err = t_udp_bind(pcb, &pcb->local_ip, pcb->local_port);
    if (err != ERR_OK)
      return err;
  }

  t_ip_addr_set(&pcb->remote_ip, ipaddr);
  pcb->remote_port = port;
  pcb->flags |= T_UDP_FLAGS_CONNECTED;

/* Insert UDP PCB into the list of active UDP PCBs. */
  for(ipcb = t_udp_pcbs; ipcb != NULL; ipcb = ipcb->next) {
    if (pcb == ipcb) {
      /* already on the list, just return */
      return ERR_OK;
    }
  }

  ERROR(("t_udp_connect not find udp_pcb"));
  return -1;
}

/*
* @param [in] p: is need udp and layer before
*/
err_t
t_udp_send(struct t_udp_pcb *pcb, struct t_pbuf *p)
{
  struct t_udp_hdr *udphdr;
  struct t_netif *netif;
  struct t_ip_addr *src_ip;
  err_t err;
  struct t_pbuf *q; /* q will be sent down the stack */

if (t_pbuf_header(p, -T_UDP_HLEN)) {
	ERROR(("udp_send p is need udp and ip layer"));
	return -1;
}else{
	q=p;
}

	udphdr = q->payload;
  udphdr->src = htons(pcb->local_port);
  udphdr->dst = htons(pcb->remote_port);
  /* in UDP, 0 checksum means 'no checksum' */
  udphdr->chksum = 0x0000;

  /* find the outgoing network interface for this packet */
  netif = t_ip_route(&(pcb->remote_ip));

  src_ip = &(netif->ip_addr);
  udphdr->len = htons(q->tot_len);
  err = t_ip_output(netif,q, src_ip, &pcb->remote_ip, pcb->ttl, pcb->tos, T_IP_PROTO_UDP);
  t_pbuf_free(q);
  q = NULL;

  return 0;
}

void
t_set_udp_recv(struct t_udp_pcb *pcb,
   void (* recv)(void *arg, struct t_udp_pcb *upcb, struct t_pbuf *p,
           struct t_ip_addr *addr, u16_t port),
   void *recv_arg)
{
  /* remember recv() callback and user data */
  pcb->recv = recv;
  pcb->recv_arg = recv_arg;
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
			struct t_udp_pcb* pcb = NULL;
			u16_t src, dest;

			src = ntohs(udphdr->src);
			dest = ntohs(udphdr->dst);
			for (pcb = t_udp_pcbs; pcb != NULL; pcb = pcb->next) {
				if (((pcb->flags & T_UDP_FLAGS_CONNECTED)) &&
					(pcb->local_port == dest) &&
					!t_ip_addr_cmp(&(pcb->local_ip), &(iphdr->dst))){
					break;
				}
			}

			if (pcb != NULL && pcb->recv){
				pcb->recv(pcb->recv_arg, pcb, p, &(iphdr->src), src);	
			}


		}

	}

	return ret;
}
