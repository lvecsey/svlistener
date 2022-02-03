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

#include <stdint.h>

#include <sys/socket.h>
#include <netdb.h>

#include "svc_pkt.h"

#define service_basepath "/etc/service"

int main(int argc, char *argv[]) {

  struct protoent *pent;
  
  unsigned char buf[512];
  size_t len;
  socklen_t addrlen = sizeof(struct sockaddr_in);
  struct sockaddr_in bind_addr, dest_addr;

  svc_pkt *msg;
  
  ssize_t bytes_read;

  unsigned char ip_address[4] = { 0, 0, 0, 0 };

  int flags;

  int s;

  int retval;

  uint16_t port;

  char cmd[240];

  flags = 0;
  
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

  memset(&bind_addr, 0, sizeof(bind_addr));

  bind_addr.sin_family = AF_INET;
  bind_addr.sin_port = htons(port);
  memcpy(&bind_addr.sin_addr.s_addr, ip_address, 4); 
  
  retval = bind(s, (const struct sockaddr *) &bind_addr, addrlen);

  msg = (svc_pkt*) buf;
  
  for (;;) {

    memset(buf, 0, sizeof(buf));
    
    len = sizeof(buf);
    addrlen = sizeof(struct sockaddr_in);
    bytes_read = recvfrom(s, buf, len, flags, (struct sockaddr*) &dest_addr, &addrlen); 

    if (bytes_read >= sizeof(svc_pkt)) {

      fprintf(stderr, "%s: Received msgpkt.\n", __FUNCTION__);

      retval = sprintf(cmd, "svc -o %s/%s", service_basepath, msg->service_name);

      retval = system(cmd);
      
    }
    
    if (bytes_read <= 0) break;

  }
      
  retval = close(s);
  if (retval == -1) {
    perror("close");
    return -1;
  }

  return 0;

}
