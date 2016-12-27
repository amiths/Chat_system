// socket server example, handles multiple clients using threads

#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread
#include <stddef.h>


typedef struct node {
  int client_id;
  char username[256];
  struct node *next;
}node_t;


node_t* head = NULL;
void add( int id, char cname[256] )
{
    struct node *temp;
    temp=(struct node *)malloc(sizeof(struct node));
    temp->client_id=id;
    strcpy(temp->username,cname);
    if (head== NULL)
    {
    head=temp;
    head->next=NULL;
    }
    else
    {
    temp->next=head;
    head=temp;
    }
}

node_t* getHead(){
    return head;
}

int delete(int id)
{
    struct node *temp, *prev;
    temp=head;
    while(temp!=NULL)
    {
    if(temp->client_id==id)
    {
        if(temp==head)
        {
        head=temp->next;
        free(temp);
        return 1;
        }
        else
        {
        prev->next=temp->next;
        free(temp);
        return 1;
        }
    }
    else
    {
        prev=temp;
        temp= temp->next;
    }
    }
    return 0;
}

void  display()
{
    struct node* r=head;
    if(r==NULL)
    {
    return;
    }
    while(r!=NULL)
    {
    printf("client id = %d , :: ",r->client_id);
    printf("username = %s\n",r->username);
    r=r->next;
    }
    printf("\n");
}


char* getUserName(int sock_id)
{
    struct node* r=head;
    int n;
    char send_buff[2000];

    if(r==NULL)
    {
    return NULL;
    }
    //printf("before while\n");
    while(r!=NULL)
    {
        if(r->client_id == sock_id)
            return r->username;
        r=r->next;
    }
}

void  broadcast(int sock_id,char buff[1000])
{
    int n;
    struct node* r=head;
    char send_buff[2000];
    if(r==NULL)
    {
    return;
    }
    while(r!=NULL)
    {   
        if ((r->client_id == sock_id))
        {
            r=r->next;
            continue;
        }
    
        sprintf(send_buff,"M ");
        if( (n = send(r->client_id, send_buff, strlen(send_buff), 0)) < 0 ) {
        perror("send error");
        }
          sleep(1);
    printf(" %s is broadcasting a message\n", getUserName(sock_id) );
    sprintf(send_buff,"Message from %s%s", getUserName(sock_id), buff);
    int n =send(r->client_id,send_buff,strlen(send_buff),0);
    r=r->next;
    }
}




void  blockcast(int sock_id, char uname[1000], char message[1000])
{
    struct node* r=head;
    int n;
    char send_buff[2000];

    if(r==NULL)
    {
    return;
    }
    while(r!=NULL)
    {
        if (strstr(r->username,uname))
        {
            r=r->next;
            continue;
        }
        if ((r->client_id == sock_id))
        {
            r=r->next;
            continue;
        }

        sprintf(send_buff,"M ");
        if( (n = send(r->client_id, send_buff, strlen(send_buff), 0)) < 0 ) {
        perror("send error");
        }
        sleep(1);
        printf(" %s is blockcasting a message\n", getUserName(sock_id) );
        sprintf(send_buff,"Message from %s%s", getUserName(sock_id), message);
        n = send(r->client_id,send_buff,strlen(send_buff),0);
        r=r->next;
    }
  //  printf("exiting block func\n");
}

void  private(int sock_id, char uname[1000], char message[1000])
{
    struct node* r=head;
    int n;
    char send_buff[2000];
    int id;
    if(r==NULL)
    {
        return;
    }
    while(r!=NULL)
    {
    if (strstr(r->username,uname)){
        id = r->client_id;
        sprintf(send_buff,"M ");
        if( (n = send(r->client_id, send_buff, strlen(send_buff), 0)) < 0 ) {
        perror("send error");
        }
        sleep(1);
    printf(" %s is sending a message to %s\n", getUserName(sock_id), getUserName(id) );
    sprintf(send_buff,"Message from %s%s", getUserName(sock_id), message);
    n = send(r->client_id,send_buff,strlen(send_buff),0);
    }
    r=r->next;
    }

}


//the thread function
void *connection_handler(void *);
void file_broadcast(int sock, char fname[100], char fsize[100]);
void file_unicast(int sock, char uname[100], char fname[100], char fsize[100]);
void file_blockcast(int sock, char uname[100], char fname[100], char fsize[100]);


int main(int argc , char *argv[])
{
    if (argc < 2)
    {
      printf("Error: Enter Port Number\n");
      exit(0);
    }
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
   // puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( atoi(argv[1]) );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    //puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);


    /*Create a directory for clients*/
    if (mkdir("./clients",0777) == -1) {

    }

    c=sizeof(struct sockaddr_in);
       while(client_sock = accept(socket_desc,(struct sockaddr*)&client,(socklen_t*)&c))
       {
       // puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
       // printf("-----%d-------\n", client_sock );
        //add(client_sock);
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }

       // puts("Handler assigned");
       // display();
    }

    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    return 0;
}
/*
  This will handle connection for each client
  */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    char c[10];    //delete  this
    int sock = *(int*)socket_desc;
    int n;
    char    sendBuff[100], client_message[2000];

      while((n=recv(sock,client_message,2000,0))>0)
      {

        add(sock,client_message);
        printf("Connected with %s\n", client_message);
        char dir[100];
        char fdir[100];
        strncpy(dir,client_message,(strlen(client_message) - 1));
        sprintf(fdir,"./clients/%s",dir);
        if (mkdir(fdir,0777) == -1) {
        
        }
        break;

      }

       while((n=recv(sock,client_message,2000,0))>0)
      {
        
        if (strstr(client_message,"Broadcast")){
            bzero(client_message,2000);
            while((n=recv(sock,client_message,2000,0))>0)
            {
        
                broadcast(sock,client_message);
                break;
            }


        }

        if (strstr(client_message,"Private")){
            bzero(client_message,2000);
            while((n=recv(sock,client_message,2000,0))>0)
            {
                char uname[1000];
                strcpy(uname,client_message);
                bzero(client_message,2000);
                while((n=recv(sock,client_message,2000,0))>0)
                {
        
                    private(sock,uname,client_message);
                    break;
                }
                //printf("Private broken\n");   
                break;
            }


        }

        if (strstr(client_message,"Blockcast")){
            bzero(client_message,2000);
            while((n=recv(sock,client_message,2000,0))>0)
            {
                char uname[1000];
                strcpy(uname,client_message);
                bzero(client_message,2000);
                while((n=recv(sock,client_message,2000,0))>0)
                {
                    blockcast(sock,uname,client_message);
                    break;
                }
                break;
            }


        }

        if (strstr(client_message,"FileBroad")){
        
        //    printf("Call ftoc\n");
            char filename[100];
            char filesize[100];
   
            bzero(client_message,2000);
            while((n=recv(sock,client_message,2000,0))>0)
            {
                strcpy(filename,client_message);
                //printf("--------FILE NAME IS --------------%s\n", filename );
                break;
            }
            //Recieve File size

            bzero(client_message,2000);
            while((n=recv(sock,client_message,2000,0))>0)
            {
                strcpy(filesize,client_message);
                //printf("--------FILE SIZE IS --------------%s\n", filesize );
                break;
            }
            file_broadcast( sock, filename,filesize);
        
        }

         if (strstr(client_message,"FileUni")){
        
            //printf("Call ftoc\n");
            char filename[100];
            char filesize[100];
            char uname[1000];

   
            bzero(client_message,2000);
            sleep(1);
            while((n=recv(sock,client_message,2000,0))>0)   // Receive private client name
            {
                strcpy(uname,client_message);
              //  printf("wats this? = %s\n", client_message);
                bzero(client_message,2000);
                break;
             }   
            while((n=recv(sock,client_message,2000,0))>0)
            {
                strcpy(filename,client_message);
               // printf("--------FILE NAME IS --------------%s\n", filename );
                break;
            }
            //Recieve File size

            bzero(client_message,2000);
            while((n=recv(sock,client_message,2000,0))>0)
            {
                strcpy(filesize,client_message);
                //printf("--------FILE SIZE IS --------------%s\n", filesize );
                break;
            }
            file_unicast(sock,uname,filename,filesize);
        }
        
        if (strstr(client_message,"FileBlock")){
        
           // printf("Call ftoc\n");
            char filename[100];
            char filesize[100];
            char uname[1000];

   
            bzero(client_message,2000);
            sleep(1);
            while((n=recv(sock,client_message,2000,0))>0)   // Receive private client name
            {
                strcpy(uname,client_message);
              //  printf("wats this? = %s\n", client_message);
                bzero(client_message,2000);
                break;
             }   
            while((n=recv(sock,client_message,2000,0))>0)
            {
                strcpy(filename,client_message);
                //printf("--------FILE NAME IS --------------%s\n", filename );
                break;
            }
            //Recieve File size

            bzero(client_message,2000);
            while((n=recv(sock,client_message,2000,0))>0)
            {
                strcpy(filesize,client_message);
                //printf("--------FILE SIZE IS --------------%s\n", filesize );
                break;
            }
            file_blockcast(sock,uname,filename,filesize);
        }

      }
      
      close(sock);

    if(n==0)
      { 

        printf("Disconnected : %s\n", getUserName(sock));
        delete(sock);

      }
      else
      {
        perror("recv failed");
      }

    return 0;
}


void file_broadcast(int sock, char fname[100], char fsize[100])
{
    printf("Sending file...\n");
    int n, m;
    int size = 0;
    int i=1;
    char client_message[2000];
    char send_buff[2000];
    int file_size = atoi(fsize);
    struct node* r = head;
    if(r==NULL)
    {
        return;
    }

    //Send file flags
    sprintf(send_buff,"F ");
    while ((r!=NULL)) {
     //   printf("Sending fileflag\n");
        if ((r->client_id == sock))
        {
            r=r->next;
            continue;
        }
        if( (n = send(r->client_id, send_buff, strlen(send_buff), 0)) < 0 ) {
        perror("send error");
        return;
        }
        r=r->next;
        
    }
    sleep(1);

    r = head;
    bzero(send_buff, 2000);    
    sprintf(send_buff,"%s", fname);
    printf("send buff in server = %s\n", send_buff);

    while ((r!=NULL)) {
     //   printf("Sending filenames\n");
        if ((r->client_id == sock))
        {
            r=r->next;
            continue;
        }
        if( (n = send(r->client_id, send_buff, strlen(send_buff), 0)) < 0 ) {
        perror("send error");
        return;
        }
        r=r->next;
        
    }
    sleep(1);


    r = head;
    bzero(send_buff, 2000);   
    sprintf(send_buff,"%d", file_size);
    while ((r!=NULL)) {

     //   printf("Sending filesizes\n");
        if ((r->client_id == sock))
        {
            r=r->next;
            continue;
        }
        if( (n = send(r->client_id, send_buff, strlen(send_buff), 0)) < 0 ) {
            perror("send error");
            return;
        }
        r=r->next;
        
    } 
    sleep(1);
    
    // File contents being sent
    r = head;
    size = 0;
    int m2 = 0;
  //  printf("size is %d\n", file_size );
    bzero(client_message, 2000); 
  // printf("%s\n", ); 
      while((n=recv(sock,client_message,2000,0))>0 )  
      { 
            //printf("\n Received from client %d\n", n );
            //printf("Message: %s\n", client_message);

            size+=n;
            
            while ((r!=NULL)) {
                if ((r->client_id == sock))
                {
                    r=r->next;
                    continue;
                }
                m =send(r->client_id,client_message,n,0);
                m2 = m2 + m;
            //    printf("\nSent bytes from server = %d\n", m);
            //    printf("\nCumulative sent = %d\n", m2);

                r=r->next;

            }
            r = head;
            

            bzero(client_message, 2000); 
            if (size >= file_size)
                break; 
        }
        printf("Done sending\n"); 

}

void file_unicast(int sock, char uname[100], char fname[100], char fsize[100])
{
    printf("Sending file...\n");
    int file_size = atoi(fsize);
    int n, m;
    int size = 0;
    int i=1;
    char client_message[2000];
    char send_buff[2000];
    struct node* r = head;
    if(r==NULL)
    {
        return;
    }

    //Send file flags
    sprintf(send_buff,"F ");
    while ((r!=NULL)) {
        if (strstr(r->username,uname)){
    //    printf("Sending fileflag\n");
        if( (n = send(r->client_id, send_buff, strlen(send_buff), 0)) < 0 ) {
        perror("send error");
        return;
        }
    }
        r=r->next;
        
        
    }
    sleep(1);

    r = head;
    bzero(send_buff, 2000);    
    sprintf(send_buff,"%s", fname);
  //  printf("send buff in server = %s\n", send_buff);

    while ((r!=NULL)) {
        if (strstr(r->username,uname)){
      //  printf("Sending filenames\n");
        if( (n = send(r->client_id, send_buff, strlen(send_buff), 0)) < 0 ) {
        perror("send error");
        return;
        }
    }
        r=r->next;
        
    }
        sleep(1);


    r = head;
    bzero(send_buff, 2000);    
    sprintf(send_buff,"%d", file_size);
    while ((r!=NULL)) {
        if (strstr(r->username,uname)){

       // printf("Sending filesizes\n");
        if( (n = send(r->client_id, send_buff, strlen(send_buff), 0)) < 0 ) {
            perror("send error");
            return;
        }
    }
        r=r->next;
        
    }
        sleep(1);

    // File contents being sent
    r = head;
    size = 0;
    int m2 = 0;
   // printf("size is %d\n", file_size );
    bzero(client_message, 2000); 
  // printf("%s\n", ); 
      while((n=recv(sock,client_message,2000,0))>0 )   
        {   

             size+=n;
             
            while ((r!=NULL)) {
                if (strstr(r->username,uname)){
                m =send(r->client_id,client_message,n,0);
                m2 = m2 + m;

                }
                r=r->next;
            }
            r = head;           

            bzero(client_message, 2000); 
            if (size >= file_size)
                break; 
        }
        printf("Done sending\n");

}

void file_blockcast(int sock, char uname[100], char fname[100], char fsize[100])
{
    printf("Sending file\n");
    int file_size = atoi(fsize);
    int n, m;
    int size = 0;
    int i=1;
    char client_message[2000];
    char send_buff[2000];
    struct node* r = head;
    if(r==NULL)
    {
        return;
    }

    //Send file flags
    sprintf(send_buff,"F ");
    while ((r!=NULL)) {
        if (strstr(r->username,uname))
        {
            r=r->next;
            continue;
        }
        if ((r->client_id == sock))
        {
            r=r->next;
            continue;
        }
    //    printf("Sending fileflag\n");
        if( (n = send(r->client_id, send_buff, strlen(send_buff), 0)) < 0 ) {
        perror("send error");
        return;
        }
    
        r=r->next;
        
    }
            sleep(1);


    r = head;
    bzero(send_buff, 2000);    
    sprintf(send_buff,"%s", fname);
  //  printf("send buff in server = %s\n", send_buff);

    while ((r!=NULL)) {
        if (strstr(r->username,uname))
        {
            r=r->next;
            continue;
        }
        if ((r->client_id == sock))
        {
            r=r->next;
            continue;
        }
      //  printf("Sending filenames\n");
        if( (n = send(r->client_id, send_buff, strlen(send_buff), 0)) < 0 ) {
        perror("send error");
        return;
        }
    
        r=r->next;
        
    }
            sleep(1);



    r = head;
    bzero(send_buff, 2000);    
    sprintf(send_buff,"%d", file_size);
    while ((r!=NULL)) {
        if (strstr(r->username,uname))
        {
            r=r->next;
            continue;
        }
        if ((r->client_id == sock))
        {
            r=r->next;
            continue;
        }
       // printf("Sending filesizes\n");
        if( (n = send(r->client_id, send_buff, strlen(send_buff), 0)) < 0 ) {
            perror("send error");
            return;
        }
    
        r=r->next;
        
    }
            sleep(1);

    // File contents being sent
    r = head;
    size = 0;
    int m2 = 0;
   // printf("size is %d\n", file_size );
    bzero(client_message, 2000); 
  // printf("%s\n", ); 
      while((n=recv(sock,client_message,2000,0))>0 )   
        {   

             size+=n;
             
            while ((r!=NULL)) {
                if (strstr(r->username,uname))
                {
                    r=r->next;
                    continue;
                }
                if ((r->client_id == sock))
                {
                    r=r->next;
                    continue;
                }
                m =send(r->client_id,client_message,n,0);
                m2 = m2 + m;
                r=r->next;
            }
            r = head;           

            bzero(client_message, 2000); 
            if (size >= file_size)
                break; 
        }
        printf("Done sending\n");
}


