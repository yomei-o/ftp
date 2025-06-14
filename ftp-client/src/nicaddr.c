#include <stdio.h>
#include <stdint.h>

#if defined(_WIN32)
#include <ws2tcpip.h>
#include <winsock.h>
#include <iphlpapi.h>
#endif

#ifdef _MSC_VER
#pragma comment(lib, "iphlpapi.lib")
#endif

#if defined(__APPLE__) || defined(__linux__)
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

static int is_linklocal_be(uint32_t addr_be) {
    return (addr_be & htonl(0xFFFF0000U)) == htonl(0xA9FE0000U);
}

static int is_linklocal_be_2(uint32_t addr_be) {
    return (addr_be & htonl(0xFFFF0000U)) == htonl(0xAc140000U);
}

uint32_t get_primary_ipv4_be(void) {
    char* env = getenv("FTP_SERVER_ADDRESS");
    if (env) {
        struct in_addr addr;
        if (inet_pton(AF_INET, env, &addr) != 1) {
            return -1;
        }
        uint32_t ip_be = addr.s_addr;
        return ip_be;
    }
#if defined(_WIN32)
    ULONG size = 0;
    GetAdaptersAddresses(AF_INET,
        GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
        NULL, NULL, &size);
    IP_ADAPTER_ADDRESSES* ad = malloc(size);
    if (!ad) return 0x7F000001U;
    if (GetAdaptersAddresses(AF_INET,
        GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
        NULL, ad, &size) != NO_ERROR) {
        free(ad);
        return 0x7F000001U;
    }
    for (IP_ADAPTER_ADDRESSES* p = ad; p; p = p->Next) {
        for (IP_ADAPTER_UNICAST_ADDRESS* u = p->FirstUnicastAddress; u; u = u->Next) {
            uint32_t addr_be = ((SOCKADDR_IN*)u->Address.lpSockaddr)
                ->sin_addr.S_un.S_addr;
            if (addr_be != htonl(INADDR_LOOPBACK) && !is_linklocal_be(addr_be) && !is_linklocal_be_2(addr_be)) {
                free(ad);
                return addr_be;
            }
        }
    }
    free(ad);
    return 0x7F000001U;

#elif defined(__APPLE__) || defined(__linux__)
    struct ifaddrs* ifa = NULL;
    if (getifaddrs(&ifa) != 0) return 0x7F000001U;

    uint32_t result = 0x7F000001U;
    for (struct ifaddrs* p = ifa; p; p = p->ifa_next) {
        if (p->ifa_addr && p->ifa_addr->sa_family == AF_INET) {
            uint32_t addr_be =
                ((struct sockaddr_in*)p->ifa_addr)->sin_addr.s_addr;
            if (addr_be != htonl(INADDR_LOOPBACK) && !is_linklocal_be(addr_be)) {
                result = addr_be;
                break;
            }
        }
    }
    freeifaddrs(ifa);
    return result;

#else
    return 0x7F000001U;
#endif
}


