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
    printf("  ifshow -i ifname : Display IP addresses for the specified interface.\n");
    printf("  ifshow -a        : Display names and IP addresses for all interfaces.\n");
    exit(EXIT_FAILURE);
}

// Function to display IP addresses for a specific interface
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
            printf("IPv4 Address: %s\n", inet_ntoa(sa->sin_addr));
        } else if (ifa->ifa_addr->sa_family == AF_INET6) {
            struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)ifa->ifa_addr;
            char ip6[INET6_ADDRSTRLEN];
            printf("IPv6 Address: %s\n", inet_ntop(AF_INET6, &(sa6->sin6_addr), ip6, INET6_ADDRSTRLEN));
        }
    }

    freeifaddrs(ifap);
}

// Function to display names and IP addresses for all interfaces
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
            printf("IPv4 Address: %s\n", inet_ntoa(sa->sin_addr));
        } else if (ifa->ifa_addr->sa_family == AF_INET6) {
            struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)ifa->ifa_addr;
            char ip6[INET6_ADDRSTRLEN];
            printf("IPv6 Address: %s\n", inet_ntop(AF_INET6, &(sa6->sin6_addr), ip6, INET6_ADDRSTRLEN));
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
        // Display IP addresses for the specified interface
        displayInterfaceInfo(argv[2]);
    } else if (strcmp(argv[1], "-a") == 0 && argc == 2) {
        // Display names and IP addresses for all interfaces
        displayAllInterfacesInfo();
    } else {
        displayUsage();
    }

    return 0;
}
