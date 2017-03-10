void* ueld_signal(int signum, void* handler, int restartsyscall);
int ueld_unblock_signal(int signum);
int ueld_block_signal(int signum);
void ueld_echo(char* msg);
void ueld_print(char* fmt, ...);

#define URF_WAIT 1
#define URF_NOOUTPUT (1 << 1)
#define URF_SETVT (1 << 2)
#define URF_CMDLINE (1 << 3)
pid_t ueld_run(char* file, int flag, int vt);

char* ueld_readconfig(char* name);
long ueld_readconfiglong(char* name, long defaultval);
void ueld_freeconfig(char* value);
void ueld_closeconfig();
