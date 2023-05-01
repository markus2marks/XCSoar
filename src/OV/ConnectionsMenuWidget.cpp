/*
 * SystemMenuWidget.cpp
 *
 *  Created on: May 1, 2023
 *      Author: markus
 */
#include "ConnectionsMenuWidget.h"

#include <arpa/inet.h>
#include <cerrno>
#include <ifaddrs.h>
#include <iostream>
#include <net/if.h>
#include <string>
#include <string.h>
#include <sysexits.h>
#include <sys/socket.h>
#include <sys/types.h>

//static int check_wireless(const char* ifname, char* protocol) {
//  int sock = -1;
//  struct iwreq pwrq;
//  memset(&pwrq, 0, sizeof(pwrq));
//  strncpy(pwrq.ifr_name, ifname, IFNAMSIZ);
//
//  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
//    perror("socket");
//    return 0;
//  }
//
//  if (ioctl(sock, SIOCGIWNAME, &pwrq) != -1) {
//    if (protocol) strncpy(protocol, pwrq.u.name, IFNAMSIZ);
//    close(sock);
//    return 1;
//  }
//
//  close(sock);
//  return 0;
//}

void ConnectionsMenuWidget::Prepare([[maybe_unused]] ContainerWindow &parent,
                          [[maybe_unused]] const PixelRect &rc) noexcept
{
  struct ifaddrs* ptr_ifaddrs = nullptr;

      auto result = getifaddrs(&ptr_ifaddrs);
      if( result != 0 ){
          std::cout << "`getifaddrs()` failed: " << strerror(errno) << std::endl;

//          return EX_OSERR;
      }

      for(
          struct ifaddrs* ptr_entry = ptr_ifaddrs;
          ptr_entry != nullptr;
          ptr_entry = ptr_entry->ifa_next
      ){
          std::string ipaddress_human_readable_form;
          std::string netmask_human_readable_form;

          std::string interface_name = std::string(ptr_entry->ifa_name);
          sa_family_t address_family = ptr_entry->ifa_addr->sa_family;
          if( address_family == AF_INET ){
              // IPv4

              // Be aware that the `ifa_addr`, `ifa_netmask` and `ifa_data` fields might contain nullptr.
              // Dereferencing nullptr causes "Undefined behavior" problems.
              // So it is need to check these fields before dereferencing.
              if( ptr_entry->ifa_addr != nullptr ){
                  char buffer[INET_ADDRSTRLEN] = {0, };
                  inet_ntop(
                      address_family,
                      &((struct sockaddr_in*)(ptr_entry->ifa_addr))->sin_addr,
                      buffer,
                      INET_ADDRSTRLEN
                  );

                  ipaddress_human_readable_form = std::string(buffer);
              }

              if( ptr_entry->ifa_netmask != nullptr ){
                  char buffer[INET_ADDRSTRLEN] = {0, };
                  inet_ntop(
                      address_family,
                      &((struct sockaddr_in*)(ptr_entry->ifa_netmask))->sin_addr,
                      buffer,
                      INET_ADDRSTRLEN
                  );

                  netmask_human_readable_form = std::string(buffer);
              }

              std::cout << interface_name << ": IP address = " << ipaddress_human_readable_form << ", netmask = " << netmask_human_readable_form << std::endl;
          }
          else if( address_family == AF_INET6 ){
              // IPv6
              uint32_t scope_id = 0;
              if( ptr_entry->ifa_addr != nullptr ){
                  char buffer[INET6_ADDRSTRLEN] = {0, };
                  inet_ntop(
                      address_family,
                      &((struct sockaddr_in6*)(ptr_entry->ifa_addr))->sin6_addr,
                      buffer,
                      INET6_ADDRSTRLEN
                  );

                  ipaddress_human_readable_form = std::string(buffer);
                  scope_id = ((struct sockaddr_in6*)(ptr_entry->ifa_addr))->sin6_scope_id;
              }

              if( ptr_entry->ifa_netmask != nullptr ){
                  char buffer[INET6_ADDRSTRLEN] = {0, };
                  inet_ntop(
                      address_family,
                      &((struct sockaddr_in6*)(ptr_entry->ifa_netmask))->sin6_addr,
                      buffer,
                      INET6_ADDRSTRLEN
                  );

                  netmask_human_readable_form = std::string(buffer);
              }

              std::cout << interface_name << ": IP address = " << ipaddress_human_readable_form << ", netmask = " << netmask_human_readable_form << ", Scope-ID = " << scope_id << std::endl;
          }
          else {
              // AF_UNIX, AF_UNSPEC, AF_PACKET etc.
              // If ignored, delete this section.
          }
      }

      freeifaddrs(ptr_ifaddrs);
}


