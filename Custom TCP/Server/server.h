#pragma once

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <netdb.h>

#define NET_BUF_SIZE 32
#define nofile "File Not Found!"
#define sendrecvflag 0
#define DEBUG 0
#define TIMEOUT 10
#define DROP_RATE 15
#define QUEUE_SIZE 10

struct packet
{
  short seq_num;
  short ack;
  int last_pkt_size;
  char buf[NET_BUF_SIZE];
};
