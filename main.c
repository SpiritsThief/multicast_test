#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "multicast_global.h"

static opt_args_t prog_opt;
static const char* prog_name = "multicast_test";

const struct option longopts[] = {
    {"inet", no_argument, NULL, '4'},       // default
    {"inet6", no_argument, NULL, '6'},
    {"dispatcher", no_argument, NULL, 'd'},
    {"listener", no_argument, NULL, 'l'},
    {"addr", required_argument, NULL, 'a'},
    {"port", required_argument, NULL, 'p'},

    {NULL, 0, NULL, 0}
};

void mc_usage() 
{
    printf("usage : %s [-dl] [-46] [-a <address>] [-p <port>] \n", prog_name);
    printf(" \n");
    printf("\t -d,\t\t\tdispatcher \n");
    printf("\t -l,\t\t\tlistener \n");
    printf("\t -4,\t\t\tIPV4 \n");
    printf("\t -6,\t\t\tIPV6 \n");
    printf("\t -a <address>,\tmulticast ip address \n");
    printf("\t -p <port>,\tmulticast ip address \n");
    printf(" \n");
    return;
}

bool is_valid_multicast_addr(char* addr, bool flag_v4)
{
    int len = 0;
    if (NULL == addr || (len = strlen(addr)) <= 0) {
        ERROR_LOG_PRINT("addr is NULL pointer or empty");
        return false;
    }

    if (flag_v4) {
        // ipv4 case:
        struct in_addr addr4;
        memset(&addr4, 0, sizeof(addr4));
        if (inet_pton(AF_INET, addr, &addr4) <= 0) {
            ERROR_LOG_PRINT("invalid ipv4 address");
            return false;
        }

        // TODO: Need to verify the multicast address type

    } else {
        // ipv6 case
        struct in6_addr addr6;
        memset(&addr6, 0, sizeof(addr6));
        if (inet_pton(AF_INET6, addr, &addr6) <= 0) {
            ERROR_LOG_PRINT("invalid ipv6 address");
            return false;
        }

        // TODO: Need to verify the multicast address type
    }

    return true;
}


bool process_args(int argc, char* argv[], opt_args_t* prog_opt)
{
    if (NULL == prog_opt || argc < 2) {
        ERROR_LOG_PRINT("Invalid Arguments\n");
        return false;
    }

    char ch = 0;
    int longindex = 0;
    memset(prog_opt->str_mc_addr, 0, sizeof(MC_ADDR_MAXLEN));

    while(-1 != (ch = getopt_long(argc, argv, "46a:dlp:", longopts, &longindex))) {

        switch(ch) {
        case '4':
            prog_opt->flag_ip4 = true;
            break;
        case '6':
            prog_opt->flag_ip6 = true;
            break;
        case 'd':
            prog_opt->flag_dispatcher = true;
            break;
        case 'l':
            prog_opt->flag_listener = true;
            break;
        case 'a':
            prog_opt->flag_addr = true;
            snprintf(prog_opt->str_mc_addr, MC_ADDR_MAXLEN - 1, "%s", optarg);
            break;
        case 'p':
            {
                char* leftover = NULL;
                prog_opt->port_num = strtoul(optarg, &leftover, 10);  // ULONG_MAX is 4294967295   ( maximum 10 number)
            }
            break;
        default:
            ERROR_LOG_PRINT("%s: Invalid Arguments\n", prog_name);
            return false;
            break;
        }

    }
    return true;
}

bool check_args(opt_args_t* prog_opt)
{
    if (NULL == prog_opt)
        return false;

    if (prog_opt->flag_ip4 && prog_opt->flag_ip6)
        return false;

    if (!prog_opt->flag_ip4 && !prog_opt->flag_ip6)
        prog_opt->flag_ip4 = true;

    if ((prog_opt->flag_dispatcher && prog_opt->flag_listener) ||
            (!prog_opt->flag_dispatcher && !prog_opt->flag_listener))
        return false;

    if (!prog_opt->flag_addr) 
    {
        // config default GROUP address
        if (prog_opt->flag_ip4)
            //strlcpy(prog_opt->str_mc_addr, GROUP_ADDR4, sizeof(prog_opt->str_mc_addr));
            strncpy(prog_opt->str_mc_addr, GROUP_ADDR4, strlen(GROUP_ADDR4)+1);
        else
             strncpy(prog_opt->str_mc_addr, GROUP_ADDR6, strlen(GROUP_ADDR6)+1);
            //strlcpy(prog_opt->str_mc_addr, GROUP_ADDR6, sizeof(prog_opt->str_mc_addr));
    } else if (!is_valid_multicast_addr(prog_opt->str_mc_addr, prog_opt->flag_ip4)) {
        return false;
    }

    return true;
}

int main(int argc, char** argv)
{
    // initialization
    memset(&prog_opt, 0, sizeof(prog_opt));
    prog_opt.flag_ip4 = false;
    prog_opt.flag_ip6 = false;
    prog_opt.flag_addr = false;
    prog_opt.port_num = GROUP_PORT;

    // args handling
    if (!process_args(argc, argv, &prog_opt)) 
        goto fail;

    if (!check_args(&prog_opt))
        goto fail;

    DEBUG_LOG_PRINT("address: %s\n", prog_opt.str_mc_addr);
    DEBUG_LOG_PRINT("port: %d\n", prog_opt.port_num);
    if (prog_opt.flag_listener) {
        do_listener(&prog_opt);
    } else if (prog_opt.flag_dispatcher) {
        do_dispatcher(&prog_opt);
    } else {
        goto fail;
    }
    return 0;

fail:
    mc_usage();
    return 0;
}
