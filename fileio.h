#ifndef __FILEIO_H_
#define __FILEIO_H_

ssize_t readnsa(int fd, char** vptr);
FILE* mysetmntent(const char* mtab, const char* type, int* pfd, char** pbuffer);
int myendmntent(FILE* mnt, int* pfd, char** pbuffer);

#endif /* __FILEIO_H_ */
