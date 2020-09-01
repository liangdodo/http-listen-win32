#define BUF_DEFAULT_SIZE 1024

struct buf_t{
	char *base;
	int head;
	int tail;
	int size;
};

int buf_init(struct buf_t *buf);
int buf_destroy(struct buf_t *buf);
int buf_push(struct buf_t *buf,void *src,int size);
int buf_pop(struct buf_t *buf,void *dst,int size);
int buf_shift(struct buf_t *buf,void *dst,int size);
int buf_get_length(struct buf_t *buf);
int buf_get_free(struct buf_t *buf);
int buf_reset(struct buf_t *buf);
int buf_read(struct buf_t *buf,int start,int len,void *dst);
char* buf_get_head(struct buf_t *buf);
char* buf_get_tail(struct buf_t *buf);
int buf_get_size(struct buf_t *buf);
int buf_clean(struct buf_t *buf);
char* buf_realloc(struct buf_t *buf,int new_size);
void buf_to_str(struct buf_t *buf);
