#ifndef SOURCE_T_IP__H
#define SOURCE_T_IP__H


#define T_IP_HLEN			20

#define T_ARP_FRAME_TYPE		0x0806U
#define T_IPV4_FRAME_TYPE 		0x0800U

#define T_IP_DF				0x4000 /* do not fragment */
#define T_IPH_V(a)		(htons((a)->_v_hl_tos)>>12)
#define T_IPH_HL(a)		(htons((a)->_v_hl_tos)>>8&0x0f)
#define T_IPH_TTL(a)		(htons((a)->_ttl_proto)>>8)

#define T_IPH_PROTO(a)		(htons((a)->_ttl_proto)&0xff)


#define T_IPH_VHLTOS_SET(hdr, v, hl, tos) (hdr)->_v_hl_tos = (htons(((v) << 12) | ((hl) << 8) | (tos)))
#define T_IPH_LEN_SET(hdr, len) (hdr)->_len = (len)
#define T_IPH_ID_SET(hdr, id) (hdr)->_id = (id)
#define T_IPH_OFFSET_SET(hdr, off) (hdr)->_offset = (off)
#define T_IPH_TTL_SET(hdr, ttl) (hdr)->_ttl_proto = (htons(T_IPH_PROTO(hdr) | ((ttl) << 8)))
#define T_IPH_PROTO_SET(hdr, proto) (hdr)->_ttl_proto = (htons((proto) | (T_IPH_TTL(hdr) << 8)))
#define T_IPH_CHKSUM_SET(hdr, chksum) (hdr)->_chksum = (chksum)

#define T_IP_PROTO_ICMP		1
#define T_IP_PROTO_UDP		17
#define T_IP_PROTO_TCP		6

#define T_ICMP_ECHO_REQUEST		0x8

#define T_IP_PCB struct t_ip_addr local_ip; \
  struct t_ip_addr remote_ip; \
   /* Socket options */  \
  u16_t so_options;      \
   /* Type Of Service */ \
  u8_t tos;              \
  /* Time To Live */     \
  u8_t ttl

struct t_ether_hdr{
	uint8_t dst[6];
	uint8_t src[6];
	uint16_t frame_type;
};

struct t_ip_hdr{
	/* version /header length / type of server */
	uint16_t _v_hl_tos;
	/* total length */
	uint16_t _len;
	int16_t _id;
	uint16_t _offset;
	uint16_t _ttl_proto;
	uint16_t _chksum;
	struct t_ip_addr src;
	struct t_ip_addr dst;
};

struct t_etherip_hdr{
	struct t_ether_hdr eth_header;
	struct t_ip_hdr ip;
};

struct t_netif *
t_ip_route(struct t_ip_addr *dest);

t_err_t t_ip_input(struct t_pbuf *p,struct t_netif* inp);

t_err_t t_raw_input(struct t_pbuf *p,struct t_netif* inp);

int t_ip_output(struct t_netif* inp,struct t_pbuf* p,struct t_ip_addr *src,
                struct t_ip_addr * dst,
                uint8_t ttl,uint8_t tos, uint8_t proto);
#endif
