#include "server.h"

//initialize the packet buffer to 0
//and increments both seq_num and ack
void packet_init(struct packet* pkt)
{
  memset(pkt->buf, 0, NET_BUF_SIZE);
  pkt->ack = (pkt->ack+1)%2;
  if (pkt->seq_num == 2) pkt->seq_num = 0;
  else pkt->seq_num = (pkt->seq_num+1)%2;
  pkt->last_pkt_size = 0;
}

int hostname2ip(char* hostname , char* ip)
{
  struct hostent *he;
  struct in_addr **addr_list;

  if ((he = gethostbyname(hostname)) == NULL)
  {
    herror("gethostbyname");
    return 1;
  }

  addr_list = (struct in_addr **) he->h_addr_list;

  for (int i = 0; addr_list[i] != NULL; i++)
  {
    strcpy(ip , inet_ntoa(*addr_list[i]));
    return 0;
  }

    return 1;
}

void printBuf(struct packet pkt)
{
  printf("Packet buffer contains:\n");
  for (int i = 0; i < NET_BUF_SIZE; i++)
  printf("%c", pkt.buf[i]);

  printf("\n");
}

int readData(FILE* fp, struct packet* pkt)
{
  size_t elems_sent;

  if (fp == NULL)
  {
    strcpy(pkt->buf, nofile);
    pkt->seq_num = 2;//set special seq_num for final packet
    return 1;
  }

  //read NET_BUF_SIZE bytes from fp and stores them in pkt->buf
  elems_sent = fread(pkt->buf, 1, NET_BUF_SIZE, fp);

  // handles EOF
  if (feof(fp))
  {
    pkt->seq_num = 2; //set special seq_num for final packet
    pkt->last_pkt_size = elems_sent;
    return 1;
  }

  return 0;
}

void printAddr(struct sockaddr_in* cliaddr)
{
  char *ip = inet_ntoa(cliaddr->sin_addr);
  printf("IP: %s\n", ip);
  printf("Port: %hu\n", ntohs(cliaddr->sin_port));
}

int main(int argc, char** argv)
{
  int sockfd, childfd, recvlen;
  int drop_pkts, delivered_pkts;
  short ack, last_ack;
  struct sockaddr_in servaddr, cliaddr;
  int n_resent_pkts, n_pkts_to_send;
  socklen_t addrlen = sizeof(servaddr);
  struct packet pkt;
  size_t packetlen = sizeof(pkt);
  FILE* fp;
  char filename[NET_BUF_SIZE];
  int server_port;
  char serv_ip[16];

  pid_t childpid;

  struct timeval timeout = {TIMEOUT, 0};

  if (argc != 4)
  {
    printf("Usage: ./server <FilenameToSend> <ServerPortNumber> <ServerIPAddress/Hostname>\n");
    return 1;
  }

  strcpy(filename, argv[1]);

  printf("Filename: %s\n", filename);

  int serv_port = atoi(argv[2]);

  hostname2ip(argv[3] , serv_ip);

  printf("%s mapped to %s\n", argv[3], serv_ip);

  bzero(&servaddr, addrlen);

  //set server address
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(serv_port);  //specify port number
  servaddr.sin_addr.s_addr = inet_addr(serv_ip);  //specify IP address

  if(DEBUG)
  {
    printf("Server address:\n");
    printAddr(&servaddr);
  }

  //creates an endpoint for communication and returns a descriptor.
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  if(sockfd == -1)
  {
    perror("socket");
  }

  int enable = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
  perror("setsockopt(SO_REUSEADDR) failed");

  //socket is bound to all local interfaces
  //sets listening port, receiving end
  if(bind(sockfd, (struct sockaddr*) &servaddr, addrlen) == -1)
  {
    printf("[-]Error in binding[-]\n");
    return 1;
  }

  pkt.seq_num = 0;
  pkt.ack = 0;
  ack = 0;

  n_resent_pkts = 0;
  drop_pkts = 0;
  delivered_pkts = 0;

  int client_port = serv_port;
  char c;

  while(1)
  {
    printf("\nWaiting for client...\n");

    //receive request from client
    recvfrom(sockfd, (char*) &c, sizeof(c), sendrecvflag, (struct sockaddr*) &cliaddr, &addrlen);
    printf("Receiving download request from:\n");
    printAddr(&cliaddr);

    client_port++;

    //sends to client the new client port
    sendto(sockfd, (int*) &client_port, sizeof(client_port), sendrecvflag, (struct sockaddr *) &cliaddr, addrlen);
    printf("Sending client port %i to:\n", client_port);
    printAddr(&cliaddr);

    //receives from client the new server port
    recvlen = recvfrom(sockfd, (int*) &server_port, sizeof(server_port), sendrecvflag, (struct sockaddr*) &cliaddr, &addrlen);
    printf("Receiving server port %i from:\n", server_port);
    printAddr(&cliaddr);

    if (recvlen < 0)
    {
      printf("Client didn't reply back\n");
      continue;
    }

    //create new process to deliver data to requesting client
    childpid = fork();
    if(childpid == 0)
    {

      printf("Setting new server listening port to: %i\n",server_port);
      servaddr.sin_port = htons(server_port);
      printf("Server:\n");
      printAddr(&servaddr);

      printf("Setting new client destination port to: %i\n",client_port);
      cliaddr.sin_port = htons(client_port);
      printf("Client:\n");
      printAddr(&cliaddr);

      //new socket for listening to the new port
      childfd = socket(AF_INET, SOCK_DGRAM, 0);

      if(childfd < 0)
      {
        printf("[-]Error in connection[-].\n");
        exit(1);
      }

      printf("[+]Child correctly binded[+]\n");

      int server_port;

      enable = 1;

      if (setsockopt(childfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
      perror("setsockopt(SO_REUSEADDR) failed");

      if(bind(childfd, (struct sockaddr*) &servaddr, addrlen) == -1)
      {
        printf("[-]Error in binding[-]\n");
        perror("bind");
        return 1;
      }

      if(setsockopt(childfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
      perror("setsockopt(SO_RCVTIMEO) failed");

      fp = fopen(filename, "rb"); //read binary

      if (fp == NULL)
      printf("\nFile open failed!\n");
      else
      printf("\nFile successfully opened!\n");

      while (1)
      {

        packet_init((struct packet*) &pkt);
        readData(fp, (struct packet*) &pkt);

        do
        {

          last_ack = ack;

          if(DEBUG)
          {
            //drops one packet every $(DROP_RATE)
            if (drop_pkts != DROP_RATE)
            {
              sendto(childfd, (struct packet*) &pkt, packetlen, sendrecvflag, (struct sockaddr *) &cliaddr, addrlen);
              printf("Sending packet: %d with ack: %d to:\n", pkt.seq_num, pkt.ack);
              printAddr(&cliaddr);
              printf("+-----------------------------+\n");
            }

            drop_pkts = (drop_pkts + 1)%(DROP_RATE+1);
          }

          else
          {
            //sends packet to client
            sendto(childfd, (struct packet*) &pkt, packetlen, sendrecvflag, (struct sockaddr *) &cliaddr, addrlen);
          }

          if (fp == NULL) break;

          //receives ack from client
          recvlen = recvfrom(childfd, (short*) &ack, sizeof(ack), sendrecvflag, (struct sockaddr *) &cliaddr, &addrlen);

          if(DEBUG)
          {
            printf("Receiving ack from:\n");
            printAddr(&cliaddr);
          }

          if (recvlen < 0) //handles missing packet
          {
            if(DEBUG) printf("No ack received...\n");
            n_resent_pkts++;
            continue;
          }

          else
          {
            if(DEBUG)
            {
              printf("Received ack: %d\n", ack);
              printf("+-----------------------------+\n");
            }
          }

          if(last_ack == ack) n_resent_pkts++; //debug info

        } while(last_ack == ack); //handles duplicate acks

        if(pkt.seq_num == 2) break; //handles last packet

      }

      fclose(fp);
      if(DEBUG) printf("Number of re-sent packets: %i\n", n_resent_pkts);
      printf("Download completed\n");
      if(DEBUG) printf("+-----------------------------+\n");
      close(childfd);
      _exit(0);

    }
  }
  return 0;
}
