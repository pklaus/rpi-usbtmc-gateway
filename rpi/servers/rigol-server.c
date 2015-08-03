/*
  rigol-server.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <linux/usb/tmc.h>
#include <readline/readline.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_RESPONSE_LENGTH (1024*1024+1024)
#define DEBUG_LEVEL 2

const int max_response_length = MAX_RESPONSE_LENGTH;
const int normal_response_length = 1024;

int rigol_write(int handle, unsigned char *string)
{
  int rc;
  unsigned char buf[256];
  strncpy(buf, string, sizeof(buf));
  buf[sizeof(buf)-1] = 0;
  strncat(buf, "\n", sizeof(buf));
  buf[sizeof(buf)-1] = 0;
  //printf("rigol_write(): \"%s\"\n",buf);
  rc = write(handle, buf, strlen(buf));
  if (rc<0) perror("write error");
  return(rc);
}

int rigol_read(int handle, unsigned char *buf, size_t size)
{
  int rc;
  if (!size) return(-1);
  buf[0] = 0;
  rc = read(handle, buf, size);
  if ((rc > 0) && (rc < size)) buf[rc] = 0;
  if (rc < 0) {
    if (errno == ETIMEDOUT) {
      printf("No response\n");
    } else {
      perror("read error");
    }
  }
  // printf("rc=%d\n",rc);
  return(rc);
}

int main(int argc, char **argv)
{
  char device[256]="/dev/usbtmc0";
  if(argc > 2 && strncmp(argv[1], "-D", 2) == 0)
  {
    strcpy(device, argv[2]);
    argc  -= 2;
    *argv += 2;
  }

  int handle;
  int rc;
  int i;
  unsigned char buf[max_response_length];
  handle = open(device, O_RDWR);
  if(handle < 0) {
    perror("error opening device");
    exit(1);
  }

  rigol_write(handle, "*IDN?");
  rigol_read(handle, buf, normal_response_length);
  printf("%s\n", buf);

  int socket_desc , client_sock , c , read_size;
  struct sockaddr_in server , client;
  char client_message[MAX_RESPONSE_LENGTH ];

  //Create socket
  socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  if (socket_desc == -1)
  {
    printf("Could not create socket");
  }
  puts("Socket created");

  //Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons( 5025 );

  //Bind
  if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
  {
    //print the error message
    perror("bind failed. Error");
    return 1;
  }
  puts("bind done");

  //Listen
  listen(socket_desc , 3);

  while(1)
  {
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
      perror("accept failed");
      return 1;
    }
    puts("Connection accepted");

    //Receive a message from client
    while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
    {
      char *cmd = &client_message;
      cmd[read_size] = 0;
      rigol_write(handle, cmd);
      if (DEBUG_LEVEL >= 2) printf(" wrote: %s\n", cmd);
      if(strchr(cmd, '?') == NULL) rc = 0;
      else
      {
        rc = rigol_read(handle, buf, max_response_length);
        if (DEBUG_LEVEL >= 2)
        {
          if (rc <= normal_response_length/2)
            printf(" read: %s\n", buf);
          else
          {
            printf(" read long message [%d bytes]:\n    ", rc);
            for(i=0; i<32; i++)
            {
              printf("%02X", buf[i]);
              if( (i%8) == 7 ) printf(" ");
              if( (i%16) == 15 ) printf("\n    ");
            }
            printf("\n    ...\n    ");
            for(i=0; i<32; i++)
            {
              printf("%02X", buf[rc-32+i]);
              if( (i%8) == 7 ) printf(" ");
              if( (i%16) == 15 ) printf("\n    ");
            }
            printf("    \n");
          }
        }
        if(rc <= 0)
          printf("  read error - [%d]\n", rc);
        else
          write(client_sock , buf, rc);
      }
    }

    if(read_size == 0)
    {
      puts("Client disconnected");
      fflush(stdout);
    }
    else if(read_size == -1)
    {
      perror("recv failed");
    }
  }

  return 0;
}
