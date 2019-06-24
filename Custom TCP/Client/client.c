#include "client.h"

void packet_init(struct packet* pkt)
{
  memset(pkt->buf, 0, NET_BUF_SIZE);
  pkt->seq_num = (pkt->seq_num+1)%2;
  pkt->ack = (pkt->ack+1)%2;
  pkt->last_pkt_size = 0;
}

void printAddr(struct sockaddr_in* addr)
{
  char *ip = inet_ntoa(addr->sin_addr);
  printf("IP %s\n", ip);
  printf("Port: %hu\n", ntohs(addr->sin_port));
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

// function to receive file
int writeData(struct packet* pkt, char* filename)
{
  FILE *write_ptr;
  size_t elems_read;

  write_ptr = fopen(filename,"a");  // a for append

  if(write_ptr == NULL)
  {
    fclose(write_ptr);
    return 1;
  }

  //write the content of pkt->buf on file pointed by write_ptr

  if (pkt->seq_num == 2) //last packet
  {
    fwrite(pkt->buf, pkt->last_pkt_size, 1, write_ptr);
    fclose(write_ptr);
    return 1;
  }

  else fwrite(pkt->buf, NET_BUF_SIZE, 1, write_ptr);

  fclose(write_ptr);

  return 0;
}


int main(int argc, char** argv)
{
  int clientfd, sockfd;
  int duplicate;
  int recvlen;
  int prev_pkt;
  int finish;
  int pkts_lost;
  short ack;
  int drop_acks;
  struct sockaddr_in servaddr, newaddr;
  socklen_t addrlen = sizeof(servaddr);
  struct packet pkt;
  size_t packetlen = sizeof(pkt);
  char filename[NET_BUF_SIZE];
  char serv_ip[16];
  char client_ip[16];

  struct timeval timeout = {TIMEOUT,0}; //10 seconds timeout

  if (argc != 5)
  {
    printf("Usage: ./client <ServerIPAddress/Hostname> <ServerPortNumber> <FilenameToSave> <ClientIPAddress/Hostname>\n");
    return 1;
  }

  bzero(&servaddr, addrlen);

  hostname2ip(argv[1], serv_ip);

  printf("%s mapped to %s\n", argv[1], serv_ip);

  int serv_port = atoi(argv[2]);

  strcpy(filename, argv[3]);

  printf("Saving file as: %s\n", filename);

  hostname2ip(argv[4] , client_ip);

  printf("%s mapped to %s\n", argv[4], client_ip);


  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(serv_port);
  servaddr.sin_addr.s_addr = inet_addr(serv_ip);



  printAddr(&servaddr);

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  if(sockfd < 0)
  {
    printf("[-]Error in connection[-].\n");
    exit(1);
  }

  int enable = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
  perror("setsockopt(SO_REUSEADDR) failed");

  if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
  perror("setsockopt(SO_RCVTIMEO) failed");

  printf("[+]Client socket created[+].\n");

  pkt.seq_num = 0;
  pkt.ack = 0;
  prev_pkt = 0;
  pkts_lost = 0;
  drop_acks = 0;
  int client_port;

  packet_init((struct packet*) &pkt); //pkt.seq_num == 1 && pkt.ack == 1

  printf("\nPress any button to start the download...\n");

  char c = getchar();

  //sends request to server
  sendto(sockfd, (char*) &c, sizeof(c), sendrecvflag, (struct sockaddr *) &servaddr, addrlen);
  printf("Sending request to server\n");
  printAddr(&servaddr);

  //server replies with a client port number, this port will be used from now on by the server to sends data to this client
  recvlen = recvfrom(sockfd, (int*) &client_port, sizeof(client_port), sendrecvflag, (struct sockaddr *) &servaddr, &addrlen);
  printf("Receiving port from:\n");
  printAddr(&servaddr);

  if (recvlen < 0)
  {
    printf("[-]Server doesn't reply[-]\n");
    return 0;
  }

  else printf("[+]Received client port: %i[+]\n",client_port);

  bzero(&newaddr, addrlen);

  //newaddr contains the new address for the client
  newaddr.sin_family = AF_INET;
  //defines client new port number
  newaddr.sin_port = htons(client_port);
  newaddr.sin_addr.s_addr = inet_addr(client_ip);

  printf("New client address:\n");
  printAddr(&newaddr);



  clientfd = socket(AF_INET, SOCK_DGRAM, 0);

  if(clientfd == -1)
  {
    printf("[-]Error in connection[-].\n");
    exit(1);
  }

  enable = 1;
  if (setsockopt(clientfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
  perror("setsockopt(SO_REUSEADDR) failed");

  if(setsockopt(clientfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
  perror("setsockopt(SO_RCVTIMEO) failed");

  //client receives pkts on new port
  if(bind(clientfd, (struct sockaddr*) &newaddr, addrlen) == -1)
  {
    printf("[-]Error in binding[-].\n");
    perror("bind");
    return 1;
  }

  printf("Setting new receiving client port to: %i\n",client_port);
  printf("Client:\n");
  printAddr(&newaddr);

  int server_port = client_port+100;

  //client sends to the server the new port where the client will send its acks
  sendto(sockfd, (int*) &server_port, sizeof(server_port), sendrecvflag, (struct sockaddr *) &servaddr, addrlen);
  printf("Sending server port to\n");
  printAddr(&servaddr);

  //defines server new port number
  servaddr.sin_port = htons(server_port);
  printf("Setting new server destination port to: %i\n",server_port);
  printf("Server:\n");
  printAddr(&servaddr);

  printf("Client\n");
  printAddr(&newaddr);


  if(DEBUG) printf("\n---------Data Received---------\n");

  while (1)
  {
    duplicate = 0;

    //receivs packet from server
    recvlen = recvfrom(clientfd, (struct packet*) &pkt, packetlen, sendrecvflag, (struct sockaddr *) &servaddr, &addrlen);

    if (recvlen < 0) //handles packet loss
    {
      if(DEBUG)
      {
        printf("No packet received\n");
        printf("+-----------------------------+\n");
      }

      pkts_lost++;

      if (pkts_lost >= 3)
      {
        if(DEBUG)
        {
          printf("Too many lost packets...aborting session\n");
          printf("+-----------------------------+\n");
        }
        break; //abort
      }

      continue; //retry
    }

    else if (strcmp(pkt.buf, nofile) == 0) //handles file not found
    {
      if(DEBUG)
      {
        printf("File not found\n");
        printf("+-----------------------------+\n");
      }
      break;
    }

    else
    {
      if (DEBUG)
      {
        printf("Receiving packet: %d with ack: %d\n", pkt.seq_num, pkt.ack);
        printf("+-----------------------------+\n");
        printf("Receiving pkt from\n");
        printAddr(&servaddr);
      }
    }

    pkts_lost = 0;

    if (prev_pkt == pkt.seq_num) duplicate = 1; //handles duplicate packet

    ack = pkt.ack;
    prev_pkt = pkt.seq_num;

    if(DEBUG)
    {
      //drops one ack every $(DROP_RATE)
      if (drop_acks != DROP_RATE)
      {
        sendto(clientfd, (short*) &ack, sizeof(ack), sendrecvflag, (struct sockaddr *) &servaddr, addrlen);
        printf("Sending ack: %d to\n", ack);
        printAddr(&servaddr);
        printf("+-----------------------------+\n");
      }

      drop_acks = (drop_acks + 1)%(DROP_RATE + 1);
    }

    //sends ack to server
    else
    {
      sendto(clientfd, (short*) &ack, sizeof(ack), sendrecvflag, (struct sockaddr *) &servaddr, addrlen);
    }

    if (duplicate) continue;

    // process
    finish = writeData((struct packet*) &pkt, (char*) &filename); //returns 1 when last packet

    if (finish)
    {
      printf("Download completed, ending client...\n");
      if(DEBUG) printf("+-----------------------------+\n");
      break;
    }

  }

  return 0;
}
