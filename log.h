#ifndef __LOG_H_
#define __LOG_H_

#define UELD_LOG_PROTO_NONE 0
#define UELD_LOG_PROTO_FILE 1

void ueld_log_init();
void ueld_log_close();
void ueld_write_log(char *msg);

#endif // __LOG_H_
