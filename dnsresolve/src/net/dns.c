/**
 * 	This module implements basic DNS resolution.
 * 
 * 	BUG:	It has occured that calls to freeaddrinfo() may result in a crash of the application.
 * 			The current version of the software however, does not seem to be affected. For a quick
 * 			workaround the lines in question may be commented out. It is noteworthy though pointing
 * 			out that this will leak memory.
*/

#include "../../include/net/dns.h"
#include "../../include/utils.h"

#include <zephyr.h>
#include <net/socket.h>
//#include <nrf_socket.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(net_dns, CONFIG_LOG_DEFAULT_LEVEL);

#define REQUIRE_DNS_STATE(r)    \
    REQUIRE_MODULE_STATE(dns.state, STATES[dns.state], (r), STATES[(r)])

/*	Module globals	*/

struct {
	enum dns_state		state;
	struct  zsock_addrinfo		*entries;
	struct  zsock_addrinfo		*current;
} dns = {
	.state		= DNS_STATE_UNINITIALIZED,
	.entries	= NULL,
	.current	= NULL
};

static const char *STATES[] = {
	[DNS_STATE_UNINITIALIZED]	= "DNS_STATE_UNINITIALIZED",
	[DNS_STATE_READY]			= "DNS_STATE_READY",
	[DNS_STATE_RESULT_READY]	= "DNS_STATE_RESULT_READY"
};


/*	Function prototypes	*/

static inline void dns_set_state(enum dns_state state);

/*	Public functions	*/

enum dns_state	dns_get_module_state() {
	return dns.state;
}

int dns_init() {
	REQUIRE_DNS_STATE(DNS_STATE_UNINITIALIZED);
	// TODO Is this call required? Should already be configured using proj.conf.
	/*
	struct nrf_in_addr dns;
	dns.s_addr = DNS_SERVER_IP4;
	int err = nrf_setdnsaddr(2, &dns);

	if (err != 0) {
		return err;
	}
	*/
	dns_set_state(DNS_STATE_READY);
	return 0;
}

void dns_uninit() {
	if (dns_get_module_state() == DNS_STATE_RESULT_READY)
        zsock_freeaddrinfo(dns.entries);
	dns_set_state(DNS_STATE_UNINITIALIZED);
}

int dns_resolve_one(const char *host, struct sockaddr_in *addr) {
	REQUIRE_DNS_STATE(DNS_STATE_READY);
	REQUIRE(addr != NULL, -EINVAL);
	int err = dns_resolve_all(host);

	if (err != 0)
		return err;
	
	// This will succeed at least once.
	dns_get_next_entry(addr);
	if (dns_get_module_state() == DNS_STATE_RESULT_READY)
        zsock_freeaddrinfo(dns.entries);
		
	dns_set_state(DNS_STATE_READY);
	return 0;
}

int dns_resolve_all(const char *host) {
	REQUIRE_DNS_STATE(DNS_STATE_READY);
	struct  zsock_addrinfo hints = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM
	};
	
	int err = zsock_getaddrinfo(host, NULL, &hints, &dns.entries);
	if (err != 0) {
		return err;
	}

	dns.current = dns.entries;
	dns_set_state(dns.current != NULL ? DNS_STATE_RESULT_READY : DNS_STATE_READY);
	return 0;
}

int dns_get_next_entry(struct sockaddr_in *addr) {
	REQUIRE_DNS_STATE(DNS_STATE_RESULT_READY);
	REQUIRE(addr != NULL, -EINVAL);

	if (dns.current->ai_addrlen != sizeof(struct sockaddr_in)) {
		return -ENODATA;
	}

	addr->sin_addr.s_addr = ((struct sockaddr_in *)dns.current->ai_addr)->sin_addr.s_addr;
	addr->sin_family = AF_INET;
	dns.current = dns.current->ai_next;
	
	if (dns.current == NULL) {
        zsock_freeaddrinfo(dns.entries);
		dns_set_state(DNS_STATE_READY);
	}

	return 0;
}

int dns_skip_all() {
	REQUIRE_DNS_STATE(DNS_STATE_RESULT_READY);
    zsock_freeaddrinfo(dns.entries);
	dns_set_state(DNS_STATE_READY);
	return 0;
}

/*	Private functions	*/

static inline void dns_set_state(enum dns_state state) {
	dns.state = state;
}
