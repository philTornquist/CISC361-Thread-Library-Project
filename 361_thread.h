/* 
 * thread library function prototypes
 */

void t_create(void(*function)(int), int thread_id, int priority);
void t_yield(void);
void t_init(void);
void t_terminate(void);
void t_shutdown(void);

typedef void sem_t;

int sem_init(sem_t **sp, int sem_count);
void sem_wait(sem_t *sp);
void sem_signal(sem_t *sp);
void sem_destroy(sem_t **sp);

typedef void mbox;

int mbox_create(mbox **mb);
void mbox_destroy(mbox **mb);

void mbox_deposit(mbox *mb, char *msg, int len);
void mbox_block_deposit(mbox *mb, char *msg, int len);

void mbox_withdraw(mbox *mb, char *msg, int *len);
void mbox_tid_withdraw(mbox *mb, int *tid, char *msg, int *len);
void mbox_block_withdraw(mbox *mb, int *tid, char *msg, int *len);


