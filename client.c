#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>

#define NO 2654435769
#define MAXPRIME  2147483647

const char *id_a = "asdfg";
const char *id_b = "qwert";
const char *key_a = "zxcvb";

char msg[200], cypher[200], session_key[20];
int msg_len;

/* Function to print message */
void print_msg(char *str)
{
	while(*str) {
		printf("%d",*str++);
	}
	printf("\n");
}

/* Function to generate Random number */

unsigned int prng()
{
	unsigned int i = 2047;
        unsigned long  t;
        static unsigned int next,support=362436;

        if(!next) {
        	FILE *fp = fopen("seed1","r+");
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

/* */
void encrypt(char *msg, const char *key)
{
	int i,msg_len = strlen(msg),key_len = strlen(key);
	
	for(i = 0; i < msg_len; i++) {
		msg[i] = msg[i] ^ key[i % (key_len)];
	}
}

void get_key()
{
	int nonce_size, i, sockfd, clilen, port;
	char nonce[20], ip[16];
	struct sockaddr_in cli_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		printf("ERROR opening socket");
		exit(1);
	}

        fprintf(stdout,"################################################\n");
    	fprintf(stdout,"                 Enter IP address of KDC <-:");
    	fscanf(stdin,"%s",ip);
    	fprintf(stdout,"                   Enter port number of KDC <-:");
    	fscanf(stdin,"%d",&port);
	cli_addr.sin_family = AF_INET;
	cli_addr.sin_addr.s_addr = inet_addr(ip);
	cli_addr.sin_port = htons(port);
	clilen = sizeof(cli_addr);
	
	if (connect(sockfd, (struct sockaddr *)&cli_addr,clilen) < 0) {
		printf("error in connect\n");
		exit(0);
	}

	fprintf(stdout,"\n--------------STEP 1 in KEY DISTRIBUTION------------------\n\n");
        strncpy(msg,id_a,5);
        printf("ID A :- ");
        print_msg(msg);
        strncat(msg,id_b,5);
        printf("ID B :- ");
        print_msg(msg+5);
	nonce_size = prng()%10+5;

	for(i = 0; i < nonce_size; i++) {
		nonce[i]=prng()%25+65;
        }

	nonce[i]='\0';
	printf("NONCE1 :- ");
	print_msg(nonce);
	strncat(msg,nonce,nonce_size);
	printf("Message send to kdc :- ");
	print_msg(msg);
	write(sockfd,msg,200);
	msg_len=strlen(msg);
	read(sockfd,cypher,200);
	fprintf(stdout,"\n\n--------------STEP 2 in KEY DISTRIBUTION------------------\n\n");
	close(sockfd);  
}
int main()
{
	int sockfd, clilen,port;
    	char ip[16];
    	struct sockaddr_in cli_addr;
    		// int socket(int domain, int type, int protocol);
    		/* domain ->protocol family which will be used .. AF_INET for ipv4 protocols
    		 *type -> provide communication symantics  SOCK_STREAM ->tcp ,SOCK_DGRM ->udp 
    		 *protocol   -> 0 for tcp
    		 *
    		 */
    	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("ERROR opening socket");
		exit(1);
	}
    					
	fprintf(stdout,"################################################\n");
    	fprintf(stdout,"                 Enter IP address of server <-:");
    	fscanf(stdin,"%s",ip);
    	fprintf(stdout,"                   Enter port number of server <-:");
    	fscanf(stdin,"%d",&port);
    	cli_addr.sin_family = AF_INET;
    	cli_addr.sin_addr.s_addr = inet_addr(ip);
    	cli_addr.sin_port = htons(port);
    	clilen = sizeof(cli_addr);
                  /* int connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
                   * The  connect()  system call connects the socket referred to by the file
       descriptor sockfd to the address specified by addr.  The addrlen  argu‐
       ment  specifies the size of addr.  The format of the address in addr is
       determined by the address space of the socket sockfd; see socket(2) for
       further details.

       If  the socket sockfd is of type SOCK_DGRAM then addr is the address to
       which datagrams are sent by default, and the only  address  from  which
       datagrams  are  received.   If  the  socket  is  of type SOCK_STREAM or
       SOCK_SEQPACKET, this call attempts to make a connection to  the  socket
       that is bound to the address specified by addr. return 0 on success -1 on failure.
                   */

	if(connect(sockfd,(struct sockaddr *)&cli_addr,clilen) < 0 )  {
		printf("error in connect\n");
		exit(0);
	}

	fprintf(stdout,"\n                                Starting client\n");
	fprintf(stdout,"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
	fprintf(stdout,"                Starting key distribution process\n");
	get_key();
	printf("cypher from kdc :- ");
    	print_msg(cypher);
    	*msg='\0';
    	strncat(msg,cypher+msg_len+15,20);
    	*(cypher+msg_len+15)='\0';
    	encrypt(cypher,key_a);
    	printf("client part message :- ");
    	print_msg(cypher);
    	*session_key='\0';
    	strncat(session_key,cypher+msg_len,15);
    	printf("session key :- ");
    	print_msg(session_key);
	fprintf(stdout,"\n\n--------------STEP 3 in KEY DISTRIBUTION-------------------\n\n");
	printf("cypher from KDC send to server :-");
	print_msg(msg);
	write(sockfd,msg,200);
    	fprintf(stdout,"\n\n--------------STEP 4 in KEY DISTRIBUTION-------------------\n\n");
    	read(sockfd,cypher,200);
    	encrypt(cypher,session_key);
    	printf("Nonce2 from server :- ");
	print_msg(cypher);
	fprintf(stdout,"\n\n--------------STEP 5 in KEY DISTRIBUTION-------------------\n\n");
	printf("Modified nonce2 send back to server :- ");
	encrypt(cypher,"SANJIV");
	strcpy(msg,cypher);
	print_msg(msg);
	write(sockfd,msg,200);
	read(sockfd,cypher,200);
	fprintf(stdout,"%s\n",cypher);
    	fprintf(stdout,"\n\n--------------KEY DISTRIBUTION COMPLETED-------------------\n\n");
    
	while( 1 ) {

		fprintf(stdout,"\nYou <-:");
		fgets(msg,200,stdin);
	    	strcpy(cypher,msg);
	    	encrypt(cypher,session_key);
	    	strcpy(msg,cypher);
	    	write(sockfd,msg,200);
	    	read(sockfd,cypher,200);
	    	encrypt(cypher,session_key);
	    	fprintf(stdout,"Peer:->%s\n",cypher);
	}
	close(sockfd);
        
}
