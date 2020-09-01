#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock.h>
#include "buf.h"

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
	WSADATA wsaData;
	int listen_fd=-1,conn_fd=-1;
	struct sockaddr_in addr;
	struct buf_t bufx;
	char buf[512],*p=NULL,*q=NULL,*str=NULL;
	int m=0,n=0,head_len=0,content_len=0,str_len=0,addr_size=sizeof(struct sockaddr);
	
	
	buf_init(&bufx);
	
	if(0!=WSAStartup(MAKEWORD(2,0),&wsaData)){
		perror("WSAStartup() error");
		return EXIT_FAILURE;
	}
	
	listen_fd=socket(AF_INET,SOCK_STREAM,0);
	
	if(INVALID_SOCKET==listen_fd){
		perror("socket() error");
		return EXIT_FAILURE;
	}
	
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(10082);
	
	if(0!=bind(listen_fd,(struct sockaddr*)&addr,sizeof(addr))){
		perror("bind() error");
		return EXIT_FAILURE;
	}
	
	if(0!=listen(listen_fd,200)){
		perror("listen() error");
		return EXIT_FAILURE;
	}
	
	printf("Listening on port:%d\n\n",10082);
	
	for(;;){
		conn_fd=accept(listen_fd,(struct sockaddr*)&addr,&addr_size);
		
		if(INVALID_SOCKET==conn_fd){
			perror("accept() error");
			continue;
		}
		
		printf("A new connection %s:%d\n",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));
		
		for(;;){
			n=recv(conn_fd,buf,sizeof(buf),0);
			
			if(n<=0){
				printf("socket close fd=%d\n",conn_fd);
				closesocket(conn_fd);
				break;
			}
			
			buf_push(&bufx,buf,n);
			buf_to_str(&bufx);
			str=buf_get_head(&bufx);
			str_len=strlen(str);
			p=strstr(str,"\r\n\r\n");
			
			//当前缓冲区中没有找到head分隔符 ,继续接收 
			if(NULL==p){
				continue;
			}
			
			head_len=p-str+3;//头部字符个数 
			
			p=strstr(str,"Content-Length: ");
			
			//存在内容长度字段 
			if(p!=NULL){
				p+=16;
				q=strstr(p,"\r\n");
				
				if(NULL==q){
					break;
				}
				
				memcpy(buf,p,q-p);
				buf[q-p]='\0';
				content_len=atoi(buf);
				
				//接收剩下的内容
				if(str_len-head_len < content_len){
					
					m=content_len-(str_len-head_len);
					
					for(;;){
						
						n=recv(conn_fd,buf,m,0);
						
						if(n<=0){
							break;
						}
						
						buf_push(&bufx,buf,n);
						
						if(n>=m){
							break;
						}
						
					}
				}
				
				buf_to_str(&bufx);
			} 
			
			break;
			
		}
		
		printf("%s\n",str);
		buf_reset(&bufx);
		
		//回应一个http响应
		sprintf(buf,"%s\r\n%s\r\n%s\r\n%s\r\n\r\n%s",
		"HTTP/1.1 200 OK",
		"Server: http-listen (Power by liangdodo)",
		"Content-Type: text/html",
		"Content-Length: 14",
		"You look good!"
		);
		
		send(conn_fd,buf,strlen(buf),0); 
		closesocket(conn_fd);
	}
		
	closesocket(listen_fd);
	return 0;
}
