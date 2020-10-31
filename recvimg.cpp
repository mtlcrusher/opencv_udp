#include "udp.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#define PORT 8080
#define MAXLINE 256

int main(int argc, char **argv)
{
  cv::Mat frame = cv::Mat(240, 320, CV_8UC3, cv::Scalar::all(0));
  int keyB;
  size_t matSize = frame.cols*frame.rows*frame.channels();
  printf("matSize = %d\n", matSize);

  int sockfd;
  int n;
  uint lenclientaddr;
  char *buffer = (char *)malloc(matSize*sizeof(char));
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

  cv::namedWindow("received", cv::WINDOW_AUTOSIZE | cv::WINDOW_OPENGL);
  printf("ready!\n");
  while(1)
  {
    memset((char *)buffer, 0, matSize);
    n = recvfrom(sockfd, (char *)buffer, 7, MSG_WAITALL, (sockaddr *)&clientaddr, &lenclientaddr);
    if(memcmp((const char *)buffer, (const char *)"_START_", 7) == 0)
    {
      memset((char *)buffer, 0, matSize);
      for(size_t i = 0; i < matSize; i += n)
      {
        n = recvfrom(sockfd, (char *)buffer+i, matSize-i, MSG_WAITALL, (sockaddr *)&clientaddr, &lenclientaddr);
        // if(n > 0)
        // {
        //   printf("numBytes = %d\n", n);
        // }
      }
      memcpy((uchar *)frame.data, (char *)buffer, matSize);
    }
    cv::imshow("received", frame);
    keyB = cv::waitKey(10);
    if(keyB == 'q')
      break;
  }
  free(buffer);
  return 0;
}