#pragma once

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>

#define NET_BUF_SIZE 32
#define nofile "File Not Found!"
#define sendrecvflag 0
#define DEBUG 0
#define TIMEOUT 5
#define DROP_RATE 20


struct packet
{
  short seq_num;
  short ack;
  int last_pkt_size;
  char buf[NET_BUF_SIZE];
};
