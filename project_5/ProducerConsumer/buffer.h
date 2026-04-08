// buffer item type and buffer size
typedef int buffer_item;
#define BUFFER_SIZE 5

// function prototypes
int buffer_init(void);
void buffer_shutdown(void);
int insert_item(buffer_item item);
int remove_item(buffer_item *item);
void *producer(void *param);
void *consumer(void *param);
