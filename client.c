#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>


#define MAX_SIZE 50
char clientUserName[100];
void *listener(void *);
void file_to_server(int sock_desc, char * path);
void store_file(int sock);

int main(int argc , char *argv[])
{
    if (argc < 2)
    {
      printf("Error: Enter Port Number\n");
      exit(0);
    }
    int sock_desc;
    struct sockaddr_in serv_addr;
    char sbuff[MAX_SIZE],rbuff[MAX_SIZE];
    pthread_t sniffer_thread;
    int *new_sock;
    char filepath[50];

    if((sock_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        printf("Failed creating socket\n");

    bzero((char *) &serv_addr, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (connect(sock_desc, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
        printf("Failed to connect to server\n");
        return -1;
    }
    new_sock = malloc(1);
    *new_sock = sock_desc;
    if( pthread_create( &sniffer_thread , NULL ,  listener , (void*)new_sock ) < 0)
        {
            perror("could not create thread");
            return 1;
        }
    printf("Connected successfully - Enter username\n");
    if(fgets(sbuff, MAX_SIZE , stdin)!=NULL)
    {
      send(sock_desc,sbuff,strlen(sbuff),0);

                
        strncpy(clientUserName,sbuff,(strlen(sbuff) - 1));


       bzero(rbuff,MAX_SIZE);//to clean buffer otherwise previous word characters also came
    }
    bzero(sbuff,MAX_SIZE);

    while(1){

      printf("ENTER COMMAND:\n");
     if(fgets(sbuff, MAX_SIZE , stdin)!=NULL)
    {  
       if (strstr(sbuff,"Broadcast")) {
          int n = send(sock_desc,sbuff,strlen(sbuff),0);
        //  printf(" value %d\n", n );
          printf("ENTER MESSAGE:\n");
          if(fgets(sbuff, MAX_SIZE , stdin)!=NULL)
          { 
            send(sock_desc,sbuff,strlen(sbuff),0);
          }

       }

       if (strstr(sbuff,"Private")) {
          int n = send(sock_desc,sbuff,strlen(sbuff),0);
          //printf(" value %d\n", n );
          printf("ENTER CLIENT NAME:\n");
          if(fgets(sbuff, MAX_SIZE , stdin)!=NULL)
          { 
            send(sock_desc,sbuff,strlen(sbuff),0);
          }
          bzero(sbuff,MAX_SIZE);
          printf("ENTER MESSAGE\n");
          if(fgets(sbuff, MAX_SIZE , stdin)!=NULL)
          { 
            send(sock_desc,sbuff,strlen(sbuff),0);
          }

       }

       if (strstr(sbuff,"Blockcast")) {
          int n = send(sock_desc,sbuff,strlen(sbuff),0);
          //printf(" value %d\n", n );
          printf("ENTER CLIENT NAME:\n");
          if(fgets(sbuff, MAX_SIZE , stdin)!=NULL)
          { 
            send(sock_desc,sbuff,strlen(sbuff),0);
          }
          bzero(sbuff,MAX_SIZE);
          printf("ENTER MESSAGE\n");
          if(fgets(sbuff, MAX_SIZE , stdin)!=NULL)
          { 
            send(sock_desc,sbuff,strlen(sbuff),0);
          }

       }

        if (strstr(sbuff,"FileBroad")) {
          int n = send(sock_desc,sbuff,strlen(sbuff),0);
          //printf(" value %d\n", n );
          printf("ENTER FILE PATH:\n");
          scanf("%s", filepath);
        //      printf("Path = %s\n", filepath);
          bzero(sbuff,MAX_SIZE);
          file_to_server(sock_desc, filepath);
       }

       if (strstr(sbuff,"FileUni")) {
          int n = send(sock_desc,sbuff,strlen(sbuff),0);
          //printf(" value %d\n", n );
          printf("ENTER CLIENT NAME:\n");
          if(fgets(sbuff, MAX_SIZE , stdin)!=NULL)
          { 
            send(sock_desc,sbuff,strlen(sbuff),0);
          }
        //      printf("Path = %s\n", filepath);
          bzero(sbuff,MAX_SIZE);
          printf("ENTER FILE PATH:\n");
          scanf("%s", filepath);
          file_to_server(sock_desc, filepath);
       }

       if (strstr(sbuff,"FileBlock")) {
          int n = send(sock_desc,sbuff,strlen(sbuff),0);
          //printf(" value %d\n", n );
          printf("ENTER CLIENT NAME:\n");
          if(fgets(sbuff, MAX_SIZE , stdin)!=NULL)
          { 
            send(sock_desc,sbuff,strlen(sbuff),0);
          }
        //      printf("Path = %s\n", filepath);
          bzero(sbuff,MAX_SIZE);
          printf("ENTER FILE PATH:\n");
          scanf("%s", filepath);
          file_to_server(sock_desc, filepath);
       }

       bzero(sbuff,MAX_SIZE);
       //printf("exiting filebroad\n");
    }
    sleep(1);  
    }
    
        //close(sock_desc);
    return 0;

}


void *listener(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;

    
    int n;
    char    sendBuff[100], client_message[2000];

      while(1)
      { 
          
        bzero(client_message,2000); 
        n = recv(sock,client_message,2,0);
        //printf("Flag = %s\n", client_message);
          

        if(strcmp(client_message, "F ") == 0)
        {
          //printf("Receiving file\n");
          bzero(client_message,2000); 
          store_file(sock);
        }
        else
        {
          //printf("MESSAGE RECEIVED\n");
          bzero(client_message,2000); 
          n = recv(sock,client_message,2000,0);
          if ( n > 0)
          {
            printf("%s\n", client_message );
            bzero(client_message,2000);  
          }
        }
     
        sleep(0.5);
      }
    
    return 0;
}


void file_to_server(int sock_desc, char * path)
{
  char sbuff[MAX_SIZE];
  char * temp,*filename;
  char new_filename[30], file_path[30];
  char file_flag[10] = "F ";
  int n;
  struct stat st; 
  int size = 0;

  strcpy(file_path, path);
  printf("file path= %s\n", file_path);
  FILE * fd;
  int j;
  bzero(sbuff, MAX_SIZE); 
  if((temp = strstr(path, "/")) == NULL)
  {
    filename = path;
  }
  else
  {
    while(temp = strstr(path, "/"))
    {
      temp++;
      filename = temp;
      path = filename;
    }
  }
 //printf("Filename!!! = %s\n", filename);

  sprintf(new_filename, "%s ", filename);
  //sprintf(sbuff,file_flag);
 // n =send(sock_desc, sbuff, strlen(sbuff), 0); //send "F "
 // if(n==0)
 //     {
   //     puts("Client  Disconnected\n");
     // }
  bzero(sbuff, MAX_SIZE);
  strcpy(sbuff,filename); 
  n =send(sock_desc, sbuff, strlen(sbuff), 0); //send Filename
  if(n<0)
      {
        puts("Client  Disconnected\n");
      }

  int sent_count; /* how many sending chunks, for debugging */
  ssize_t read_bytes, /* bytes read from local file */
  sent_bytes, /* bytes sent to connected socket */
  sent_file_size;


  sent_count = 0;
  sent_file_size = 0;


  fd = fopen(file_path, "r");
    if (fd == NULL)
    {
      perror("Invalid path. Try again\n");
      exit(0);
    }


    if (stat(file_path, &st) == 0)
         size = st.st_size;

  bzero(sbuff, MAX_SIZE);
  sprintf(sbuff,"%d",size); 
  sleep(1);
 // printf("File size is %s\n", sbuff );
  n =send(sock_desc, sbuff, strlen(sbuff), 0); //Send filesize
  if(n<0)
      {
        puts("Client  Disconnected\n");
      }
  bzero(sbuff, MAX_SIZE);
  
  while ( !feof(fd))
  {
   read_bytes = fread( sbuff, 1, MAX_SIZE,  fd);
   //bzero(sbuff, MAX_SIZE); 
   if( (sent_bytes = send(sock_desc, sbuff, read_bytes, 0)) < read_bytes )
      {
        perror("send error");
        return;
      }else{
        //printf("Sent message: %s\n", sbuff);
        //printf("Sending to file info to server\n");
      }
  sent_count++;
  sent_file_size += sent_bytes;
  }

 fclose(fd);

 return;
}

void store_file(int sock)
{
  //sleep(2);

        int i = 0;
        int x;
        int size = 0;
        char * a = " ";
        int n;
        char   client_message[2000], filename[30], filesize[30];
        FILE * fd;
        char *c;
        int file;

          bzero(client_message,2000); 
          n = recv(sock,client_message,2000,0);
          if ( n > 0)
          {
            //printf("RECIEVED A MSG here: %s\n", client_message );
          }
          strcpy(filename,client_message);
          
          bzero(client_message,2000);  

          n = recv(sock,client_message,2000,0);
          if ( n > 0)
          {
            //printf("RECIEVED A MSG here2: %s\n", client_message );
          }
          strcpy(filesize,client_message);
          file = atoi(filesize);
          bzero(client_message,2000);  



        /*Create the directory for the client */
/*        char dir[100];
        char fname[100];
        struct stat st = {0};
        sprintf(dir,"./clients/%s",filename);
        if (stat(dir, &st) == -1) {
        mkdir(dir, 0700);
        }*/
        
       // sprintf(fname,"%s/%s",dir,filename);

        char fdir[100]; 
        sprintf(fdir,"./clients/%s/%s",clientUserName,filename);
        //printf(" path = %s\n", fdir);
        fd = fopen(fdir, "w");
        if (fd == NULL)
        {
            printf("Error opening file\n");    
            return;
        }
        while((n=recv(sock,client_message,2000,0))>0 && (size <= file))
        {
            size+=n;
            //printf("Cumulative size = %d\n", size);
            //printf("SIZE = %d\n", size);
            //printf("client_message = %s\n", client_message);
         //   if (size > file)
           // {
             // printf("size = %d, filesize = %d\n", size, file);
           //   n = file - size + n;
           // }
            x = fwrite(client_message, 1, n, fd);
            //printf("%d bytes Wrote to file\n", x);
            bzero(client_message, 2000);
            if (size >= file)
                break;
        }
        printf("File received\n");
        fclose(fd);
}