#include "t_common.h"


err_t
t_tcp_write(struct t_tcp_pcb *pcb, const void *arg, u16_t len, u8_t copy)
{
  /* connection is in valid state for data transmission? */
  if (pcb->state == ESTABLISHED ||
     pcb->state == CLOSE_WAIT ||
     pcb->state == SYN_SENT ||
     pcb->state == SYN_RCVD) {
    if (len > 0) {
      return t_tcp_enqueue(pcb, (void *)arg, len, 0, copy, NULL, 0);
    }
    return ERR_OK;
  } else {
    return ERR_CONN;
  }
}

/**
 * Enqueue either data or TCP options (but not both) for tranmission
 *
 *
 *
 * @arg pcb Protocol control block for the TCP connection to enqueue data for.
 * @arg arg Pointer to the data to be enqueued for sending.
 * @arg len Data length in bytes
 * @arg flags
 * @arg copy 1 if data must be copied from arg to t_pbuf, 0 if data is non-volatile and can be
 * referenced.
 * @arg optdata
 * @arg optlen
 */
err_t
t_tcp_enqueue(struct t_tcp_pcb *pcb, void *arg, u16_t len,
  u8_t flags, u8_t copy,
  u8_t *optdata, u8_t optlen)
{
  struct t_pbuf *p;
  struct t_tcp_seg *seg, *useg, *queue;
  u32_t left, seqno;
  u16_t seglen;
  void *ptr;
  u8_t queuelen;
  if (len > pcb->snd_buf){
	ERROR(("tcp_enqueue len is too big %d",len));
	return -1;
  }

  left = len;
  ptr = arg;

  seqno = pcb->snd_lbb;
  queuelen = pcb->snd_queuelen;

  if (queuelen >= T_TCP_SND_QUEUELEN) {
    ERROR(("tcp_enqueue: too long queue %u (max %u)\n", queuelen, T_TCP_SND_QUEUELEN));
    return -1;
  }

  #if 0
  if (queuelen != 0) {
    LWIP_ASSERT("tcp_enqueue: pbufs on queue => at least one queue non-empty",
      pcb->unacked != NULL || pcb->unsent != NULL);
  } else {
    LWIP_ASSERT("tcp_enqueue: no pbufs on queue => both queues empty",
      pcb->unacked == NULL && pcb->unsent == NULL);
  }
  #endif
  useg = queue = seg = NULL;
  seglen = 0;
  while (queue == NULL || left > 0) {
	seglen = left > pcb->mss? pcb->mss: left;

    /* Allocate memory for tcp_seg, and fill in fields. */
    seg = t_memp_malloc(T_MEMP_TCP_SEG);
    if (seg == NULL) {
      ERROR(("tcp_enqueue: could not allocate memory for tcp_seg"));
      goto memerr;
    }
    seg->next = NULL;
    seg->p = NULL;

    /* first segment of to-be-queued data? */
    if (queue == NULL) {
      queue = seg;
    }
    /* subsequent segments of to-be-queued data */
    else {
      /* Attach the segment to the end of the queued segments */
      useg->next = seg;
    }
    useg = seg;

	/* options? */
    if (optdata != NULL) {
      if ((seg->p = t_pbuf_alloc(T_PBUF_TRANSPORT, optlen, T_PBUF_FLAG_POOL)) == NULL) {
        goto memerr;
      }
      ++queuelen;
      seg->dataptr = seg->p->payload;
    }
    else if (copy) {
      if ((seg->p = t_pbuf_alloc(T_PBUF_TRANSPORT, seglen, T_PBUF_FLAG_POOL)) == NULL) {
        ERROR(("tcp_enqueue : could not allocate memory for pbuf copy size %u\n", seglen));
        goto memerr;
      }
      ++queuelen;
      if (arg != NULL) {
        memcpy(seg->p->payload, ptr, seglen);
      }
      seg->dataptr = seg->p->payload;
    }
    /* do not copy data */
    else {
      /* First, allocate a pbuf for holding the data.
       * since the referenced data is available at least until it is sent out on the
       * link (as it has to be ACKed by the remote party) we can safely use PBUF_ROM
       * instead of PBUF_REF here.
       */
      if ((p = t_pbuf_alloc(T_PBUF_TRANSPORT, seglen, T_PBUF_FLAG_POOL)) == NULL) {
        ERROR(("tcp_enqueue: could not allocate memory for zero-copy pbuf\n"));
        goto memerr;
      }
      ++queuelen;
      /* reference the non-volatile payload data */
      p->payload = ptr;
      seg->dataptr = ptr;

      /* Second, allocate a pbuf for the headers. */
      if ((seg->p = t_pbuf_alloc(T_PBUF_TRANSPORT, 0, T_PBUF_FLAG_POOL)) == NULL) {
        /* If allocation fails, we have to deallocate the data pbuf as
         * well. */
        t_pbuf_free(p);
        ERROR(("tcp_enqueue: could not allocate memory for header pbuf\n"));
        goto memerr;
      }
      ++queuelen;

      /* Concatenate the headers and data pbufs together. */
      t_pbuf_cat(seg->p/*header*/, p/*data*/);
      p = NULL;
    }

	if (queuelen > T_TCP_SND_QUEUELEN) {
      ERROR(("tcp_enqueue: queue too long %u (%u)\n", queuelen, T_TCP_SND_QUEUELEN));
      goto memerr;
    }

	seg->len = seglen;

    /* build TCP header */
    if (t_pbuf_header(seg->p, -T_TCP_HLEN)) {
      ERROR(("tcp_enqueue: no room for TCP header in pbuf.\n"));
      goto memerr;
    }
    seg->tcphdr = seg->p->payload;
    seg->tcphdr->src = htons(pcb->local_port);
    seg->tcphdr->dest = htons(pcb->remote_port);
    seg->tcphdr->seqno = htonl(seqno);
    seg->tcphdr->urgp = 0;
    T_TCPH_FLAGS_SET(seg->tcphdr, flags);
    /* don't fill in tcphdr->ackno and tcphdr->wnd until later */

    /* Copy the options into the header, if they are present. */
    if (optdata == NULL) {
      T_TCPH_HDRLEN_SET(seg->tcphdr, 5);
    }
    else {
      T_TCPH_HDRLEN_SET(seg->tcphdr, (5 + optlen / 4));
      /* Copy options into data portion of segment.
       Options can thus only be sent in non data carrying
       segments such as SYN|ACK. */
      memcpy(seg->dataptr, optdata, optlen);
    }
    left -= seglen;
    seqno += seglen;
    ptr = (void *)((char *)ptr + seglen);
  } /* end while */

  if (pcb->unsent == NULL) {
    useg = NULL;
  }
  else {
    for (useg = pcb->unsent; useg->next != NULL; useg = useg->next);
  }

  if (useg != NULL &&
    T_TCP_TCPLEN(useg) != 0 &&
    !(T_TCPH_FLAGS(useg->tcphdr) & (TCP_SYN | TCP_FIN)) &&
    !(flags & (TCP_SYN | TCP_FIN)) &&
    /* fit within max seg size */
    useg->len + queue->len <= pcb->mss) {
    /* Remove TCP header from first segment of our to-be-queued list */
    t_pbuf_header(queue->p, T_TCP_HLEN);
    t_pbuf_cat(useg->p, queue->p);
    useg->len += queue->len;
    useg->next = queue->next;

    LWIP_DEBUGF(TCP_OUTPUT_DEBUG | DBG_TRACE | DBG_STATE, ("tcp_enqueue: chaining segments, new len %u\n", useg->len));
    if (seg == queue) {
      seg = NULL;
    }
    t_memp_free(T_MEMP_TCP_SEG, queue);
  }
  else {
    /* empty list */
    if (useg == NULL) {
      /* initialize list with this segment */
      pcb->unsent = queue;
    }
    /* enqueue segment */
    else {
      useg->next = queue;
    }
  }

  if ((flags & TCP_SYN) || (flags & TCP_FIN)) {
    ++len;
  }

  pcb->snd_lbb += len;
  pcb->snd_buf -= len;
  /* update number of segments on the queues */
  pcb->snd_queuelen = queuelen;

  if (seg != NULL && seglen > 0 && seg->tcphdr != NULL) {
    T_TCPH_SET_FLAG(seg->tcphdr, TCP_PSH);
  }

  return 0;

memerr:
  if (queue != NULL) {
    t_tcp_segs_free(queue);
  }
  return -1;

}

/**
 * Actually send a TCP segment over IP
 */
static void
t_tcp_output_segment(struct t_tcp_seg *seg, struct t_tcp_pcb *pcb)
{
  u16_t len;
  struct t_netif *netif;

  /* The TCP header has already been constructed, but the ackno and
   wnd fields remain. */
  seg->tcphdr->ackno = htonl(pcb->rcv_nxt);

  /* silly window avoidance */
  if (pcb->rcv_wnd < pcb->mss) {
    seg->tcphdr->wnd = 0;
  } else {
    /* advertise our receive window size in this TCP segment */
    seg->tcphdr->wnd = htons(pcb->rcv_wnd);
  }

  /* If we don't have a local IP address, we get one by
     calling ip_route(). */
  if (ip_addr_isany(&(pcb->local_ip))) {
    netif = t_ip_route(&(pcb->remote_ip));
    if (netif == NULL) {
      return;
    }
    t_ip_addr_set(&(pcb->local_ip), &(netif->ip_addr));
  }

  pcb->rtime = 0;

  if (pcb->rttest == 0) {
    pcb->rttest = tcp_ticks;
    pcb->rtseq = ntohl(seg->tcphdr->seqno);

  }

  len = (u16_t)((u8_t *)seg->tcphdr - (u8_t *)seg->p->payload);

  seg->p->len -= len;
  seg->p->tot_len -= len;

  seg->p->payload = seg->tcphdr;

  seg->tcphdr->chksum = 0;

  seg->tcphdr->chksum = t_inet_chksum_pseudo(seg->p,
             &(pcb->local_ip),
             &(pcb->remote_ip),
             T_IP_PROTO_TCP, seg->p->tot_len);

  t_ip_output(netif,seg->p, &(pcb->local_ip), &(pcb->remote_ip), pcb->ttl, pcb->tos,
      T_IP_PROTO_TCP);
}

err_t
t_tcp_output(struct t_tcp_pcb *pcb)
{
  struct t_pbuf *p;
  struct t_tcp_hdr *tcphdr;
  struct t_tcp_seg *seg, *useg;
  struct t_netif *netif;
  u32_t wnd;

#if 0
  if (tcp_input_pcb == pcb) {
    return ERR_OK;
  }
#endif

  wnd = MIN(pcb->snd_wnd, pcb->cwnd);

  seg = pcb->unsent;

  /* useg should point to last segment on unacked queue */
  useg = pcb->unacked;
  if (useg != NULL) {
    for (; useg->next != NULL; useg = useg->next);
  }

  if (pcb->flags & TF_ACK_NOW &&
     (seg == NULL ||
      ntohl(seg->tcphdr->seqno) - pcb->lastack + seg->len > wnd)) {

    netif = t_ip_route(&(pcb->local_ip));
    p = t_pbuf_alloc(T_PBUF_IP, T_TCP_HLEN, T_PBUF_FLAG_POOL);
    if (p == NULL) {
      ERROR(("tcp_output: (ACK) could not allocate pbuf\n"));
      return ERR_BUF;
    }
    /* remove ACK flags from the PCB, as we send an empty ACK now */
    pcb->flags &= ~(TF_ACK_DELAY | TF_ACK_NOW);

    tcphdr = p->payload;
    tcphdr->src = htons(pcb->local_port);
    tcphdr->dest = htons(pcb->remote_port);
    tcphdr->seqno = htonl(pcb->snd_nxt);
    tcphdr->ackno = htonl(pcb->rcv_nxt);
    T_TCPH_FLAGS_SET(tcphdr, TCP_ACK);
    tcphdr->wnd = htons(pcb->rcv_wnd);
    tcphdr->urgp = 0;
    T_TCPH_HDRLEN_SET(tcphdr, 5);

    tcphdr->chksum = 0;

    tcphdr->chksum = t_inet_chksum_pseudo(p, &(pcb->local_ip), &(pcb->remote_ip),
          T_IP_PROTO_TCP, p->tot_len);

    t_ip_output(netif,p, &(pcb->local_ip), &(pcb->remote_ip), pcb->ttl, pcb->tos, \
        T_IP_PROTO_TCP);
    t_pbuf_free(p);

    return ERR_OK;
  }
 /* data available and window allows it to be sent? */
  while (seg != NULL &&
  ntohl(seg->tcphdr->seqno) - pcb->lastack + seg->len <= wnd) {

    pcb->unsent = seg->next;

    if (pcb->state != SYN_SENT) {
      T_TCPH_SET_FLAG(seg->tcphdr, TCP_ACK);
      pcb->flags &= ~(TF_ACK_DELAY | TF_ACK_NOW);
    }

    	t_tcp_output_segment(seg, pcb);
	pcb->snd_nxt = ntohl(seg->tcphdr->seqno) + T_TCP_TCPLEN(seg);
    if (T_TCP_SEQ_LT(pcb->snd_max, pcb->snd_nxt)) {
      pcb->snd_max = pcb->snd_nxt;
    }

	/* put segment on unacknowledged list if length > 0 */
    if (T_TCP_TCPLEN(seg) > 0) {
      seg->next = NULL;
      /* unacked list is empty? */
      if (pcb->unacked == NULL) {
        pcb->unacked = seg;
        useg = seg;
      /* unacked list is not empty? */
      } else {
        /* In the case of fast retransmit, the packet should not go to the tail
         * of the unacked queue, but rather at the head. We need to check for
         * this case. -STJ Jul 27, 2004 */
        if (T_TCP_SEQ_LT(ntohl(seg->tcphdr->seqno), ntohl(useg->tcphdr->seqno))){
          /* add segment to head of unacked list */
          seg->next = pcb->unacked;
          pcb->unacked = seg;
        } else {
          /* add segment to tail of unacked list */
          useg->next = seg;
          useg = useg->next;
        }
      }
    /* do not queue empty segments on the unacked list */
    } else {
      t_tcp_seg_free(seg);
    }
    seg = pcb->unsent;
  }
  return ERR_OK;
}


void
tcp_keepalive(struct t_tcp_pcb *pcb)
{
   struct t_pbuf *p;
   struct t_tcp_hdr *tcphdr;


   
   p = t_pbuf_alloc(T_PBUF_IP, T_TCP_HLEN, T_PBUF_FLAG_POOL);

   if(p == NULL) {
      ERROR(("tcp_keepalive: could not allocate memory for pbuf\n"));
      return;
   }

   tcphdr = p->payload;
   tcphdr->src = htons(pcb->local_port);
   tcphdr->dest = htons(pcb->remote_port);
   tcphdr->seqno = htonl(pcb->snd_nxt - 1);
   tcphdr->ackno = htonl(pcb->rcv_nxt);
   tcphdr->wnd = htons(pcb->rcv_wnd);
   tcphdr->urgp = 0;
   T_TCPH_HDRLEN_SET(tcphdr, 5);
   
   tcphdr->chksum = 0;
   tcphdr->chksum = t_inet_chksum_pseudo(p, &pcb->local_ip, &pcb->remote_ip, T_IP_PROTO_TCP, p->tot_len);

   /* Send output to IP */
  t_ip_output(NULL,p, &pcb->local_ip, &pcb->remote_ip, pcb->ttl, 0, T_IP_PROTO_TCP);

  t_pbuf_free(p);

}

