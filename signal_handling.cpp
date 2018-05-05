/**
 * More info?
 * a.dotreppe@aspyct.org
 * http://aspyct.org
 * @aspyct (twitter)
 *
 * Hope it helps :)
 */

/*
 * For reference, here is a list of all the signals on Linux.
The signals from SIGRTMIN and above are real time signals.

$ uname -a
Linux localhost 4.9.0-6-amd64 #1 SMP Debian 4.9.82-1+deb9u3 (2018-03-02) x86_64 GNU/Linux

$ kill -l
 1) SIGHUP       2) SIGINT       3) SIGQUIT      4) SIGILL       5) SIGTRAP
 6) SIGABRT      7) SIGBUS       8) SIGFPE       9) SIGKILL     10) SIGUSR1
11) SIGSEGV     12) SIGUSR2     13) SIGPIPE     14) SIGALRM     15) SIGTERM
16) SIGSTKFLT   17) SIGCHLD     18) SIGCONT     19) SIGSTOP     20) SIGTSTP
21) SIGTTIN     22) SIGTTOU     23) SIGURG      24) SIGXCPU     25) SIGXFSZ
26) SIGVTALRM   27) SIGPROF     28) SIGWINCH    29) SIGIO       30) SIGPWR
31) SIGSYS      34) SIGRTMIN    35) SIGRTMIN+1  36) SIGRTMIN+2  37) SIGRTMIN+3
38) SIGRTMIN+4  39) SIGRTMIN+5  40) SIGRTMIN+6  41) SIGRTMIN+7  42) SIGRTMIN+8
43) SIGRTMIN+9  44) SIGRTMIN+10 45) SIGRTMIN+11 46) SIGRTMIN+12 47) SIGRTMIN+13
48) SIGRTMIN+14 49) SIGRTMIN+15 50) SIGRTMAX-14 51) SIGRTMAX-13 52) SIGRTMAX-12
53) SIGRTMAX-11 54) SIGRTMAX-10 55) SIGRTMAX-9  56) SIGRTMAX-8  57) SIGRTMAX-7
58) SIGRTMAX-6  59) SIGRTMAX-5  60) SIGRTMAX-4  61) SIGRTMAX-3  62) SIGRTMAX-2
63) SIGRTMAX-1 64) SIGRTMAX
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h> // sigaction(), sigsuspend(), sig*()
#include <unistd.h> // alarm()

void handle_signal(int signal);
void handle_sigalrm(int signal);
void do_sleep(int seconds);

/* Usage example
 *
 * First, compile and run this program:
 *     $ gcc signal.c
 *     $ ./a.out
 *
 * It will print out its pid. Use it from another terminal to send signals
 *     $ kill -HUP <pid>
 *     $ kill -USR1 <pid>
 *     $ kill -ALRM <pid>
 *
 * Exit the process with ^C ( = SIGINT) or SIGKILL, SIGTERM
 */
int main() {
	struct sigaction sa;

	// Print pid, so that we can send signals from other shells
	printf("My pid is: %d\n", getpid());

	// Setup the sighub handler
	sa.sa_handler = &handle_signal;

	// Restart the system call, if at all possible
	sa.sa_flags = SA_RESTART;

	// Block every signal during the handler
	sigfillset(&sa.sa_mask);

	// Intercept SIGHUP and SIGINT
	if (sigaction(SIGHUP, &sa, NULL) == -1) {
		perror("Error: cannot handle SIGHUP"); // Should not happen
	}

	if (sigaction(SIGUSR1, &sa, NULL) == -1) {
		perror("Error: cannot handle SIGUSR1"); // Should not happen
	}

	// Will always fail, SIGKILL is intended to force kill your process
	if (sigaction(SIGKILL, &sa, NULL) == -1) {
		perror("Cannot handle SIGKILL"); // Will always happen
		printf("You can never handle SIGKILL anyway...\n");
	}

	if (sigaction(SIGINT, &sa, NULL) == -1) {
		perror("Error: cannot handle SIGINT"); // Should not happen
	}

	for (;;) {
		printf("\nSleeping for ~3 seconds\n");
		do_sleep(3); // Later to be replaced with a SIGALRM
	}
}

void handle_signal(int signal) {
	const char *signal_name;
	sigset_t pending;

	// Find out which signal we're handling
	switch (signal) {
		case SIGHUP:
			signal_name = "SIGHUP";
			break;
		case SIGUSR1:
			signal_name = "SIGUSR1";
			break;
		case SIGINT:
			printf("Caught SIGINT, exiting now\n");
			exit(0);
		default:
			fprintf(stderr, "Caught wrong signal: %d\n", signal);
			return;
	}

	/*
	 * Please note that printf et al. are NOT safe to use in signal handlers.
	 * Look for async safe functions.
	 */
	printf("Caught %s, sleeping for ~3 seconds\n"
		   "Try sending another SIGHUP / SIGINT / SIGALRM "
		   "(or more) meanwhile\n", signal_name);
	/*
	 * Indeed, all signals are blocked during this handler
	 * But, at least on OSX, if you send 2 other SIGHUP,
	 * only one will be delivered: signals are not queued
	 * However, if you send HUP, INT, HUP,
	 * you'll see that both INT and HUP are queued
	 * Even more, on my system, HUP has priority over INT
	 *
	 * There are two types of signals: standard signals,
	 * and realtime signals. Realtime signals are SIGRTMIN (34)
	 * and above.
	 * While there can be only one traditional signal waiting at
	 * a time for each type (i.e. one HUP, one INT etc), there
	 * However, there can be multiple realtime signals queued
	 * for each type (e.g. 3 SIGRTMIN, 4 SIGRTMAX...)
	 *
	 * See the other file for a list of all the signals available.
	 */
	do_sleep(3);
	printf("Done sleeping for %s\n", signal_name);

	// So what did you send me while I was asleep?
	sigpending(&pending);
	if (sigismember(&pending, SIGHUP)) {
		printf("A SIGHUP is waiting\n");
	}
	if (sigismember(&pending, SIGUSR1)) {
		printf("A SIGUSR1 is waiting\n");
	}

	printf("Done handling %s\n\n", signal_name);
}

void handle_sigalrm(int signal) {
	if (signal != SIGALRM) {
		fprintf(stderr, "Caught wrong signal: %d\n", signal);
	}

	printf("Got sigalrm, do_sleep() will end\n");
}

void do_sleep(int seconds) {
	struct sigaction sa;
	sigset_t mask;

	sa.sa_handler = &handle_sigalrm; // Intercept and ignore SIGALRM
	sa.sa_flags = SA_RESETHAND; // Remove the handler after first signal
	sigfillset(&sa.sa_mask);
	sigaction(SIGALRM, &sa, NULL);

	// Get the current signal mask
	sigprocmask(0, NULL, &mask);

	// Unblock SIGALRM
	sigdelset(&mask, SIGALRM);

	// Wait with this mask
	alarm(seconds);
	sigsuspend(&mask);

	printf("sigsuspend() returned\n");
}
