#ifndef __LOG_H_
#define __LOG_H_

#define UELD_LOG_PROTO_NONE 0
#define UELD_LOG_PROTO_FILE 1
#define UELD_LOG_PROTO_KLOG 2

typedef void (*ueld_write_log_t)(char *msg);
extern ueld_write_log_t ueld_write_log;
extern int logfd;

void ueld_log_init();
void ueld_log_close();

#endif // __LOG_H_
