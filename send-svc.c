/*
    Listen to UDP packets on a specific port, to command daemontools services
    Copyright (C) 2022  Lester Vecsey

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netdb.h>

#include <string.h>

#include "svc_pkt.h"

int main(int argc, char *argv[]) {

  struct protoent *pent;

  svc_pkt msg;
  
  size_t len;
  socklen_t addrlen = sizeof(struct sockaddr_in);
  struct sockaddr_in bind_addr, dest_addr;
  
  unsigned char ip_address[4];

  unsigned char bind_ip[4] = { 0, 0, 0, 0 };
  
  uint16_t port;

  int retval;

  long int n;
  
  int s;

  port = def_port;
  
  pent = getprotobyname("UDP");
  if (pent==NULL) {
    perror("getprotobyname");
    return -1;
  }

  s = socket(AF_INET, SOCK_DGRAM, pent->p_proto); 
  if (s==-1) {
    perror("socket");
    return -1;
  }

  for (n = 0; n < 10; n++) {
  
    memset(&bind_addr, 0, sizeof(bind_addr));
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_port = htons(port+n);
    memcpy(&bind_addr.sin_addr.s_addr, bind_ip, 4); 
  
    retval = bind(s, &bind_addr, addrlen);

    if (!retval) {
      printf("Successfully bound to bind_ip %d.%d.%d.%d\n", bind_ip[0], bind_ip[1], bind_ip[2], bind_ip[3]);
      break;
    }
    
  }

  if (n == 10) {
    printf("Trouble binding to ip_address %d.%d.%d.%d\n", bind_ip[0], bind_ip[1], bind_ip[2], bind_ip[3]);
    return -1;
  }      
  
  {

    long int argiter;

    long int ip[4];

    len = strlen(argv[1]);

    if (len > sizeof(msg.service_name)) {
      len = sizeof(msg.service_name);
    }

    memset(msg.service_name, 0, sizeof(msg.service_name));

    strncpy(msg.service_name, argv[1], len);
    
    printf("%s: Starting up the cluster.\n", __FUNCTION__);

    for (argiter = 2; argiter < argc; argiter++) {

      printf("Sending to node %s\n", argv[argiter]);

      retval = sscanf(argv[argiter], "%ld.%ld.%ld.%ld", ip+0, ip+1, ip+2, ip+3);
      if (retval != 4) {
	printf("Mismatch for ip address %s\n", argv[argiter]);
	continue;
      }

      ip_address[0] = ip[0];
      ip_address[1] = ip[1];
      ip_address[2] = ip[2];
      ip_address[3] = ip[3];      
      
      memset(&dest_addr, 0, sizeof(dest_addr));
      dest_addr.sin_family = AF_INET;
      dest_addr.sin_port = htons(port);
      memcpy(&dest_addr.sin_addr.s_addr, ip_address, 4); 
      
      len = sizeof(msg);
      addrlen = sizeof(struct sockaddr_in);
      retval = sendto(s, &msg, len, 0, &dest_addr, addrlen);
      if (retval != len) {
	perror("sendto");
	return -1;
      }
      
    }

  }
    
  return 0;

}
