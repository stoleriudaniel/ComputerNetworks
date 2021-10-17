#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4444

int main(){

	int clientSocket, ret;
	struct sockaddr_in serverAddr;
	char buffer[1024];

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Client Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Connected to Server.\n");

	//primire introTxt de la server
	char introTxt[250];introTxt[0]='\0'; bzero(introTxt,sizeof(introTxt));
	if(recv(clientSocket,introTxt,250,0)<0)
	{
		printf("[-]Eroare la primirea textului intro.\n"); exit(1);
	}
	printf("\n%s",introTxt);
	char comanda[10];
	while(1){
		buffer[0]='\0'; bzero(buffer,sizeof(buffer));
		if(recv(clientSocket, buffer, 1024, 0) < 0){
			printf("[-]Error in receiving data.\n");
		}else{
			printf("[Server] %s\n", buffer);
		}
		bzero(buffer,sizeof(buffer));
		printf("[Client] "); //scanf("%s",comanda);
		gets(comanda);
		send(clientSocket,comanda,strlen(comanda),0);
		if(strcmp(comanda,"iesire")==0)
		{
			close(clientSocket);
			printf("[-]Disconnected from server.\n");
			exit(1); break;
		}
	}
	return 0;
}
