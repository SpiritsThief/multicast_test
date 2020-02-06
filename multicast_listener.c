#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "multicast_global.h"

bool do_listener(opt_args_t* prog_opt)
{
    if (NULL == prog_opt) {
        ERROR_LOG_PRINT("prog_opt is NULL pointer !!");
        return false;
    }

    int fd = 0;
    int ret = 0;

    if (prog_opt->flag_ip4) {
        // ipv4
        fd = socket(AF_INET, SOCK_DGRAM, 0);

        //BIND
        struct sockaddr_in address;
        memset(&address, 0, sizeof (address));
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_family = AF_INET;
        address.sin_port = htons(prog_opt->port_num);

        bind(fd, (struct sockaddr*)&address, sizeof(struct sockaddr_in));

        // JOIN MEMBERSHIP
        struct ip_mreq group;
        memset((char *) &group, 0, sizeof group);
        inet_pton(AF_INET, prog_opt->str_mc_addr, &group.imr_multiaddr);

        group.imr_interface.s_addr = 0;
#ifdef __QNXNTO__
        ret = setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, sizeof group);
#else
        ret = setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, sizeof group);
#endif

    } else {
        // ipv6
        fd = socket(AF_INET6, SOCK_DGRAM, 0);

        //BIND
        struct sockaddr_in6 address;
        memset(&address, 0, sizeof (address));
        address.sin6_addr = in6addr_any;
        address.sin6_family = AF_INET6;
        address.sin6_port = htons(prog_opt->port_num);

        bind(fd, (struct sockaddr*)&address, sizeof address);

        // JOIN MEMBERSHIP
        struct ipv6_mreq group;
        memset((char *) &group, 0, sizeof group);
        inet_pton(AF_INET6, prog_opt->str_mc_addr, &group.ipv6mr_multiaddr);

        group.ipv6mr_interface= 0;
#ifdef __QNXNTO__
        ret = setsockopt(fd, IPPROTO_IPV6, IPV6_JOIN_GROUP, &group, sizeof group);
#else
        ret = setsockopt(fd, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &group, sizeof group);
#endif
    }

    if (ret != 0) {
        ERROR_LOG_PRINT("setsockopt error(%d): %s\n", errno, strerror(errno));
        return false;
    }

    // READ Buffer
    char buffer[MULTICAST_MSG_BUFF_LEN];
    int read_len = 0;
    while(1) {
        printf("Wait for the Message:\n");
        read_len = read(fd, buffer, sizeof buffer);
        if (read_len < 0)
        {
            ERROR_LOG_PRINT("read buffer error(%d): %s !!! \n", errno, strerror(errno));
        } else if (0 == read_len) {
            WARNING_LOG_PRINT("read buffer length is 0 !!!\n");
        } else {
            printf("read length:%d \n", read_len);
            printf("%s \n", buffer);
            printf("\n");
        }
    }

    return true;
}
