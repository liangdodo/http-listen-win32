#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buf.h"

int buf_init(struct buf_t *buf){
	char *p=NULL;
	int size=BUF_DEFAULT_SIZE;
	
	buf->base=NULL;
	buf->head=0;
	buf->tail=0;
	buf->size=0;
	
	p=(char*)malloc(size+1);
	
	if(NULL==p){
		return -1;
	}
	
	//memset(p,0x00,size+1);
	buf->base=p;
	buf->size=size;
	return 0;
	
}

int buf_destroy(struct buf_t *buf){
	if(buf->base){
		free(buf->base);
		buf->head=0;
		buf->tail=0;
		buf->size=0;
		buf->base=NULL;
	}
	return 0;
}

int buf_get_free(struct buf_t *buf){
	return buf->size-buf->tail;
}

int buf_get_length(struct buf_t *buf){
	return buf->tail-buf->head;
}

int buf_push(struct buf_t *buf,void *src,int size){
    int n=0;
	char *p=NULL;
	
	if(size <= 0){
		return 0;
	}
	
	n=buf_get_free(buf);
	
	if(n < size ){
		p=(char*)realloc(buf->base,buf->size+size-n+1);
		
		if(NULL==p){
			//free(buf->base);
			buf_destroy(buf);
			return 0;
		}
		
		
		buf->base=p;
		buf->size+=size-n;
		
		n=buf_get_free(buf);
		//memset(buf_get_tail(buf)+1,0x00,n+1);
	}
	
	
	//printf("push n=%d\n",n);
	//printf("push size-n+1=%d\n",size-n+1);
	memcpy(buf_get_tail(buf)+1,src,size);
	buf->tail+=size;
	return size;
}


int buf_pop(struct buf_t *buf,void *dst,int size){
	int k=0;

	if(size <= 0){
		return 0;
	}
	
	k=buf->tail-size;
	
	if(k < buf->head){
		return -1;
	}
	
	if(dst){
		memcpy(dst,buf->base+k,size);
	}
	
	buf->tail=k;
	return size;
}


int buf_shift(struct buf_t *buf,void *dst,int size){
	int k=0;

	if(size <= 0){
		return 0;
	}
	
	k=buf->head+size;
	
	if(k > buf->tail){
		return -1;
	}
	
	if(dst){
		memcpy(dst,buf_get_head(buf),size);
	}
	
	buf->head+=size;
	return size;
}

int buf_reset(struct buf_t *buf){
	buf_destroy(buf);
	buf_init(buf);
	
	/**
	char *p=NULL;
	int size=1024;
	
	p=(char*)realloc(buf->base,size+1);
	
	if(NULL==p){
		buf_destroy(buf);
		return -1;
	}
	
	memset(p,0x00,size+1);
	
	buf->base=p;
	buf->head=0;
	buf->tail=0;
	buf->size=size;
	*/
	return 0;
}

int buf_read(struct buf_t *buf,int start,int len,void *dst){
	char *s=NULL;
	
	if(len<=0){
		return 0;
	}
	
	if(buf->head+len >= buf->tail){
		return -1;
	}
	
	s=buf->base+buf->head;
	memcpy(dst,s+start,len);
	return len;
}

char* buf_get_head(struct buf_t *buf){
	return buf->base+buf->head;
}

char* buf_get_tail(struct buf_t *buf){
	return buf->base+buf->tail-1;
}

int buf_get_size(struct buf_t *buf){
	return buf->size;
}

int buf_clean(struct buf_t *buf){
	//memset(buf->base,0x00,buf->size+1);
	return buf->tail=buf->head=0;
}

char* buf_realloc(struct buf_t *buf,int new_size){
	char *p=NULL;
	
	if(buf->base){
		p=(char*)realloc(buf->base,new_size);
		
		if(NULL==p){
			return NULL;
		}
		
		buf->base=p;
		buf->size=new_size;
		return p;
	}
	
	return NULL;
}

void buf_to_str(struct buf_t *buf){
    *(buf->base+buf->tail)='\0';
}
