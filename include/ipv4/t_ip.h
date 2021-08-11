#ifndef SOURCE_T_IP__H
#define SOURCE_T_IP__H


struct t_ether_hdr{
	uint8_t src[6];
	uint8_t dst[6];
#define T_ARP_FRAME_TYPE		0x0608U
#define T_IPV4_FRAME_TYPE 		0x0008U
	uint16_t frame_type;
};

struct t_ip_hdr{
	uint8_t r1[14];
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
