#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define NO 2654435769
#define MAXPRIME  2147483647

const char *id_a = "asdfg";
const char *id_b = "qwert";
const char *key_b = "yuiop";
char msg[200], cypher[200], session_key[20];
int msg_len;

void print_msg(char *str)
{
	while(*str) {
		printf("%d",*str++);
	}

	printf("\n");
}

unsigned int prng()
{
        unsigned int i = 2047;
        unsigned long  t;
        static unsigned int next,support=362436;

        if( !next ) {

		FILE *fp=fopen("seed1","r+");
        	fscanf(fp,"%d",&next);
        	rewind(fp);
        	fprintf(fp,"%d",next+1);
        	fclose(fp);
        }

        i = (i + 1) & 2047;
        t = (12345* next) + support;
        support = t >> 32;
        next = MAXPRIME - t;
        return next;
}

void encrypt(char *msg,const char *key)
	{
	int i,msg_len=strlen(msg),key_len=strlen(key);
	for(i=0;i<msg_len;i++)
	{
	msg[i]=msg[i]^key[i%(key_len)];
	}
	}
void child (int sock)
{	int nonce_size,i;
	char session_key[20],nonce[20];
	nonce_size=prng()%10+5;
		for(i=0;i<nonce_size;i++)
		nonce[i]=prng()%25+97;
		nonce[i]='\0';
	read(sock,msg,200);
	printf("\n\n-------------Cypher from KDC via client----------- \n\n ");
    	print_msg(msg);
	encrypt(msg,key_b);
	strncpy(session_key,msg+5,15);
	printf("Session key  :- ");
    	print_msg(session_key);
    	printf("Nonce2  :- ");
    	print_msg(nonce);
    	encrypt(nonce,session_key);
    	printf("\n\n-------------Cypher sent back to client-------------\n\n :- ");
    	print_msg(nonce);
    	*cypher='\0';
	strncat(cypher,nonce,strlen(nonce));
	write(sock,cypher,200);
	read(sock,msg,200);
	encrypt(msg,"SANJIV");
	printf("\n\n-------------Nonce2 varified---------------------------\n\n:- ");
	print_msg(msg);
	strcpy(cypher,"confirmed continue messaging");
	write(sock,cypher,200);
	
while(1)
	{
	read(sock,msg,200);
	encrypt(msg,session_key);
	fprintf(stdout,"Peer :->%s\n",msg);
	fprintf(stdout,"\nYou <-:");
	fgets(cypher,200,stdin);
	encrypt(cypher,session_key);
	write(sock,cypher,200);
	}
}
int main()
{
    int sockfd, newsockfd,clilen,port;
    char buffer[256];
    pid_t pid;
    struct sockaddr_in serv_addr, cli_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        printf("ERROR opening socket");
        exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    printf(":::::::::::::::::::::::::::::::::::::::::::::\n");
    printf("			Enter port number <-:");
    scanf("%d",&port);
    printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    printf("                SERVER running at port no :-> %d\n",port);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
 
    /* Now bind the host address using bind() call.
       int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
       
       When  a  socket  is  created  with socket(2), it exists in a name space.
       (address family) but has no address assigned to it.  bind() assigns the
       address  specified  by  addr  to  the  socket  referred  to by the file
       descriptor sockfd.  addrlen  specifies  the  size,  in  bytes,  of  the
       address structure pointed to by addr.  Traditionally, this operation is
       called “assigning a name to a socket”.
    
       0 on success -1 on failure. bind() and connect() are very much similar .
    */
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
                          sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(1);
    }
    /*
       int listen(int sockfd, int backlog);

       listen()  marks  the  socket referred to by sockfd as a passive socket,
       that is, as a socket that will be used to  accept  incoming  connection
       requests using accept(2).

       The  sockfd  argument  is  a file descriptor that refers to a socket of
       type SOCK_STREAM or SOCK_SEQPACKET.

       The backlog argument defines the maximum length to which the  queue  of
       pending  connections  for  sockfd  may  grow.   If a connection request
       arrives when the queue is full, the client may receive an error with an
       indication  of  ECONNREFUSED  or,  if  the underlying protocol supports
       retransmission, the request may be ignored so that a later reattempt at
       connection succeeds.
     */
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    while (1) 
    {   
       /*So, as was mentioned earlier... the listen() API tells the system that
         we're willing to accept connections. When someone connects, the system 
         puts them in a queue. The accept() API takes the connections off    of
         the queue in First-In-First-Out (FIFO) order.

	 Each time you accept a new connection, the IP address and port number of
	 the connecting client is placed in a sockaddr_in structure that is pointed 
	 to by the 'address' parameter.
         The 'address_len' parameter has two purposes. On input to the accept() API,
         it contains the amount of memory that you've allocated to the 'address' 
         parameter. Accept() uses this to ensure that it doesn't write data beyond what 
         you've allocated. On output from the accept() API, it contains the actual number
         of bytes that the accept() API wrote into the area of memory that you supplied 
         as 'address'.
         It's important to understand that accept() creates a new socket. The original 
         socket which you used with bind() and listen() will continue to listen for new 
         connections and put them into the backlog queue.

         The new socket created by accept() contains your TCP connection to the client 
         program. Once you've accept()-ed it, you can use the send() and recv() APIs to
         hold a conversation with the client program.

	 When you're done talking to the client, you'll want to call close() for the descriptor
	 returned by accept(). Again, remember that this is a separate socket from the one that's
	 listen()-ing. So, when you close() the socket that was returned from accept(), your 
	 program will still be listening for more connections.
         To stop listening for connections, you need to call close() for the original socket.*/
        newsockfd = accept(sockfd, 
                (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            exit(1);
        }
        /* Create child process */
        pid = fork();
        if (pid < 0)
        {
            perror("ERROR on fork");
	    exit(1);
        }
        if (pid == 0)  
        {
            /* This is the client process */
            close(sockfd);
            child(newsockfd);
            exit(0);
        }
        else
        {
            close(newsockfd);
        }
    } 
}
