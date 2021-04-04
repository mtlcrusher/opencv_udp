#include "udp.h"

#define PORT 8080
#define SERVERADDR "127.0.0.1"
const size_t MAXLINE = 1024;

// create a thread for UDP task
void *udpThread(void *params)
{
  printf("Creating UDP Thread!\n");
  int sockfd;
  int n;
  socklen_t len;
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
  while (1)
  {
    sendto(sockfd, (const char *)hello, strlen((const char *)hello), MSG_CONFIRM, (const struct sockaddr *)&serveraddr, len);
    // printf("Sent!\n");

    n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *)&serveraddr, &len);
    buffer[n] = '\0';
    printf("Server: %s\n", buffer);
    usleep(100000);
  }
}

void *dummyTask(void *params)
{
  while (1)
  {
    printf("Dummy Task yo!\n");
    sleep(5);
  }  
}

int main(int argc, char **argv)
{
  pthread_t udpThreadId, dummyTaskId;
  pthread_create(&udpThreadId, NULL, udpThread, NULL);
  pthread_create(&dummyTaskId, NULL, dummyTask, NULL);

  printf("Here!\n");

  while(1); // program runs forevaaaaa!
  return 0;
}