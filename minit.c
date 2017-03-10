#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <termios.h>

#include "tools.h"

#include "restarts.h"

int timeout = 0;

int ueld_main(int argc, char* argv[]);

void sig_alarm(int signo) {
	timeout = 1;
}

int main(int argc, char* argv[]) {
	if (argc > 0 && (strcmp(argv[0], "-ueld") == 0)) {
		ueld_main(argc, argv);
	}
	
	if (ueld_readconfiglong("ueld_enable_muti_init", -1) != 1) {
		ueld_main(argc, argv);
	}
	
	/* then, use muti init... */
		
	if (getpid() != 1) {
		char* args[argc + 1];
		for (int i = 0; i < argc; i++) {
			args[i] = argv[i];
		}
		args[argc] = 0;

		char* other_init_telinit = ueld_readconfig("ueld_muti_init_other_init_telinit");
		if (other_init_telinit)
			execvp(other_init_telinit, args);
		ueld_freeconfig(other_init_telinit);

		ueld_main(argc, argv);
	}
	
	ueld_signal(SIGALRM, sig_alarm, 0);

	printf("%s\n", "=============================\n"
	"[Ueld Muti Init]\n"
	"Press the keys(default s) to load the init which you configed, "
	"the others or timeout to load ueld.");

	struct termios ots, ts;
	int s;
	if ((s = tcgetattr(STDIN_FILENO, &ts)) == 0) {
		ots = ts;

		ts.c_lflag &= ~(ECHO | ECHONL | ICANON);
		ts.c_cc[VMIN] = 1;
		ts.c_cc[VTIME] = 0;

		if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &ts) < 0)
			ueld_echo("Warning: could not change the terminal characteristics.");

	} else {
		ueld_echo("Warning: could not get the terminal characteristics.");
	}

	alarm(ueld_readconfiglong("ueld_muti_init_choose_time_out", 3));
	char c = 0;

	read(STDIN_FILENO, &c, 1);
	ueld_signal(SIGALRM, SIG_IGN, 0);

	if (s == 0){
		if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &ots) < 0)
			ueld_echo("Warning: could not restore the terminal characteristics.");
	}

	char* press_keys = ueld_readconfig("ueld_muti_init_press_keys");
	if (!press_keys) press_keys = "sS";

	if ((c != '\0') && strchr(press_keys, c) && !timeout) {
		ueld_freeconfig(press_keys);
		char* other_init = ueld_readconfig("ueld_muti_init_other_init");
		if (other_init)
			execl(other_init, argv[0], 0);
		ueld_freeconfig(other_init);
		ueld_echo("Can not load other init, load ueld!!!");
	}
	ueld_freeconfig(press_keys);
	
	ueld_main(argc, argv);
	exit(0);
}
