#ifndef SOURCE_T_TCP__H
#define SOURCE_T_TCP__H

#define T_TCP_HLEN		20


#define T_TCP_PRIO_MIN    1
#define T_TCP_PRIO_NORMAL 64
#define T_TCP_PRIO_MAX    127

#ifndef T_TCP_TTL
#define T_TCP_TTL	255
#endif

#define T_TCP_SEQ_LT(a,b)     ((s32_t)((a)-(b)) < 0)
#define T_TCP_SEQ_LEQ(a,b)    ((s32_t)((a)-(b)) <= 0)
#define T_TCP_SEQ_GT(a,b)     ((s32_t)((a)-(b)) > 0)
#define T_TCP_SEQ_GEQ(a,b)    ((s32_t)((a)-(b)) >= 0)

#ifndef T_TCP_TMR_INTERVAL
#define T_TCP_TMR_INTERVAL       250  /* The TCP timer interval in
                                       milliseconds. */
#endif /* TCP_TMR_INTERVAL */

#ifndef T_TCP_FAST_INTERVAL
#define T_TCP_FAST_INTERVAL      T_TCP_TMR_INTERVAL /* the fine grained timeout in milliseconds */
#endif /* TCP_FAST_INTERVAL */

#ifndef TCP_SLOW_INTERVAL
#define T_TCP_SLOW_INTERVAL      (2*T_TCP_TMR_INTERVAL)  /* the coarse grained timeout in milliseconds */
#endif /* TCP_SLOW_INTERVAL */

/* Keepalive values */
#define  T_TCP_KEEPDEFAULT   7200000                       /* KEEPALIVE timer in miliseconds */
#define  T_TCP_KEEPINTVL     75000                         /* Time between KEEPALIVE probes in miliseconds */
#define  T_TCP_KEEPCNT       9                             /* Counter for KEEPALIVE probes */
#define  T_TCP_MAXIDLE       TCP_KEEPCNT * TCP_KEEPINTVL   /* Maximum KEEPALIVE probe time */

#define TCP_FIN 0x01U
#define TCP_SYN 0x02U
#define TCP_RST 0x04U
#define TCP_PSH 0x08U
#define TCP_ACK 0x10U
#define TCP_URG 0x20U
#define TCP_ECE 0x40U
#define TCP_CWR 0x80U

#define TCP_FLAGS 0x3fU

#define T_TCPH_OFFSET(phdr) (ntohs((phdr)->_hdrlen_rsvd_flags) >> 8)
#define T_TCPH_HDRLEN(phdr) (ntohs((phdr)->_hdrlen_rsvd_flags) >> 12)
#define T_TCPH_FLAGS(phdr)  (ntohs((phdr)->_hdrlen_rsvd_flags) & TCP_FLAGS)

#define T_TCPH_OFFSET_SET(phdr, offset) (phdr)->_hdrlen_rsvd_flags = htons(((offset) << 8) | T_TCPH_FLAGS(phdr))
#define T_TCPH_HDRLEN_SET(phdr, len) (phdr)->_hdrlen_rsvd_flags = htons(((len) << 12) | T_TCPH_FLAGS(phdr))
#define T_TCPH_FLAGS_SET(phdr, flags) (phdr)->_hdrlen_rsvd_flags = htons((ntohs((phdr)->_hdrlen_rsvd_flags) & ~TCP_FLAGS) | (flags))
#define T_TCPH_SET_FLAG(phdr, flags ) (phdr)->_hdrlen_rsvd_flags = htons(ntohs((phdr)->_hdrlen_rsvd_flags) | (flags))
#define T_TCPH_UNSET_FLAG(phdr, flags) (phdr)->_hdrlen_rsvd_flags = htons(ntohs((phdr)->_hdrlen_rsvd_flags) | (T_TCPH_FLAGS(phdr) & ~(flags)) )

#define T_TCP_TCPLEN(seg) ((seg)->len + ((T_TCPH_FLAGS((seg)->tcphdr) & TCP_FIN || \
          T_TCPH_FLAGS((seg)->tcphdr) & TCP_SYN)? 1: 0))

#define T_TCP_REG(pcbs, npcb) do { \
                            npcb->next = *pcbs; \
                            *(pcbs) = npcb; \
                            } while(0)
/*               tcp_timer_needed(); \  */
enum tcp_state {
  CLOSED      = 0,
  LISTEN      = 1,
  SYN_SENT    = 2,
  SYN_RCVD    = 3,
  ESTABLISHED = 4,
  FIN_WAIT_1  = 5,
  FIN_WAIT_2  = 6,
  CLOSE_WAIT  = 7,
  CLOSING     = 8,
  LAST_ACK    = 9,
  TIME_WAIT   = 10
};

#pragma pack(1)
struct t_tcp_hdr {
  PACK_STRUCT_FIELD(u16_t src);
  PACK_STRUCT_FIELD(u16_t dest);
  PACK_STRUCT_FIELD(u32_t seqno);
  PACK_STRUCT_FIELD(u32_t ackno);
  PACK_STRUCT_FIELD(u16_t _hdrlen_rsvd_flags);
  PACK_STRUCT_FIELD(u16_t wnd);
  PACK_STRUCT_FIELD(u16_t chksum);
  PACK_STRUCT_FIELD(u16_t urgp);
} PACK_STRUCT_STRUCT;
#pragma pack()

struct t_tcp_seg {
  struct t_tcp_seg *next;    /* used when putting segements on a queue */
  struct t_pbuf *p;          /* buffer containing data + TCP header */
  void *dataptr;           /* pointer to the TCP data in the pbuf */
  u16_t len;               /* the TCP length of this segment */
  struct t_tcp_hdr *tcphdr;  /* the TCP header */
};

struct t_tcp_pcb {
/** common PCB members */
  T_IP_PCB;
/** protocol specific PCB members */
  struct t_tcp_pcb *next; /* for the linked list */
  enum tcp_state state; /* TCP state */
  u8_t prio;
  void *callback_arg;

  u16_t local_port;
  u16_t remote_port;

  u8_t flags;
#define TF_ACK_DELAY (u8_t)0x01U   /* Delayed ACK. */
#define TF_ACK_NOW   (u8_t)0x02U   /* Immediate ACK. */
#define TF_INFR      (u8_t)0x04U   /* In fast recovery. */
#define TF_RESET     (u8_t)0x08U   /* Connection was reset. */
#define TF_CLOSED    (u8_t)0x10U   /* Connection was sucessfully closed. */
#define TF_GOT_FIN   (u8_t)0x20U   /* Connection was closed by the remote end. */
#define TF_NODELAY   (u8_t)0x40U   /* Disable Nagle algorithm */

  /* receiver variables */
  u32_t rcv_nxt;   /* next seqno expected */
  u16_t rcv_wnd;   /* receiver window */

  /* Timers */
  u32_t tmr;
  u8_t polltmr, pollinterval;

  /* Retransmission timer. */
  u16_t rtime;

  u16_t mss;   /* maximum segment size */

  /* RTT (round trip time) estimation variables */
  u32_t rttest; /* RTT estimate in 500ms ticks */
  u32_t rtseq;  /* sequence number being timed */
  s16_t sa, sv; /* @todo document this */

  u16_t rto;    /* retransmission time-out */
  u8_t nrtx;    /* number of retransmissions */

  /* fast retransmit/recovery */
  u32_t lastack; /* Highest acknowledged seqno. */
  u8_t dupacks;

  /* congestion avoidance/control variables */
  u16_t cwnd;
  u16_t ssthresh;

  /* sender variables */
  u32_t snd_nxt,       /* next seqno to be sent */
    snd_max,       /* Highest seqno sent. */
    snd_wnd,       /* sender window */
    snd_wl1, snd_wl2, /* Sequence and acknowledgement numbers of last
       window update. */
    snd_lbb;       /* Sequence number of next byte to be buffered. */

  u16_t acked;

  u16_t snd_buf;   /* Available buffer space for sending (in bytes). */
  u8_t snd_queuelen; /* Available buffer space for sending (in tcp_segs). */


  /* These are ordered by sequence number: */
  struct t_tcp_seg *unsent;   /* Unsent (queued) segments. */
  struct t_tcp_seg *unacked;  /* Sent but unacknowledged segments. */
#if TCP_QUEUE_OOSEQ
  struct t_tcp_seg *ooseq;    /* Received out of sequence segments. */
#endif /* TCP_QUEUE_OOSEQ */

#if LWIP_CALLBACK_API
  /* Function to be called when more send buffer space is available. */
  err_t (* sent)(void *arg, struct t_tcp_pcb *pcb, u16_t space);

  /* Function to be called when (in-sequence) data has arrived. */
  err_t (* recv)(void *arg, struct t_tcp_pcb *pcb, struct t_pbuf *p, err_t err);

  /* Function to be called when a connection has been set up. */
  err_t (* connected)(void *arg, struct t_tcp_pcb *pcb, err_t err);

  /* Function to call when a listener has been connected. */
  err_t (* accept)(void *arg, struct t_tcp_pcb *newpcb, err_t err);

  /* Function which is called periodically. */
  err_t (* poll)(void *arg, struct t_tcp_pcb *pcb);

  /* Function to be called whenever a fatal error occurs. */
  void (* errf)(void *arg, err_t err);
#endif /* LWIP_CALLBACK_API */

  /* idle time before KEEPALIVE is sent */
  u32_t keepalive;

  /* KEEPALIVE counter */
  u8_t keep_cnt;
};

extern uint32_t tcp_ticks;

int t_tcp_init();

int t_tcp_input(struct t_netif* inp,struct t_pbuf* p);

struct t_tcp_pcb * t_tcp_new(void);

err_t
t_tcp_connect(struct t_tcp_pcb *pcb, struct t_ip_addr *ipaddr, u16_t port,
      err_t (* connected)(void *arg, struct t_tcp_pcb *tpcb, err_t err));

u8_t
t_tcp_seg_free(struct t_tcp_seg *seg);

u8_t
t_tcp_segs_free(struct t_tcp_seg *seg);

err_t
t_tcp_enqueue(struct t_tcp_pcb *pcb, void *arg, u16_t len,
  u8_t flags, u8_t copy,
  u8_t *optdata, u8_t optlen);

err_t
t_tcp_output(struct t_tcp_pcb *pcb);

#endif
