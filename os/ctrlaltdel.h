#ifndef __OS_CTRLALTDEL_H_
#define __OS_CTRLALTDEL_H_

typedef void (*CAD_handler)(void);
int ueld_os_handle_ctrl_alt_del(CAD_handler handler);

#endif // __OS_CTRLALTDEL_H_
