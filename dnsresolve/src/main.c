//=================================================================================================
// Includes:                  List of required include files.
//=================================================================================================
#include <zephyr.h>
#include <sys/printk.h>
#include <logging/log.h>
#include <net/net_ip.h>
#include <stdio.h>
#include "../include/net/dns.h"

//=================================================================================================
// Defines/Macros:            Definition of local macros.
//=================================================================================================
LOG_MODULE_REGISTER(dns_resolve_main, LOG_LEVEL_DBG);

//=================================================================================================
// Variables:                 Definition of local variables & constants.
//=================================================================================================
struct k_delayed_work dns_resolve_work;


static int retrieveUrl(const char *url, char *address, size_t addressLen) {
    struct sockaddr_in sa;

    int err = dns_resolve_one(url, &sa);
    LOG_INF("DNS resolve result: %d", err);

    if (err != 0) {
        LOG_WRN("Failed to retrieve IP address of %s. Error: %d", url, err);
        return err;
    }

    uint8_t *ip = (sa).sin_addr.s4_addr;

    LOG_INF("IP of %s is %d.%d.%d.%d", url, ip[0], ip[1], ip[2], ip[3]);

    address[addressLen - 1] = 0;
    snprintf(address, addressLen, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    LOG_INF("IP of %s is %s", url, address);

    if (zsock_inet_ntop(AF_INET, &sa.sin_addr, address, addressLen) != NULL) {
        LOG_INF("IP address of %s is %s", url, address);
    }

    return 0;
}

static void run_request(struct k_work *work) {
    ARG_UNUSED(work);
    const char* url = "www.zephyrproject.org";
    char addr[NET_IPV6_ADDR_LEN];
    int ret = retrieveUrl(url, addr, NET_IPV6_ADDR_LEN);
    if(ret == 0) {
        LOG_INF("IP of %s is %s", url, addr);
    }
    else {
        LOG_WRN("dns resolve failed with code %d", ret);
    }
}

//*************************************************************************************************
// main:        main-thread / main-function

void main(void) {

    dns_init();
    k_delayed_work_init(&dns_resolve_work, run_request);
    k_delayed_work_submit(&dns_resolve_work, K_SECONDS(1));
}