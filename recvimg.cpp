#include "udp.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#define PORT 8080
#define MAXLINE 256

void receiveFrame(int *socketfd, sockaddr_in *address, uint *lenaddress, cv::Mat *frame, const size_t sizeBuff, char *buffer)
{
  char *tempBuff = buffer;
  memset((char *)buffer, 0, sizeBuff);
  int n = recvfrom(*socketfd, (char *)buffer, 2, MSG_WAITALL, (sockaddr *)address, lenaddress);
  if(memcmp((const char *)buffer, (const char *)"<<", 2) == 0)
  {
    memset((char *)buffer, 0, sizeBuff);
    for(size_t i = 0; i < sizeBuff; i += n)
    {
      n = recvfrom(*socketfd, (char *)buffer+i, sizeBuff-i, MSG_WAITALL, (sockaddr *)address, lenaddress);
      // if(n > 0)
      // {
      //   printf("numBytes = %d\n", n);
      // }
      tempBuff+=n;
    }
    if(memcmp(tempBuff-2, ">>", 2) == 0)
    {
      fflush(stdout);
      printf("ok!\r");
      memcpy((uchar *)frame->data, (char *)buffer, sizeBuff-2);
    }
    // else
    //   memset(frame->data, 0, sizeBuff);
  }
}

int main(int argc, char **argv)
{
  cv::Mat frame = cv::Mat(240, 320, CV_8UC3, cv::Scalar::all(0));
  int keyB;
  size_t matSize = frame.cols*frame.rows*frame.channels();
  printf("matSize = %d\n", matSize);

  int sockfd;
  int n;
  uint lenclientaddr;
  size_t sizeBuff = (matSize + 2)*sizeof(char);
  char *buffer = (char *)malloc(sizeBuff);
  sockaddr_in clientaddr, serveraddr;

  // create socket
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd < 0)
  {
    perror("Failed create socket!");
    exit(EXIT_FAILURE);
  }
  memset(&serveraddr, 0, sizeof(serveraddr));
  memset(&clientaddr, 0, sizeof(clientaddr));

  // filling server information
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = INADDR_ANY;
  serveraddr.sin_port = htons(PORT);

  // bind socket
  if(bind(sockfd, (const sockaddr *)&serveraddr, sizeof(serveraddr)))
  {
    perror("Failed bind socket!");
    exit(EXIT_FAILURE);
  }

  // create timeout
  timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 100000;
  if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)))
  {
    perror("Timeout error");
  }

  cv::namedWindow("received", cv::WINDOW_AUTOSIZE);// | cv::WINDOW_OPENGL);
  printf("ready!\n");
  while(1)
  {
    receiveFrame(&sockfd, &clientaddr, &lenclientaddr, &frame, sizeBuff, buffer);
    cv::imshow("received", frame);
    keyB = cv::waitKey(5);
    if(keyB == 'q')
      break;
  }
  free(buffer);
  return 0;
}