struct arg_t{
	int argc;
	char **argv;
};

int arg_init(struct arg_t *pa,int arg,char *argv[]);
int arg_is_exists(struct arg_t *pa,const char *name);
char* arg_get_value(struct arg_t *pa,const char *name);
