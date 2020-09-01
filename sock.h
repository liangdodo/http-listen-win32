#include <windows.h>
#include <winsock.h>
//#include <ws2tcpip.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include "buf.h"

#define SOCK_FNORMAL 0
#define SOCK_FCLOSE  1
#define SOCK_FRPAUSE 2
#define SOCK_FWPAUSE 4

struct sslctx_t{
	char *cer_file;
	char *key_file;
	char *ca_file;
	char *ca_path;
	int  verify_mode;
	int  verify_depth;
	int (*verify_callback)(int,X509_STORE_CTX*);
};

struct sock_t{
	int fd;
	SSL *ssl;
	SSL_CTX *ssl_ctx;
	struct buf_t r_buf;
	struct buf_t w_buf;
	void *ptr;
	int flag;
	int ssl_handshake;
};


int sock_create_listen(char *ip,short port,int backlog);
int sock_init(struct sock_t *so,int fd);
int sock_read(struct sock_t *so,void *buf,int size,int msg_peek);
int sock_write(struct sock_t *so,void *buf,int size);
int sock_close(struct sock_t *so);
int sock_sslctx_load(struct sock_t *so,struct sslctx_t *sc);
int sock_sslctx_free(struct sock_t *so);
int sock_eagain(struct sock_t *so,int res,int mod);
int sock_setnonblock(int fd);
int sock_setblock(int fd);
int sock_setsndtimeout(int fd,int timeout);
int sock_setrcvtimeout(int fd,int timeout);
int sock_raccept(int lsfd,int nonblock);
int sock_ssl_accept(struct sock_t *so);
int sock_ssl_connect(struct sock_t *sop,int timeout);
int sock_nonblock_read(struct sock_t *so);
int sock_nonblock_write(struct sock_t *so);
int sock_get_flag(struct sock_t *so);
int sock_set_flag(struct sock_t *so,int flag);
int sock_get_sndbuf_size(int fd);
int sock_get_rcvbuf_size(int fd);
int sock_gethostbyname(char *hostname,char *ip_buf,int size);
int sock_connect(int fd,char *ip,unsigned short port,int timeout);
int sock_get_fionread(int fd);
int sock_set_r_buf(struct sock_t *so,struct buf_t buf);
int sock_set_w_buf(struct sock_t *so,struct buf_t buf);
