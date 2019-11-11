#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>

#include <arpa/inet.h>

uint8_t CAMERA_STREAMING;
uint16_t PICTURE_SIZE_X;
uint16_t PICTURE_SIZE_Y;
uint8_t CAMERA_SINGLE_SHOT;
uint8_t JPEG_QUALITY;
uint8_t CONTRAST;
uint16_t STREAMIG_PORT;

uint32_t CAMERA_SET_IP;
uint32_t CAMERA_STREAMING_IP;

char *ARTICLE_CODE = "SMITCAM";
char *APPLICATION_VERSION = "SMITv0.1";

void initvar(){
    struct sockaddr_in ip;

    CAMERA_STREAMING = 0;
    PICTURE_SIZE_X = 640;
    PICTURE_SIZE_Y = 480;
    CAMERA_SINGLE_SHOT = 0;
    JPEG_QUALITY = 80;
    CONTRAST = 50;
    STREAMIG_PORT = 53252;

    inet_aton("192.168.2.70", &ip.sin_addr);
    CAMERA_SET_IP = ip.sin_addr.s_addr;
    inet_aton("192.168.2.130", &ip.sin_addr);
    CAMERA_STREAMING_IP = ip.sin_addr.s_addr;
}

void strupr(char *str);

void readVar(char *var, char **payload, int *len){
    char str[1024];
    struct sockaddr_in ip;

    strupr(var);
    printf("R: %s\n", var);
    if(strcmp(var,"CAMERA_STREAMING") == 0){
        sprintf(str, "\x1B%s\t%d\r", var, CAMERA_STREAMING);
    }
    else if(strcmp(var,"PICTURE_SIZE_X") == 0){
        sprintf(str, "\x1B%s\t%d\r", var, PICTURE_SIZE_X);
    }
    else if(strcmp(var,"PICTURE_SIZE_Y") == 0){
        sprintf(str, "\x1B%s\t%d\r", var, PICTURE_SIZE_Y);
    }
    else if(strcmp(var,"CAMERA_SINGLE_SHOT") == 0){
        sprintf(str, "\x1B%s\t%d\r", var, CAMERA_SINGLE_SHOT);
    }
    else if(strcmp(var,"JPEG_QUALITY") == 0){
        sprintf(str, "\x1B%s\t%d\r", var, JPEG_QUALITY);
    }
    else if(strcmp(var,"CONTRAST") == 0){
        sprintf(str, "\x1B%s\t%d\r", var, CONTRAST);
    }
    else if(strcmp(var,"STREAMING_PORT") == 0){
        sprintf(str, "\x1B%s\t%d\r", var, STREAMIG_PORT);
    }
    else if(strcmp(var,"ARTICLE_CODE") == 0){
        sprintf(str, "\x1B%s\t%s\r", var, ARTICLE_CODE);
    }
    else if(strcmp(var,"APPLICATION_VERSION") == 0){
        sprintf(str, "\x1B%s\t%s\r", var, APPLICATION_VERSION);
    }
    else if(strcmp(var,"CAMERA_SET_IP") == 0){
        ip.sin_addr.s_addr = CAMERA_SET_IP;
        sprintf(str, "\x1B%s\t%s\r", var, inet_ntoa(ip.sin_addr));
    }
    else if(strcmp(var,"CAMERA_STREAMING_IP") == 0){
        ip.sin_addr.s_addr = CAMERA_STREAMING_IP;
        sprintf(str, "\x1B%s\t%s\r", var, inet_ntoa(ip.sin_addr));
    }
    else{
        printf("No var: %s\n",var);
        exit(EXIT_FAILURE);
    }

    *len = strlen(str);
    *payload = (char *) malloc(*len);

    memcpy(*payload, str, *len);
}

void writeVar(char *var, char* val){
    struct sockaddr_in ip;

    strupr(var);

    printf("W: %s -> %s\n", var, val);

    if(strcmp(var,"CAMERA_STREAMING") == 0){
        sscanf(val, "%2" SCNu8, &CAMERA_STREAMING);
    }
    else if(strcmp(var,"PICTURE_SIZE_X") == 0){
        sscanf(val, "%2" SCNu16, &PICTURE_SIZE_X);
    }
    else if(strcmp(var,"PICTURE_SIZE_Y") == 0){
        sscanf(val, "%2" SCNu16, &PICTURE_SIZE_Y);
    }
    else if(strcmp(var,"CAMERA_SINGLE_SHOT") == 0){
        sscanf(val, "%2" SCNu8, &CAMERA_SINGLE_SHOT);
    }
    else if(strcmp(var,"JPEG_QUALITY") == 0){
        sscanf(val, "%2" SCNu8, &JPEG_QUALITY);
    }
    else if(strcmp(var,"CONTRAST") == 0){
        sscanf(val, "%2" SCNu8, &CONTRAST);
    }
    else if(strcmp(var,"STREAMING_PORT") == 0){
        sscanf(val, "%2" SCNu16, &STREAMIG_PORT);
    }
    else if(strcmp(var,"CAMERA_SET_IP") == 0){
        inet_aton(val,&ip.sin_addr);
        CAMERA_SET_IP = ip.sin_addr.s_addr;
    }
    else if(strcmp(var,"CAMERA_STREAMING_IP") == 0){
        inet_aton(val,&ip.sin_addr);
        CAMERA_STREAMING_IP = ip.sin_addr.s_addr;
    }
    else if(strcmp(var,"ARTICLE_CODE") == 0){
        printf("RO  var : %s", var);
    }
    else if(strcmp(var,"APPLICATION_VERSION") == 0){
        printf("RO  var : %s", var);
    }
    else{
        printf("No var: %s\n",var);
        exit(EXIT_FAILURE);
    }
}

void fileout(void){
	FILE *fp,*network;
	char strin[512];
	char strout[512];
    	struct sockaddr_in ip;

	int linectr = 0;
	fp = fopen("network_settings.txt","r");
	network = fopen("network","w+");

	if(fp == NULL || network == NULL){
		printf("FILE IO ERROR\n");
		exit(-1);
	}
	if(feof(fp)){
		printf("EMPTY FILE\n");
		exit(-1);
	}

	while(!feof(fp)){
		fgets(strin, 512, fp);
		if(!feof(fp)){
			++linectr;
		}
		else{
			break;
		}
		printf("-%d : %s",linectr,strin);
		switch(linectr){
			case 2:
		                ip.sin_addr.s_addr = CAMERA_SET_IP;
				sprintf(strout,"%s\n",inet_ntoa(ip.sin_addr));
				break;
			case 21:
				sprintf(strout,"%d\n",STREAMIG_PORT);
				break;
			default:
				sprintf(strout,"%s",strin);
		}
		printf("+%d : %s",linectr,strout);
		fputs(strout, network);
	}

	fclose(fp);
	fclose(network);

	remove("network_settings.txt");
	rename("network", "network_settings.txt");
}

void filein(void){
	FILE *fp,*network;
	char strin[512];
	char strout[512];
	struct sockaddr_in ip;

	int linectr = 0;
	fp = fopen("network_settings.txt","r");
	network = fopen("network","w+");

	if(fp == NULL || network == NULL){
		printf("FILE IO ERROR\n");
		exit(-1);
	}
	if(feof(fp)){
		printf("EMPTY FILE\n");
		exit(-1);
	}

	while(!feof(fp)){
		fgets(strin, 512, fp);
		if(!feof(fp)){
			++linectr;
			break;
		}
		//printf("-%d : %s",linectr,strin);
		switch(linectr){
			case 2:
				sscanf(strin,"%s\n",strout);
	        		inet_aton(strout,&ip.sin_addr);
               			CAMERA_STREAMING_IP = ip.sin_addr.s_addr;
			break;
			case 21:
				sscanf(strin,"%u\n", &STREAMIG_PORT);
			break;
		}
	}

	fclose(fp);
}

void strupr(char *str){
    while (*str != '\0')
    {
        *str = toupper(*str);
        str++;
    }
}
