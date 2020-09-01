#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
	WsaData wsaData;
	int listen_fd=-1;
	struct sockaddr_in addr_in;
	
	if(0!=WSAStartup(MAKEWORD(2,0),&wsaData)){
		perror("WSAStartup() error");
		return EXIT_FAILURE;
	}
	
	listen_fd=socket(AF_INET,SOCK_STREAM,0);
	
	if(INVALID_SOCKET==sockfd){
		perror("socket() error");
		return EXIT_FAILURE;
	}
	
	addr_in.sin_family=AF_INET;
	addr_in.sin_addr=htonl(INADDR_ANY),
	addr_in.sin_port=htons(1234);	
	closesocket(listen_fd);
	return 0;
}
