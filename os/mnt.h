#ifndef __OS_MNT_H_
#define __OS_MNT_H_

/* 
 * return 0 for success, > 0 is the number of failed filesystems, < 0 for an error
 */
int ueld_os_umount_all();

#endif /* __OS_MNT_H_ */
