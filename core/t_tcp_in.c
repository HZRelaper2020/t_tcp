#include "t_common.h"

static struct tcp_seg inseg;
static struct tcp_hdr *tcphdr;
static struct ip_hdr *iphdr;
static u32_t seqno, ackno;
static u8_t flags;
static u16_t tcplen;

static u8_t recv_flags;
static struct pbuf *recv_data;

struct tcp_pcb *tcp_input_pcb;

int t_tcp_input(struct t_netif* inp,struct t_pbuf* p)
{
        int ret = 0;
	struct tcp_pcb *pcb, *prev;
  struct tcp_pcb_listen *lpcb;
  u8_t hdrlen;
  err_t err;



  iphdr = p->payload;
  tcphdr = (struct tcp_hdr *)((u8_t *)p->payload + IPH_HL(iphdr) * 4);

  /* remove header from payload */
  if (pbuf_header(p, -((s16_t)(IPH_HL(iphdr) * 4))) || (p->tot_len < sizeof(struct tcp_hdr))) {
    /* drop short packets */
    LWIP_DEBUGF(TCP_INPUT_DEBUG, ("tcp_input: short packet (%u bytes) discarded\n", p->tot_len));
    TCP_STATS_INC(tcp.lenerr);
    TCP_STATS_INC(tcp.drop);
    pbuf_free(p);
    return;
  }

  /* Don't even process incoming broadcasts/multicasts. */
  if (ip_addr_isbroadcast(&(iphdr->dest), inp) ||
      ip_addr_ismulticast(&(iphdr->dest))) {
    pbuf_free(p);
    return;
  }

  hdrlen = TCPH_HDRLEN(tcphdr);
  pbuf_header(p, -(hdrlen * 4));

  /* Convert fields in TCP header to host byte order. */
  tcphdr->src = ntohs(tcphdr->src);
  tcphdr->dest = ntohs(tcphdr->dest);
  seqno = tcphdr->seqno = ntohl(tcphdr->seqno);
  ackno = tcphdr->ackno = ntohl(tcphdr->ackno);
  tcphdr->wnd = ntohs(tcphdr->wnd);

  flags = TCPH_FLAGS(tcphdr) & TCP_FLAGS;
  tcplen = p->tot_len + ((flags & TCP_FIN || flags & TCP_SYN)? 1: 0);

  /* Demultiplex an incoming segment. First, we check if it is destined
     for an active connection. */
  prev = NULL;

  for(pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next) {
	if (pcb->remote_port == tcphdr->src &&
       pcb->local_port == tcphdr->dest &&
       ip_addr_cmp(&(pcb->remote_ip), &(iphdr->src)) &&
       ip_addr_cmp(&(pcb->local_ip), &(iphdr->dest))) {

	   if (prev != NULL) {
  prev->next = pcb->next;
  pcb->next = tcp_active_pcbs;
  tcp_active_pcbs = pcb;
      }
      LWIP_ASSERT("tcp_input: pcb->next != pcb (after cache)", pcb->next != pcb);
      break;
    }
    prev = pcb;
  }


  if (pcb != NULL){
  }

	
  return ret;
}

