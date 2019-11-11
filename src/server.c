#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#include "state.h"

#define PORT	53252
#define MAXLINE 1024

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void dump(char* buffer, int len);

void * socketThread(void *arg);
void process_payload(char* payload, int len, char** ret_payload, int* ret_len);

int main() {
	int sd, nsd;
	struct sockaddr_in me, you;
	struct in_addr localaddr;

	if ( (sd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&me, 0, sizeof(me));
	memset(&you, 0, sizeof(you));

	me.sin_family = AF_INET;
	me.sin_addr.s_addr = htonl(INADDR_ANY);
	me.sin_port = htons(PORT);

   	if ( bind(sd, (struct sockaddr *)&me, sizeof(me)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}


	char *ifname = "enp2s0";
	if(setsockopt(sd, SOL_SOCKET, SO_BINDTODEVICE, ifname, sizeof(ifname)) != -1){
    	perror("bind2dev error");
        exit(EXIT_FAILURE);
	}

    if ( (listen(sd, 5)) != 0 ) {
    	perror("listen error");
        exit(EXIT_FAILURE);
    }

    initvar();
    filein();

    pthread_t tid[60];
    int i = 0;

	while(1){
		int you_len;

        nsd = accept(sd, (struct sockaddr *) &you, &you_len);
        if (nsd < 0) {
            perror("accept error");
            exit(EXIT_FAILURE);
        }

		if( pthread_create(&tid[i], NULL, socketThread, &nsd) != 0 )
           printf("Failed to create thread\n");
        if( i >= 50)
        {
          i = 0;
          while(i < 50)
          {
            pthread_join(tid[i++],NULL);
          }
          i = 0;
        }
	}

	return 0;
}

void * socketThread(void *arg){
	char buffer[MAXLINE];
	char *ret_buf;
	int ret_len;
    int n;

    int nsd = *((int *)arg);

    memset(buffer, 0 , sizeof(buffer));

    n = read(nsd, (char *)buffer, MAXLINE);


    // printf("CLIENT : %s\n",inet_ntoa(you.sin_addr));
    printf("D_CLIENT: %d(%04X)\n",n,n);
    dump(buffer, n);


    pthread_mutex_lock(&lock);
    process_payload(buffer, n, &ret_buf, &ret_len);

    printf("RESP:\n");
    dump(ret_buf,ret_len);

    fileout();
    pthread_mutex_unlock(&lock);

    if(write(nsd, (const char *)ret_buf, ret_len) <0 ){
        perror("response error");
        exit(EXIT_FAILURE);
    }
    close(nsd);		//closing TCP session after each command
    pthread_exit(NULL);
}

void process_payload(char* payload, int maxlen, char** ret_payload, int* ret_len){
    char com_t[2];
    char com_var[1024];
    char com_val[1024];

	*ret_len = 0;
	*ret_payload = NULL;

	if(*payload != 0x1B){
	    printf("Not a SPS packet\n");
	    return;
    }

    if(*(payload+maxlen-1) == '\r')
        *(payload+maxlen-1) = '\0';

    memcpy(com_t, payload+1 ,2);
    if(strcasecmp(com_t,"MO") == 0){
        sscanf(payload+3, "%s", com_var);
        readVar(com_var, ret_payload, ret_len);
    }
    else if(strcasecmp(com_t,"MS") == 0){
        sscanf(payload+3, "%s\t%s", com_var, com_val);
        writeVar(com_var, com_val);
    }
}

void dump(char* buffer, int len){
	if(buffer == NULL){
		printf("<<Empty Buffer>>\n\n");
		return;
	}
    for(int i = 0; i<len;){
    	for (int j = 0; i<len && j < 4; j++){
    		printf(" %02X ", buffer[i++]);
    	}
    	printf("\n");
    }
    printf("\n\n");
}
