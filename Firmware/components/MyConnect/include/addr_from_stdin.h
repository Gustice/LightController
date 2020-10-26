#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "lwip/sys.h"
#include <lwip/netdb.h>
#include <arpa/inet.h>

esp_err_t get_addr_from_stdin(int port, int sock_type,
                              int *ip_protocol,
                              int *addr_family,
                              struct sockaddr_in6 *dest_addr);

#ifdef __cplusplus
}
#endif