/*
** EPITECH PROJECT, 2021
** libtry
** File description:
** Created by kraken,
*/

#include <stdio.h>
#include "try.h"

jmp_buf * __LIBTRY__raise_point = NULL;
jmp_buf * __LIBTRY__retry_point = NULL;

void * rmalloc(size_t const nbytes)
{
	void * const mem = malloc(nbytes);
	if (mem == NULL) {
		raise(MALLOC_EXCEPTION);
	}
	return mem;
}

void libtry_handle_signal(int signum, void (*handler)(int, siginfo_t*, void*))
{
	struct sigaction sa;

	memset(&sa, 0, sizeof(sigaction));
	sigemptyset(&sa.sa_mask);
	sa.sa_flags     = SA_NODEFER;
//	sa.sa_sigaction = LAMBDA(void, (int sig, siginfo_t* info, void* prt) {raise(err_code);});
	sa.sa_sigaction = handler;
	sigaction(signum, &sa, NULL); /* ignore whether it works or not */
}



void test_func(void) {
	raise(SEGFAULT_EXCEPTION);
}


int main(void) {

	char *str = "ewqfdew";

	libtry_handle_signal(SIGSEGV, handler(SEGFAULT_EXCEPTION));
	try (
		str[0] = 0;
	) catch ( exception == SEGFAULT_EXCEPTION,
		printf("segfault error !!!\n");
	) end;
}