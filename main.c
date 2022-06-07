#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

// this simple program will read from stdin the arguments for execve and run it

int main(int argc, char *argv[]) {
	char datalen[4];
	// ssize_t read(int fd, void *buf, size_t count);
	ssize_t rdln = read(0, &datalen, 4);
	if (rdln != 4) {
		return 101; // invalid read
	}
	uint32_t dataln = (uint32_t)(datalen[0]) << 24 | (uint32_t)(datalen[1]) << 16 | (uint32_t)(datalen[2]) << 8 | (uint32_t)(datalen[3]);

	if ((dataln > 128*1024) || (dataln < 4)) {
		return 102; // too small or large
	}

	char *buf = malloc(dataln);
	rdln = read(0, buf, dataln);
	if (rdln != dataln) {
		return 103; // invalid read
	}

	int newargc = buf[0];
	int newenvc = buf[1];

	buf += 2;
	dataln -= 2;

	// read arguments for execve (separated by NULL chars)
	//
	// int execve(const char *pathname, char *const argv[], char *const envp[]);
	int pos = strnlen(buf, dataln);
	char *pathname = buf;

	buf += pos+1;
	dataln -= pos+1;

	char **newargv = calloc(newargc+1, sizeof(char*));
	char **newenvp = calloc(newenvc+1, sizeof(char*));

	for(int i = 0; i < newargc; i++) {
		if (dataln <= 0) {
			return 104; // should not happen
		}
		pos = strnlen(buf, dataln);
		newargv[i] = buf;
		buf += pos+1;
		dataln -= pos+1;
	}
	for(int i = 0; i < newenvc; i++) {
		if (dataln <= 0) {
			return 105; // should not happen
		}
		pos = strnlen(buf, dataln);
		newenvp[i] = buf;
		buf += pos+1;
		dataln -= pos+1;
	}
	if (dataln != 0) {
		return 106; // should have consumed everything
	}

	execve(pathname, newargv, newenvp);

	return 106; // execve failed
}
