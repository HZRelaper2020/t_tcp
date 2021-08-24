#ifndef SOURCE_SOCKETS__H
#define SOURCE_SOCKETS__H

#include "t_ip_addr.h"

struct sockaddr_in {
  u8_t sin_len;
  u8_t sin_family;
  u16_t sin_port;
  struct in_addr sin_addr;
  char sin_zero[8];
};

struct sockaddr {
  u8_t sa_len;
  u8_t sa_family;
  char sa_data[14];
};

#ifndef socklen_t
#  define socklen_t int
#endif

#define SOCK_RAW	3
#define SOCK_DGRAM	2
#define SCOK_STREAM	1

int t_lwip_socket(int domain,int type,int protocol);

#endif
