#ifndef SOURCE_T_IP__H
#define SOURCE_T_IP__H


#define T_ARP_FRAME_TYPE		0x0806U
#define T_IPV4_FRAME_TYPE 		0x0800U

#define T_IPH_V(a)		(htons((a)->_v_hl_tos)>>12)
#define T_IPH_HL(a)		(htons((a)->_v_hl_tos)>>8&0x0f)

#define T_IPH_PROTO(a)		(htons((a)->_ttl_proto)&0xff)

#define T_IP_PROTO_ICMP		1

#define T_ICMP_ECHO_REQUEST		0x8

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
	uint8_t r2[6];
	struct t_ip_addr dst;
};

struct t_etherip_hdr{
	struct t_ether_hdr eth_header;
	struct t_ip_hdr ip;
};

t_err_t t_ip_input(struct t_pbuf *p,struct t_netif* inp);

t_err_t t_raw_input(struct t_pbuf *p,struct t_netif* inp);


#endif
