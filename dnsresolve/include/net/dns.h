/**
 *  This header defines an API for DNS lookups.
*/

#ifndef _DNS_H_
#define _DNS_H_

#include <zephyr.h>
#include <net/socket.h>

#define DNS_SERVER_IP4  134744072   //< Google DNS server.

/**
 *  @brief  Describes the possible states of the DNS module.
*/
enum dns_state {
    DNS_STATE_UNINITIALIZED,    //< Module has not been initilaized.
    DNS_STATE_READY,            //< Module has been initialized and is ready to receive requests.
    DNS_STATE_RESULT_READY      //< DNS resolution was successful and there are pending results.
};

/**
 *  @brief  Returns the current state of the DNS module.
 * 
 *  @return The current module state (see above for details).
*/
enum dns_state dns_get_module_state();

/**
 *  @brief Initializes the DNS module.
 * 
 *  @note   The module must be in the state DNS_STATE_UNINITIALIZED.
 * 
 *  @return 0 on success or a negative error code.
*/
int dns_init();

/**
 *  @brief  Uninitializes the DNS module.
 * 
 *  Pending results will be lost.
*/
void dns_uninit();

/**
 *  @brief  Tries to resolve the IPv4 address of a given host.
 * 
 *  @note   The module state must be DNS_STATE_READY.
 * 
 *  This function tres to resolve the IPv4 address of host. It returns the first entry of the
 *  result.
 * 
 *  @param  host    Hostname to resolve.
 *  @param  addr    Structure in which the host address will be written on success.
 * 
 *  @return 0 on success or a negative error code.
*/
int dns_resolve_one(const char *host, struct sockaddr_in *addr);

/**
 *  @brief  Tries to resolve the IPv4 address of a given host returning all found addresses.
 * 
 *  @note   The module state must be DNS_STATE_READY.
 * 
 *  This function tries to resolve the IPv4 address of host providing access to all returned
 *  entries.
 *  Values can be retrieved using dns_get_next_entry() as long as the module state is
 *  DNS_STATE_RESULT_READY.
 * 
 *  @param  host    Hostname to resolve.
 * 
 *  @return 0 on success or a negative error value.
*/
int dns_resolve_all(const char *host);

/**
 *  @brief  Gets the next DNS entry of the previous query.
 * 
 *  @note   The module state must be DNS_STATE_RESULT_READY.
 *  @note   An error can occur if there is a malformed entry. In this case -ENODATA is returned.
 * 
 *  @param  addr    Pointer to the address structure in which the next result is written.
 * 
 *  @return 0 on success or a negative error value.
*/
int dns_get_next_entry(struct sockaddr_in *addr);

/**
 *  @brief  This function ignores all pending results of a query.
 *  
 *  @note   The module state must be DNS_STATE_RESULT_READY.
 * 
 *  After the function returns the module is in the state DNS_STATE_READY if it has been
 *  initilized before.
 * 
 *  @return 0 on success or a negative error code in case the module has not been initilized.
*/
int dns_skip_all();

#endif
