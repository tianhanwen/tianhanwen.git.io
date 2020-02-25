#ifndef __MAKEU_NET_H__
#define __MAKEU_NET_H__

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>			/* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>			/* superset of previous */

#include <sys/select.h>
#include <sys/time.h>
//#include <sys/types.h>
#include <unistd.h>



#define SERV_PORT 5002
#define SERV_IP_ADDR "192.168.7.246"
#define BACKLOG 5

#define QUIT_STR "quit"
#define SERV_RESP_STR "SERVER:"
#endif
