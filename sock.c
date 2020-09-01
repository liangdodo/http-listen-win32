#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sock.h"

int sock_init(struct sock_t *so,int fd){
	SSL *ssl=NULL;
	
	if(0!=buf_init(&(so->r_buf))){
		return -1;
	}
	
	if(0!=buf_init(&(so->w_buf))){
		buf_destroy(&(so->r_buf));
		return -1;
	}
	
	if(so->ssl_ctx){
		ssl=SSL_new(so->ssl_ctx);
		
		if(NULL==ssl){
			return -1;
		}

		if(!SSL_set_fd(ssl,fd)){
			SSL_free(ssl);
			return -1;
		}
    }
	
	so->fd=fd;
	so->flag=SOCK_FNORMAL;
	so->ssl_handshake=0;
	so->ssl=ssl;
	so->ptr=NULL;
	return 0;
}

int sock_create_listen(char *ip,short port,int backlog){
	int fd;
	int flag=1;
	struct sockaddr_in addr;
	fd=socket(AF_INET,SOCK_STREAM,0);

	if(-1 == fd){
		return fd;
	}

	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port        = htons(port);

	if(-1 == setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(void*)&flag,sizeof(flag))){
		return -1;
	}

	if(-1 == bind(fd,(struct sockaddr*)&addr,sizeof(addr))){
		return -1;
	}

	if(-1 == listen(fd,backlog)){
		return -1;
	}

	return fd;
}


int sock_setsndtimeout(int fd,int timeout){
	struct timeval tv;
	tv.tv_sec=0;
	tv.tv_usec=timeout*1000; 
	
	if(setsockopt(fd,SOL_SOCKET,SO_SNDTIMEO,(const char*)&tv,sizeof(tv))!=0){
		return -1;
	}
	
	return 0;
}

int sock_setrcvtimeout(int fd,int timeout){
	struct timeval tv;
	tv.tv_sec=0;
	tv.tv_usec=timeout*1000; 
	
	if(setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,(const char*)&tv,sizeof(tv))!=0){
		return -1;
	}
	
	return 0;
}


int sock_gethostbyname(char *hostname,char *ip_buf,int size){
	struct hostent hostinfo,*host=NULL;
	char *p=NULL;
	int err=0;
	
	if(INADDR_NONE == inet_addr(hostname)){
		
		host=gethostbyname(hostname);
		
		if(NULL==host){
			return -1;
		}
		
		p=inet_ntoa(*((struct in_addr*)host->h_addr));
		
		if(NULL==p){
			return -1;
		}
		
		memcpy(ip_buf,p,strlen(p)+1);	
	}else{
		memcpy(ip_buf,hostname,strlen(hostname)+1);
	}
	
	return 0;
}

int sock_connect(int fd,char *ip,unsigned short port,int timeout){
	int res=-1;
	struct sockaddr_in addr,in;
	int errinfo, errlen=sizeof(int);
    fd_set r_fds,w_fds,e_fds;
	struct timeval tv={0,timeout*1000};
	
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=inet_addr(ip);
	addr.sin_port=htons(port);

	res=connect(fd,(struct sockaddr*)&addr,sizeof(addr));
	
	if(-1==res){
		
		if(WSAGetLastError() == WSAEWOULDBLOCK){
			FD_ZERO(&w_fds);
			FD_SET(fd,&w_fds);
			
			res = select(fd + 1, NULL, &w_fds,NULL, &tv);

			if(res <= 0){
				return -1;
			}
			
			if(FD_ISSET(fd,&w_fds)){
			    //printf("w_fds!\n");
			    
				if(-1==getsockopt(fd, SOL_SOCKET, SO_ERROR, (char*)&errinfo, &errlen)){
					return -1;
				}
				
				if(0 != errinfo){
					return -1;
				}
				
				return 0;
			}
		}
		
		return -1;
	}
	
	return 0;
}


int sock_ssl_connect(struct sock_t *sop,int timeout){
	
	if(timeout > 0){
		sock_setsndtimeout(sop->fd,timeout);
		sock_setrcvtimeout(sop->fd,timeout);
	}

	return SSL_connect(sop->ssl);
}

int sock_setnonblock(int fd){
	int res;
	unsigned long ul=1;

	res=ioctlsocket(fd,FIONBIO,(unsigned long *)&ul);
	
	if(res==SOCKET_ERROR){
		return -1;
	}
	
	return 0;
}


int sock_setblock(int fd){
	int res;
	unsigned long ul=0;

	res=ioctlsocket(fd,FIONBIO,(unsigned long *)&ul);
	
	if(res==SOCKET_ERROR){
		return -1;
	}
	
	return 0;
}


int sock_read(struct sock_t *so,void *buf,int size,int msg_peek){
	
	if(-1==so->fd){
		return 0;
	}
	
	if(so->ssl){
		return msg_peek ? SSL_peek(so->ssl,buf,size) : SSL_read(so->ssl,buf,size);
	}
	
	return msg_peek ? recv(so->fd,buf,size,MSG_PEEK) : recv(so->fd,buf,size,0);
}

int sock_write(struct sock_t *so,void *buf,int size){
	
	if(-1==so->fd){
		return 0;
	}
	
	if(so->ssl){
		return SSL_write(so->ssl,buf,size);
	}
	
	return send(so->fd,buf,size,0);
}

int sock_close(struct sock_t *so){
	
	if(so->ssl){
		SSL_shutdown(so->ssl);
		SSL_free(so->ssl);
		so->ssl=NULL;
		so->ssl_ctx=NULL;
		so->ssl_handshake=0;
	}
	
	if(so->fd!=-1){
		closesocket(so->fd);
        //so->fd=-1;
	}
	
	if(so->ptr){
		free(so->ptr);
		so->ptr=NULL;
	}
	
	buf_destroy(&(so->r_buf));
	buf_destroy(&(so->w_buf));
	return 0;
}

int sock_get_rcvbuf_size(int fd){
	int val = 0;
	int len = sizeof(val);
	getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&val, &len);
	return val;
}

int sock_get_sndbuf_size(int fd){
	int val = 0;
	int len = sizeof(val);
	getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*)&val, &len);
	return val;
}

int sock_nonblock_read(struct sock_t *so){
	int res=0,m=0,n=0,e=0;
	char *p=NULL;
	
	if(so->ssl){
		m=sock_get_rcvbuf_size(so->fd);
	}else{
		m=sock_get_fionread(so->fd);
	}
	
	//缓冲区剩余的空间不足，扩展空间 
	if(buf_get_free(&(so->r_buf)) < m){
		if(NULL==buf_realloc(&(so->r_buf),buf_get_length(&(so->r_buf))+m)){
			return 0;
		}	
	}
	
	for(;;){
		res=sock_read(so,buf_get_tail(&(so->r_buf))+1,m-n,0);
		
		if(0 == res){
			return 0;
		}
		
		if(0 > res){
			e=sock_eagain(so,res,0);
			
			if(e){
				return -1;
			}
			
			return 0;
		}
		
		n+=res;
		so->r_buf.tail+=res;
		
		if(n >= m){
			break;
		}
	}
	
	return n;
}


int sock_nonblock_write(struct sock_t *so){
	int res=0,m=0,n=0,e=0;
	char *p=NULL;
	int k=1024;
	
	for(;;){
		m=buf_get_length(&(so->w_buf));
		
		if(m <= 0){
			break;
		}
		
		p=buf_get_head(&(so->w_buf));
		
		if(m > k){
			m=k;
		}
		
	    res=sock_write(so,p,m);	
		
		if(0 == res){
			return 0;
		}
		
		if(0 > res){
			
			e=sock_eagain(so,res,1);
			
			if(e){
				return -1;
			}
			
			//printf("unknow closed\n");
			
			return 0;
		}
		
	 	if(buf_shift(&(so->w_buf),NULL,res) <=0){
	 		return 0;
		}
		
		n+=res;
	}
	
	return n;
}


int sock_sslctx_load(struct sock_t *so,struct sslctx_t *sc){
	SSL_CTX *ctx;
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();
	ctx=SSL_CTX_new(SSLv23_method());
	
	if(NULL==ctx){
		return -1;
	}
	
	if(sc){
		if(NULL!=sc->cer_file){	
			//加载用户证书
			if(SSL_CTX_use_certificate_file(ctx,sc->cer_file,SSL_FILETYPE_PEM)<=0){
				SSL_CTX_free(ctx);
				return -1;
			}

		}

		if(NULL!=sc->key_file){
			//加载用户私钥
			if(SSL_CTX_use_PrivateKey_file(ctx,sc->key_file,SSL_FILETYPE_PEM)<=0){
				SSL_CTX_free(ctx);
				return -1;
			}
			
			//验证用户私钥
			if(!SSL_CTX_check_private_key(ctx)){
				SSL_CTX_free(ctx);
				return -1;
			}
		}

		if(NULL!=sc->ca_file || NULL!=sc->ca_path){
			//加载CA证书
			if(!SSL_CTX_load_verify_locations(ctx,sc->ca_file,sc->ca_path)){
				SSL_CTX_free(ctx);
				return -1;
			}
		}
		
		if(sc->verify_mode){
			SSL_CTX_set_verify(ctx,sc->verify_mode,sc->verify_callback);
		}
	}
	
	so->ssl_ctx=ctx;
	return 0;
}

int sock_sslctx_free(struct sock_t *so){
	
	if(NULL!=so->ssl_ctx){
		SSL_CTX_free(so->ssl_ctx);
	}

	return 0;
}

int sock_eagain(struct sock_t *so,int res,int mod){
	int e;

	if(so->ssl){
		e=SSL_get_error(so->ssl,res);

		if(0==mod && e==SSL_ERROR_WANT_READ ){
			return 1;	
		}else if(1==mod && e==SSL_ERROR_WANT_WRITE){
			return 1;
		}
		
		return 0;
	}
	
	if(WSAGetLastError() == WSAEWOULDBLOCK){
		return 1;
	}
	
	return 0;
}

int sock_raccept(int lsfd,int nonblock){
	struct sockaddr_in addr;
	int addr_len=sizeof(struct sockaddr);
	int conn_fd=-1;
	int e;
	
	if(nonblock){
		if(0!=sock_setnonblock(lsfd)){
			return 0;
		}
		
		conn_fd=accept(lsfd,(struct sockaddr*)&addr,&addr_len);
	
		if(-1==conn_fd){
			
			e=WSAGetLastError();
			
			if(e==WSAEWOULDBLOCK || e == WSAECONNABORTED || e == WSAEINTR){
				return -1;
			}
			
			return 0;
		}
	}else{
		conn_fd=accept(lsfd,(struct sockaddr*)&addr,&addr_len);
		
		if(-1==conn_fd){
			return 0;
		}
		
	}
	
	return conn_fd;
}

int sock_ssl_accept(struct sock_t *so){
	return SSL_accept(so->ssl);
}

int sock_get_flag(struct sock_t *so){
	return so->flag;
}

int sock_set_flag(struct sock_t *so,int flag){
	return so->flag = flag;
}


int sock_get_fionread(int fd){
	int avail =0;
	ioctlsocket(fd, FIONREAD, (u_long*)&avail);
	return avail;
}

int sock_set_r_buf(struct sock_t *so,struct buf_t buf){
	so->r_buf=buf;
	return 0;
}

int sock_set_w_buf(struct sock_t *so,struct buf_t buf){
	so->w_buf=buf;
	return 0;
}
