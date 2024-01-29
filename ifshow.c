#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Function to display usage information
void displayUsage() {
    printf("Usage:\n");
    printf("  ifshow -i ifname : Display IP addresses and subnet masks for the specified interface.\n");
    printf("  ifshow -a        : Display names, IP addresses, and subnet masks for all interfaces.\n");
    exit(EXIT_FAILURE);
}

// Function to display IP addresses and subnet masks for a specific interface
void displayInterfaceInfo(char *ifname) {
    struct ifaddrs *ifap, *ifa;

    if (getifaddrs(&ifap) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL || strcmp(ifa->ifa_name, ifname) != 0)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
            struct sockaddr_in *netmask = (struct sockaddr_in *)ifa->ifa_netmask;

            // Count the number of leading 1s in the 32-bit netmask
            int cidr = 0;
            uint32_t mask = ntohl(netmask->sin_addr.s_addr);
            while ((mask & 0x80000000) == 0x80000000) {
                cidr++;
                mask <<= 1;
            }

            printf("IPv4 Address: %s/%d\n", inet_ntoa(sa->sin_addr), cidr);
        } else if (ifa->ifa_addr->sa_family == AF_INET6) {
            struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)ifa->ifa_addr;
            char ip6[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &(sa6->sin6_addr), ip6, INET6_ADDRSTRLEN);

            struct sockaddr_in6 *netmask6 = (struct sockaddr_in6 *)ifa->ifa_netmask;

            // Count the number of leading 1s in each 16-bit block of the 128-bit netmask
            int cidr6 = 0;
            for (int i = 0; i < 8; i++) {
                uint16_t block = ntohs(netmask6->sin6_addr.s6_addr16[i]);
                while ((block & 0x8000) == 0x8000) {
                    cidr6++;
                    block <<= 1;
                }
            }

            printf("IPv6 Address: %s/%d\n", ip6, cidr6);
        }
    }

    freeifaddrs(ifap);
}

// Function to display names, IP addresses, and subnet masks for all interfaces
void displayAllInterfacesInfo() {
    struct ifaddrs *ifap, *ifa;

    if (getifaddrs(&ifap) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        printf("Interface Name: %s\n", ifa->ifa_name);

        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
            struct sockaddr_in *netmask = (struct sockaddr_in *)ifa->ifa_netmask;

            int cidr = 0;
            uint32_t mask = ntohl(netmask->sin_addr.s_addr);
            while ((mask & 0x80000000) == 0x80000000) {
                cidr++;
                mask <<= 1;
            }

            printf("IPv4 Address: %s/%d\n", inet_ntoa(sa->sin_addr), cidr);
        } else if (ifa->ifa_addr->sa_family == AF_INET6) {
            struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)ifa->ifa_addr;
            char ip6[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &(sa6->sin6_addr), ip6, INET6_ADDRSTRLEN);

            struct sockaddr_in6 *netmask6 = (struct sockaddr_in6 *)ifa->ifa_netmask;

            int cidr6 = 0;
            for (int i = 0; i < 8; i++) {
                uint16_t block = ntohs(netmask6->sin6_addr.s6_addr16[i]);
                while ((block & 0x8000) == 0x8000) {
                    cidr6++;
                    block <<= 1;
                }
            }

            printf("IPv6 Address: %s/%d\n", ip6, cidr6);
        }

        printf("\n");
    }

    freeifaddrs(ifap);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        displayUsage();
    }

    // Parse command line arguments
    if (strcmp(argv[1], "-i") == 0 && argc == 3) {
        // Display IP addresses and subnet masks for the specified interface
        displayInterfaceInfo(argv[2]);
    } else if (strcmp(argv[1], "-a") == 0 && argc == 2) {
        // Display names, IP addresses, and subnet masks for all interfaces
        displayAllInterfacesInfo();
    } else {
        displayUsage();
    }

    return 0;
}
