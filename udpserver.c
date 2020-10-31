#include "udp.h"

#define PORT 8080
const size_t MAXLINE = 1024;

int main(int argc, char **argv)
{
  int sockfd;
  int bindStatus;
  int n;
  uint len;
  char *buffer = (char *)malloc(MAXLINE*sizeof(char));
  const char *hello = "Server-kun!";
  struct sockaddr_in serveraddr, clientaddr;

  // Creating socket file descriptor
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd < 0)
  {
    perror("Failed create socket!");
    exit(EXIT_FAILURE);
  }
  memset(&serveraddr, 0, sizeof(serveraddr));
  memset(&clientaddr, 0, sizeof(clientaddr));

  // Filling server information
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = INADDR_ANY;
  serveraddr.sin_port = htons(PORT);

  bindStatus = bind(sockfd, (const struct sockaddr *)&serveraddr, sizeof(serveraddr));
  if(bindStatus < 0)
  {
    perror("Bind failed!");
    exit(EXIT_FAILURE);
  }

  len = sizeof(clientaddr);
  while(1)
  {
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *)&clientaddr, &len);
    buffer[n] = '\0';
    if(n > 0) printf("Client: %s\n", buffer);
  }

  // sendto(sockfd, (const char *)hello, strlen(hello), MSG_CONFIRM, (const struct sockaddr *)&clientaddr, len);
  // printf("Sent!\n");

  return 0;
}