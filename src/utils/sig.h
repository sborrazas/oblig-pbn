#ifndef __SIG_H__
#define __SIG_H__

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

int signals_termination(void (*handle_sigchld)(int), void (*handle_exit)(void));

#endif
