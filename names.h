#ifndef __NAMES_H_
#define __NAMES_H_

#ifdef LINUX
#define VT_TTY_NAME "/dev/tty%d"
#endif /* LINUX */

#ifdef BSD
#define VT_TTY_NAME "/dev/ttyv%d"
#endif /* BSD */

#endif /* __NAMES_H_ */
