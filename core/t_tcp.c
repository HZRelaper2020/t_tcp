#include "t_common.h"

uint32_t tcp_ticks;
struct t_tcp_pcb *tcp_active_pcbs;

int t_tcp_init()
{
	int ret = 0;
	
	tcp_ticks = 0;
	tcp_active_pcbs = NULL;
	return ret;
}


static uint16_t t_tcp_new_port()
{
	static int times = 0;
	times += 1;
	return 4096 + times;
}

u8_t
t_tcp_seg_free(struct t_tcp_seg *seg)
{
  u8_t count = 0;

  if (seg != NULL) {
    if (seg->p != NULL) {
      count = t_pbuf_free(seg->p);
#if TCP_DEBUG
      seg->p = NULL;
#endif /* TCP_DEBUG */
    }
    t_memp_free(T_MEMP_TCP_SEG, seg);
  }
  return count;
}

u8_t
t_tcp_segs_free(struct t_tcp_seg *seg)
{
  u8_t count = 0;
  struct t_tcp_seg *next;
  while (seg != NULL) {
    next = seg->next;
    count += t_tcp_seg_free(seg);
    seg = next;
  }
  return count;
}

static u32_t
t_tcp_next_iss(void)
{
  static u32_t iss = 6510;

  iss += tcp_ticks;       /* XXX */
  return iss;
}

struct t_tcp_pcb *
t_tcp_alloc(u8_t prio)
{
  struct t_tcp_pcb *pcb;
  u32_t iss;
  
  pcb = t_memp_malloc(T_MEMP_TCP_PCB);
  if (pcb == NULL) {
	  ERROR(("t_tcp_alloc no memory for pcb"));
  }
  if (pcb != NULL) {
    memset(pcb, 0, sizeof(struct t_tcp_pcb));
    pcb->prio = T_TCP_PRIO_NORMAL;
    pcb->snd_buf = T_TCP_SND_BUF;
    pcb->snd_queuelen = 0;
    pcb->rcv_wnd = T_TCP_WND;
    pcb->tos = 0;
    pcb->ttl = T_TCP_TTL;
    pcb->mss = T_TCP_MSS;
    pcb->rto = 3000 / T_TCP_SLOW_INTERVAL;
    pcb->sa = 0;
    pcb->sv = 3000 / T_TCP_SLOW_INTERVAL;
    pcb->rtime = 0;
    pcb->cwnd = 1;
    iss = t_tcp_next_iss();
    pcb->snd_wl2 = iss;
    pcb->snd_nxt = iss;
    pcb->snd_max = iss;
    pcb->lastack = iss;
    pcb->snd_lbb = iss;   
    pcb->tmr = tcp_ticks;

    pcb->polltmr = 0;

#if LWIP_CALLBACK_API
    pcb->recv = NULL;
#endif /* LWIP_CALLBACK_API */  
    
    /* Init KEEPALIVE timer */
    pcb->keepalive = T_TCP_KEEPDEFAULT;
    pcb->keep_cnt = 0;
  }
  return pcb;
}
  
  
  
struct t_tcp_pcb * t_tcp_new(void)
{
  return t_tcp_alloc(T_TCP_PRIO_NORMAL);
}

err_t
t_tcp_connect(struct t_tcp_pcb *pcb, struct t_ip_addr *ipaddr, u16_t port,
      err_t (* connected)(void *arg, struct t_tcp_pcb *tpcb, err_t err))
{
  u32_t optdata;
  err_t ret = 0;
  u32_t iss;

  pcb->remote_ip = *ipaddr;
  pcb->remote_port = port;
  if (pcb->local_port == 0) {
    pcb->local_port = t_tcp_new_port();
  }
  iss = t_tcp_next_iss();
  pcb->rcv_nxt = 0;
  pcb->snd_nxt = iss;
  pcb->lastack = iss - 1;
  pcb->snd_lbb = iss - 1;
  pcb->rcv_wnd = T_TCP_WND;
  pcb->snd_wnd = T_TCP_WND;
  pcb->mss = T_TCP_MSS;
  pcb->cwnd = 1;
  pcb->ssthresh = pcb->mss * 10;
  pcb->state = SYN_SENT;
  #if LWIP_CALLBACK_API
  pcb->connected = connected;
  #endif /* LWIP_CALLBACK_API */

  T_TCP_REG(&tcp_active_pcbs, pcb);

  optdata = htonl(((u32_t)2 << 24) |
      ((u32_t)4 << 16) |
      (((u32_t)pcb->mss / 256) << 8) |
      (pcb->mss & 255));

  ret = t_tcp_enqueue(pcb, NULL, 0, TCP_SYN, 0, (u8_t *)&optdata, 4);

  if (ret == ERR_OK) {
    t_tcp_output(pcb);
  }
  return ret;
}
