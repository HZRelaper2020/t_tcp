#ifndef SOURCE_LWIPOPTS__H
#define SOURCE_LWIPOPTS__H


#define T_PBUF_POOL_SIZE			30	

#define T_PBUF_BUFSIZE				1514  // do not change this

#define T_MEMP_NUM_TCPIP_MSG            	80


#define T_TCP_MSS				536
#define T_TCP_SND_BUF				(12*T_TCP_MSS)
#define T_TCP_WND				(10*T_TCP_MSS)

#define T_TCP_SND_QUEUELEN			40
#define T_MEMP_NUM_TCP_SEG			T_TCP_SND_QUEUELEN

#endif

