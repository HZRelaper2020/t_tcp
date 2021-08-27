#include "t_common.h"

/**
 * Enqueue either data or TCP options (but not both) for tranmission
 *
 *
 *
 * @arg pcb Protocol control block for the TCP connection to enqueue data for.
 * @arg arg Pointer to the data to be enqueued for sending.
 * @arg len Data length in bytes
 * @arg flags
 * @arg copy 1 if data must be copied, 0 if data is non-volatile and can be
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
}
