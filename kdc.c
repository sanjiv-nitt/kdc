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
const char *id_a ="asdfg";
const char *id_b ="qwert";
const char *key_a="zxcvb";
const char *key_b="yuiop";
void print_msg(char *str)
{
printf("\n");
while(*str)
printf("%d",*str++);
printf("\n\n");
}
void encrypt(char *msg,const char *key)
	{
	int i,msg_len=strlen(msg),key_len=strlen(key);
	printf("msg len == %d   %d",msg_len,key_len);
	for(i=0;i<msg_len;i++)
	{
	msg[i]=msg[i]^key[i%(key_len)];
	}
	}
unsigned int prng(void) {
        unsigned int i = 2047;
        unsigned long  t;
        static unsigned int next,support=362436;
        if(!next)
        	{
        	FILE *fp=fopen("seed","r+");
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
void child (int sock)
{ 
	char msg[200],cypher[200],session_key[30],sub[30];
	int msg_len,key_size; 
		read(sock,msg,200);
		printf("msg received from client :- ");
		print_msg(msg);
				for(key_size=0;key_size<15;key_size++)
					session_key[key_size]=prng()%25+65;
						session_key[key_size]='\0';
		printf("session key :- ");
		print_msg(session_key);
		*sub='\0';
		strncat(sub,msg,5);
		print_msg(sub);
			strncat(msg,session_key,15);
			printf("msg for client :- ");
			print_msg(msg);
				strncat(sub,session_key,15);
			printf("msg for server :- ");
			print_msg(sub);
					 encrypt(msg,key_a);
			printf("cypher for client :- ");
			print_msg(msg);
					 encrypt(sub,key_b);
			printf("cypher for server :- ");
			print_msg(sub);
			    *cypher='\0';
			    strncat(cypher,msg,strlen(msg)+1);
		strncat(cypher,sub,20);
		printf("msg sent back to client :- ");
		print_msg(cypher);
	write(sock,cypher,200);
}
int main()
{
    int sockfd, newsockfd,clilen,port;
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
    		printf("	 Enter port number for KDC server <-:");
    		scanf("%d",&port);
    		printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    	printf("            KDC SERVER running at port no :-> %d\n",port);
    				serv_addr.sin_family = AF_INET;
    					serv_addr.sin_addr.s_addr = INADDR_ANY;
    						serv_addr.sin_port = htons(port);
						    if (bind(sockfd, (struct sockaddr *) &serv_addr,
									  sizeof(serv_addr)) < 0)
						    {
							 perror("ERROR on binding");
							 exit(1);
						    }
						    listen(sockfd,5);
						    clilen = sizeof(cli_addr);
    while (1) 
    {
        		newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
				if (newsockfd < 0)
				{
				    perror("ERROR on accept");
				    exit(1);
				}
        			pid = fork();
			if (pid == 0)  
			{
			  
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
