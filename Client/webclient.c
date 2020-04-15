// kompajlirajte sa: $gcc webclient.c -o webclient 
//i startajte sa $./webclient   
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include<unistd.h>

int main(int argc, char * argv[])
{
    int sockfd = 0, bytesRead=0, bytesRecieved = 0;
    char sendBuffer[512];
    char recvBuffer[512];
    struct sockaddr_in serv_addr;

    /* Create a socket first */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
      printf("\n Error : Could not create socket \n");
      return 1;
    }
    puts("Socket created");

    /* Initialize sockaddr_in data structure */
    	serv_addr.sin_addr.s_addr = INADDR_ANY;
    	serv_addr.sin_family = AF_INET;
    	serv_addr.sin_port = htons( 8000 );
   	//serv_addr.sin_addr.s_addr = inet_addr("192.168.1.5");

    /* Attempt a connection */
    if( connect(sockfd, (struct sockaddr *)&serv_addr, 
        sizeof(serv_addr))<0)
    {
      printf("\n Error : Connect Failed \n");
      return 1;
    }
   	puts("Connected\n");
    /////////////////////////////////////////

  memset(sendBuffer, '\0', sizeof(sendBuffer));
  //char * message = "GET / HTTP/1.1 \r\n\r\n";
  char * message = "GET /folder/index.html HTTP/1.0 \r\n\r\n";
  strcpy(sendBuffer,message);

  while( send(sockfd , sendBuffer , sizeof(sendBuffer) , 0) < 0)
    { 
      puts("Send failed");
      return 1;
    }

  while( ( bytesRecieved = recv(sockfd, recvBuffer, sizeof(recvBuffer), 0 ) ) > 0)
  {
    recvBuffer[bytesRecieved] = 0x0;
    puts(recvBuffer);
  }
 
  close(sockfd);	

  return 0;
}
