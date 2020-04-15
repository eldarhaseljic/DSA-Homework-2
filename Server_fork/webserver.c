// kompajlirajte sa: $gcc webserver.c -o webserver
//  i startajte sa $./webserver
#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>  // socket
#include<stdlib.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>

int main(void)
{
  int listenfd = 0, c = sizeof(struct sockaddr_in), 
      bytesReceived = 0, connfd = 0, bytesRead = 0;
  struct sockaddr_in serv_addr,client;
  //
  pid_t pid;
  //
  char recvBuff[512];
  char sendBuff[512];

  //Creating a socket
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd == -1)
    {
      perror("Could not create socket\n");
      return 1;
    }
  puts("Socket created");

  //Prepare the sockaddr_in structure
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  //serv_addr.sin_addr.s_addr = inet_addr("127.0.0.2");
  serv_addr.sin_port = htons( 8000 );

  //Bind
  if( bind( listenfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr) ) < 0)
   	{
      perror("Bind failed\n");
      return 1;
    }
  puts("Bind done");
  
  //Listening on a socket
  if( listen( listenfd, 10 ) == -1)
    {
      perror("Failed to listen\n");
      return 1;
    }
  puts("Waiting for incoming connections...");
  
  while(1) 
  {
    //Accepting connection
    connfd = accept( listenfd, (struct sockaddr *) &client, (socklen_t*) &c);
    if (connfd < 0)
      {
        perror("Accept failed\n");
   	    return 1;
      }
    puts("Connection accepted");
  
    //
    if((pid=fork())==0)
    {
      close(listenfd);
      puts("Child process created");
    //  
      char elements [3][40];
      if( ( bytesReceived = recv ( connfd, recvBuff, sizeof(recvBuff), 0) ) > 0)
        {       
          int cur_pos = 0, begin = 0,num = 0;
          while ( recvBuff[cur_pos] != '\r' && recvBuff[cur_pos+1] != '\n')
          {
            if(recvBuff[cur_pos] == ' ')
              {
                strncpy(elements[num], recvBuff + begin, cur_pos-begin);
                elements[num][cur_pos - begin] = '\0';
                ++num;
                begin = ++cur_pos;
              }
            ++cur_pos;
          }
        }
      else
        {
          perror("Recv failed");
          return 1; 
        }

    FILE *html;
    if(strcmp(elements[0],"GET") == 0)
      {  
        memset(sendBuff,'\0',sizeof(sendBuff));   
        if(strcmp(elements[1],"/") == 0)
          { 
            strcpy(elements[1],"/index.html");
          } 

        char naziv [sizeof(elements[1])-1];
        strncpy(naziv, &elements[1][1], sizeof(elements[1])-1);
        naziv [sizeof(elements[1])-1] = '\0';
        puts(naziv);
        html = fopen(naziv, "rb");        
        
        if(NULL == html)
          {
            sprintf(sendBuff,"%s 404 Not found \r\n \r\n\r\n",elements[2]);
            if(send(connfd, sendBuff, sizeof(sendBuff), 0) < 0)
              {
                puts("Send failed");
                return 1;
              }
          }            
        else
          {
            char length [30];
            fseek(html,0L,SEEK_END);
            long int size = ftell(html);  
            strcat(elements[2], " 200 OK \r\n");
            sprintf(length,"Content-length : %ld \r\n",size);
            strcat(elements[2],length);
            strcat(elements[2], "\r\n\r\n");
            int head_send=0;
            fseek(html,0L,SEEK_SET);
            while(!feof(html))
              {
                if(head_send==0)
                  {
                    strcpy(sendBuff,elements[2]);
                    bytesRead = sizeof(sendBuff);
                    head_send = 1; 
                  }
                else
                  {
                    bytesRead = fread(sendBuff, 1, sizeof(sendBuff), html);
                  }
                if(send(connfd, sendBuff, bytesRead, 0) < 0)
                  {
                    puts("Send failed");
                    return 1;
                  }
              }
        fclose(html);
          }
      }
    else
    {
      puts("Wrong option");
      return 1;
    }  
    
    close(connfd);
    exit(0);
    }
  close(connfd);
  }
  return 0;
}
