#include "t_common.h"

static struct t_tcp_seg inseg;
static struct t_tcp_hdr *tcphdr;
static struct t_ip_hdr *iphdr;
static u32_t seqno, ackno;
static u8_t flags;
static u16_t tcplen;

static u8_t recv_flags;
static struct t_pbuf *recv_data;

struct t_tcp_pcb *tcp_input_pcb;


static void
t_tcp_parseopt(struct t_tcp_pcb *pcb)
{
  u8_t c;
  u8_t *opts, opt;
  u16_t mss;

  opts = (u8_t *)tcphdr + T_TCP_HLEN;

  /* Parse the TCP MSS option, if present. */
  if(T_TCPH_HDRLEN(tcphdr) > 0x5) {
    for(c = 0; c < (T_TCPH_HDRLEN(tcphdr) - 5) << 2 ;) {
      opt = opts[c];
      if (opt == 0x00) {
        /* End of options. */
  break;
      } else if (opt == 0x01) {
        ++c;
        /* NOP option. */
      } else if (opt == 0x02 &&
        opts[c + 1] == 0x04) {
        /* An MSS option with the right option length. */
        mss = (opts[c + 2] << 8) | opts[c + 3];
        pcb->mss = mss > T_TCP_MSS? T_TCP_MSS: mss;

        /* And we are done processing options. */
        break;
      } else {
  if (opts[c + 1] == 0) {
          /* If the length field is zero, the options are malformed
             and we don't process them further. */
          break;
        }
        /* All other options have a length field, so that we easily
           can skip past them. */
        c += opts[c + 1];
      }
    }
  }
}

static err_t
t_tcp_process(struct t_tcp_pcb *pcb)
{
  struct t_tcp_seg *rseg;
  u8_t acceptable = 0;
  err_t err;


  err = 0;
  /* Update the PCB (in)activity timer. */
  pcb->tmr = tcp_ticks;
  pcb->keep_cnt = 0;

  switch (pcb->state) {
  case SYN_SENT:
  if ((flags & TCP_ACK) && (flags & TCP_SYN)
        && ackno == ntohl(pcb->unacked->tcphdr->seqno) + 1) {
      pcb->snd_buf ++;
      pcb->rcv_nxt = seqno + 1;
      pcb->lastack = ackno;
      pcb->snd_wnd = tcphdr->wnd;
      pcb->snd_wl1 = seqno - 1; /* initialise to seqno - 1 to force window update */
      pcb->state = ESTABLISHED;
      pcb->cwnd = pcb->mss;
      --pcb->snd_queuelen;
      //LWIP_DEBUGF(TCP_QLEN_DEBUG, ("tcp_process: SYN-SENT --queuelen %u\n", (unsigned int)pcb->snd_queuelen));
      rseg = pcb->unacked;
      pcb->unacked = rseg->next;
      t_tcp_seg_free(rseg);

      /* Parse any options in the SYNACK. */
      t_tcp_parseopt(pcb);

      /* Call the user specified function to call when sucessfully
       * connected. */
      T_TCP_EVENT_CONNECTED(pcb, ERR_OK, err);

      t_tcp_ack_now(pcb);
    }
    break;
  }


  return err;
}

int t_tcp_input(struct t_netif* inp,struct t_pbuf* p)
{
        int ret = 0;
	struct t_tcp_pcb *pcb, *prev;
//  struct tcp_pcb_listen *lpcb;
  u8_t hdrlen;
  err_t err;



  iphdr = p->payload;
  tcphdr = (struct t_tcp_hdr *)((u8_t *)p->payload + T_IPH_HL(iphdr) * 4);

  /* remove header from payload */
  if (t_pbuf_header(p, ((s16_t)(T_IPH_HL(iphdr) * 4))) || (p->tot_len < sizeof(struct t_tcp_hdr))) {
    /* drop short packets */
    ret = -1;
    return ret;
  }

  /* Don't even process incoming broadcasts/multicasts. */
#if 0
  if (ip_addr_isbroadcast(&(iphdr->dst)) ||
      ip_addr_ismulticast(&(iphdr->dst))) {
    return -1;
  }
#endif

  hdrlen = T_TCPH_HDRLEN(tcphdr);
  t_pbuf_header(p, (hdrlen * 4));

  /* Convert fields in TCP header to host byte order. */
  tcphdr->src = ntohs(tcphdr->src);
  tcphdr->dest = ntohs(tcphdr->dest);
  seqno = tcphdr->seqno = ntohl(tcphdr->seqno);
  ackno = tcphdr->ackno = ntohl(tcphdr->ackno);
  tcphdr->wnd = ntohs(tcphdr->wnd);

  flags = T_TCPH_FLAGS(tcphdr) & TCP_FLAGS;
  tcplen = p->tot_len + ((flags & TCP_FIN || flags & TCP_SYN)? 1: 0);

  /* Demultiplex an incoming segment. First, we check if it is destined
     for an active connection. */
  prev = NULL;

  for(pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next) {
	if (pcb->remote_port == tcphdr->src &&
       pcb->local_port == tcphdr->dest &&
       !t_ip_addr_cmp(&(pcb->remote_ip), &(iphdr->src)) &&
       !t_ip_addr_cmp(&(pcb->local_ip), &(iphdr->dst))) {

	   if (prev != NULL) {
  prev->next = pcb->next;
  pcb->next = tcp_active_pcbs;
  tcp_active_pcbs = pcb;
      }
      break;
    }
    prev = pcb;
  }


  if (pcb != NULL){
	  /* Set up a tcp_seg structure. */
    inseg.next = NULL;
    inseg.len = p->tot_len;
    inseg.dataptr = p->payload;
    inseg.p = p;
    inseg.tcphdr = tcphdr;

    recv_data = NULL;
    recv_flags = 0;

    tcp_input_pcb = pcb;

    err = t_tcp_process(pcb);

    if (err == ERR_OK) {
  	t_tcp_output(pcb);
    }

    tcp_input_pcb = NULL;
  }

	
  return ret;
}

