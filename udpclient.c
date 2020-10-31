#include "udp.h"

#define PORT 8080
#define SERVERADDR "127.0.0.1"
const size_t MAXLINE = 1024;

int main(int argc, char **argv)
{
  int sockfd;
  int n;
  uint len;
  char *buffer = (char *)malloc(MAXLINE*sizeof(char));
  const char *hello = "Client1!";
  struct sockaddr_in serveraddr;

  // Creating socket
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd < 0)
  {
    perror("Failed create socket");
    exit(EXIT_FAILURE);
  }
  memset(&serveraddr, 0, sizeof(serveraddr));

  // Filling server information
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = inet_addr(SERVERADDR);
  serveraddr.sin_port = htons(PORT);

  len = sizeof(serveraddr);

  sendto(sockfd, (const char *)hello, strlen((const char *)hello), MSG_CONFIRM, (const struct sockaddr *)&serveraddr, len);
  printf("Sent!\n");

  n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *)&serveraddr, &len);
  buffer[n] = '\0';
  printf("Server: %s\n", buffer);

  return 0;
}