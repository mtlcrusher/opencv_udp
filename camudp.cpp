#include "udp.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#define PORT 8080
#define MAXLINE 256
#define SERVERADDR "127.0.0.1"

int sendFrame(int *socketfd, sockaddr_in *address, uint *lenaddress, cv::Mat *frame, const uint matSize, const size_t packetSize)
{
  // send start frame
  int nSend = sendto(*socketfd, (const char *)"_START_", 7, MSG_CONFIRM, (const sockaddr *)address, *lenaddress);
  if(nSend < 0)
  {
    printf("max packet size is 65507 Bytes!\n");
    perror("sendto error!");
    exit(EXIT_FAILURE);
  }
  // send frame
  int totalbytes = 0;
  size_t bytes = packetSize; 
  for(size_t i = 0; i < matSize; i += nSend)
  {
    size_t spare = matSize - i;
    if(spare < bytes)
      bytes = spare;
    nSend = sendto(*socketfd, (const uchar *)frame->data + i, bytes, MSG_CONFIRM, (const sockaddr *)address, *lenaddress);
    if(nSend < 0)
    {
      printf("max packet size is 65507 Bytes!\n");
      perror("sendto error!");
      exit(EXIT_FAILURE);
    }
    totalbytes+=nSend;
  }
  return totalbytes;
}

int main(int argc, char **argv)
{
  if(argc < 2)
  {
    printf("How to use:\n");
    printf("  run: ./cam [packetSize]\n");
    printf("  Example: ./cam 64000\n");
    printf("  Maximum packetSize is 65507 Bytes!\n");
    return -1;
  }
  cv::Mat frame, framecont;
  int keyB;

  int sockfd;
  int nSend, nRecv, totalbytes;
  uint lenserveradr;
  char *buffer = (char *)malloc(MAXLINE*sizeof(char));
  const char *hello = "Hello!";
  sockaddr_in serveraddr;

  // // create timeout
  // timeval tv;
  // tv.tv_sec = 0;
  // tv.tv_usec = 100000;
  // if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)))
  // {
  //   perror("Timeout error");
  // }

  // create socket
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

  lenserveradr = sizeof(serveraddr);

  cv::VideoCapture camera(0);
  camera.set(3, 320);
  camera.set(4, 240);
  if(!camera.isOpened())
  {
    printf("Failed opening camera!\n");
    return -1;
  }

  cv::namedWindow("frame", cv::WINDOW_AUTOSIZE | cv::WINDOW_OPENGL);
  camera.read(frame);
  size_t matSize = frame.rows * frame.cols * frame.channels(); //230400
  size_t dataSize = atoi(argv[1]);
  printf("matSize = %d\n", matSize);
  while(1)
  {
    camera.read(frame);
    frame.copyTo(framecont);
    // cv::cvtColor(frame, framecont, cv::COLOR_BGR2GRAY);
    cv::imshow("frame", frame);
    keyB = cv::waitKey(10);
    if(keyB == 'q')
      break;

    sendFrame(&sockfd, &serveraddr, &lenserveradr, &framecont, matSize, dataSize);
  }
  camera.release();
  free(buffer);
  return 0;
}