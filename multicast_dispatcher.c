#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>


#include "multicast_global.h"

bool do_dispatcher(opt_args_t* prog_opt)
{
    if (NULL == prog_opt) {
        ERROR_LOG_PRINT("prog_opt is NULL pointer !!");
        return false;
    }

    int fd = 0;
    int ret = 0;

    struct sockaddr* sockaddr = NULL;
    socklen_t socklen = 0;
    struct sockaddr_in address4;
    struct sockaddr_in6 address6;

    if (prog_opt->flag_ip4) {
        // ipv4
        fd = socket(AF_INET, SOCK_DGRAM, 0);

        // ADDRESS
        memset(&address4, 0, sizeof(address4));
        address4.sin_family = AF_INET;
        address4.sin_port = htons(prog_opt->port_num);
        inet_pton(AF_INET, prog_opt->str_mc_addr, &address4.sin_addr);

        sockaddr = (struct sockaddr*)&address4;
        socklen = sizeof address4;
    } else {
        // ipv6
        fd = socket(AF_INET6, SOCK_DGRAM, 0);

        // ADDRESS
        memset(&address6, 0, sizeof(address6));
        address6.sin6_family = AF_INET6;
        address6.sin6_port = htons(prog_opt->port_num);
#ifdef __QNXNTO__
        address6.sin6_len = sizeof(struct sockaddr_in6);
#endif
        inet_pton(AF_INET6, prog_opt->str_mc_addr, &address6.sin6_addr);

        sockaddr = (struct sockaddr*)&address6;
        socklen = sizeof address6;
    }

    // SEND TO
    char buffer[MULTICAST_MSG_BUFF_LEN];
    strncpy(buffer, MULTICAST_MESSAGE, strlen(MULTICAST_MESSAGE)+1);
    ret = sendto(fd, buffer, strlen(MULTICAST_MESSAGE)+1, 0, sockaddr, socklen);

    if (ret < 0)
        ERROR_LOG_PRINT("sendto failed: errno=%d : %s\n", errno, strerror(errno));

    return true;
}
