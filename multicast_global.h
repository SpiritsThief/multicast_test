#ifndef __MULTICAST_GLOBAL__
#define __MULTICAST_GLOBAL__

#include <stdlib.h>
#include <stdbool.h>

// ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff
// 8 * 4 + 7 + '\0' = 40
#define MC_ADDR_MAXLEN   45

#define GROUP_ADDR4    "224.0.55.55"
#define GROUP_ADDR6    "ff12::1234"
#define GROUP_PORT       4321

#define MULTICAST_MSG_BUFF_LEN  128
#define MULTICAST_MESSAGE            "Hello World"

#define WARNING_LOG_PRINT(format, ...)    \
    do{     \
        printf("[WARN] %s: "format, __func__,##__VA_ARGS__);   \
    }while(0)

#define ERROR_LOG_PRINT(format, ...)    \
    do{     \
        printf("[ERR] %s: "format, __func__,##__VA_ARGS__);   \
    }while(0)

#define DEBUG_LOG_PRINT(format, ...)    \
    do{     \
        printf("[DBG] %s: "format, __func__,##__VA_ARGS__);   \
    }while(0)

typedef struct option_args {
    //int ip_family;
    bool flag_ip4;
    bool flag_ip6;
    bool flag_addr;
    bool flag_dispatcher;
    bool flag_listener;
    char str_mc_addr[MC_ADDR_MAXLEN];         /*  string of multicast ip address. etc:"ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"  */
    unsigned long port_num;
}opt_args_t;


// extern function
bool do_listener(opt_args_t*);
bool do_dispatcher(opt_args_t*);


#endif /*__MULTICAST_GLOBAL__*/