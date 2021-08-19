#ifndef SOURCE_SOCKETS__H
#define SOURCE_SOCKETS__H

#define SOCK_RAW	3
#define SOCK_DGRAM	2
#define SCOK_STREAM	1

int t_lwip_socket(int domain,int type,int protocol);

#endif
