#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef PATH_MAX
// we define PATH_MAX here, because we always perform checks prior to using it, value doesn't need to be in sync with the OS
#define PATH_MAX 4096
#endif

// this simple program will read from stdin the arguments for execve and run it

char lookup_path_buffer[PATH_MAX+1];

const char *lookup_path(const char *exe) {
	// lookup for given exe in PATH
	if (strchr(exe, '/') != NULL) {
		// if path contains a / anywhere, do not perform lookup
		return exe;
	}

	const char *searchpath = getenv("PATH");
	if (searchpath == NULL) return exe;
	if (strlen(searchpath) <= 0) return exe;

	int len;
	int exelen = strlen(exe);
	const char *beg, *end;
	beg = searchpath;

	do {
		end = strchr(beg, ':');
		if (end == NULL) {
			strncpy(lookup_path_buffer, beg, PATH_MAX);
			len = strlen(lookup_path_buffer);
			beg = NULL;
		} else {
			len = end - beg;
			if (len > PATH_MAX) {
				// can't handle that long of a path
				beg = end+1;
				continue;
			}
			memcpy(lookup_path_buffer, beg, len);
			beg = end+1;
		}

		if (lookup_path_buffer[len-1] != '/') {
			lookup_path_buffer[len] = '/';
			len += 1;
		}
		if (len+exelen > PATH_MAX) {
			// can't handle this
			continue;
		}
		memcpy(lookup_path_buffer+len, exe, exelen);
		len += exelen;
		lookup_path_buffer[len] = 0; // ensure it ends with a NULL

		int res = access(lookup_path_buffer, X_OK);
		if (res == 0) return lookup_path_buffer;
	} while(end != NULL);

	// not found?
	return exe;
}

// readfull reads
ssize_t readfull(int fd, void *buf, size_t count) {
	size_t total = 0;

	do {
		ssize_t v = read(fd, buf, count);
		if (v == -1) return -1; // failed
		if (v == 0) return total; // EOF?
		total += v;
		buf += v;
		count -= v;
	} while(count > 0);
	return total;
}

int main(int argc, char *argv[]) {
	unsigned char datalen[4];
	// ssize_t read(int fd, void *buf, size_t count);
	ssize_t rdln = readfull(0, &datalen, 4);
	if (rdln != 4) {
		fprintf(stderr, "%s: invalid packet length read %ld\n", argv[0], rdln);
		return 101; // invalid read
	}
	uint32_t dataln = (uint32_t)(datalen[0]) << 24 | (uint32_t)(datalen[1]) << 16 | (uint32_t)(datalen[2]) << 8 | (uint32_t)(datalen[3]);

	if ((dataln > 128*1024) || (dataln < 4)) {
		fprintf(stderr, "%s: invalid packet read %u\n", argv[0], dataln);
		return 102; // too small or large
	}

	char *buf = malloc(dataln);
	rdln = readfull(0, buf, dataln);
	if (rdln != dataln) {
		fprintf(stderr, "%s: invalid data read %ld\n", argv[0], rdln);
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
	const char *pathname = buf;
	if (pos == 0) {
		// if pathname len is zero, set it to NULL so we look it up later
		pathname = NULL;
	}

	buf += pos+1;
	dataln -= pos+1;

	char **newargv = calloc(newargc+1, sizeof(char*));
	char **newenvp = calloc(newenvc+1, sizeof(char*));

	for(int i = 0; i < newargc; i++) {
		if (dataln <= 0) {
			fprintf(stderr, "%s: invalid data while reading argv\n", argv[0]);
			return 104; // should not happen
		}
		pos = strnlen(buf, dataln);
		newargv[i] = buf;
		buf += pos+1;
		dataln -= pos+1;
	}

	int n = 0;
	char *evar;

	for(int i = 0; i < newenvc; i++) {
		if (dataln <= 0) {
			fprintf(stderr, "%s: invalid data while reading envp\n", argv[0]);
			return 105; // should not happen
		}
		evar = buf;
		pos = strnlen(buf, dataln);
		buf += pos+1;
		dataln -= pos+1;

		if (strncmp(evar, "PWD=", 4) == 0) {
			// chdir, do not add to env
			if (chdir(evar+4) != 0) {
				perror("chdir");
				return 106;
			}
			continue;
		}

		newenvp[n++] = evar;
	}
	if (dataln != 0) {
		fprintf(stderr, "%s: invalid data: trailing data\n", argv[0]);
		return 107; // should have consumed everything
	}

	if ((pathname == NULL) && (newargc > 0)) {
		pathname = lookup_path(newargv[0]);
	}

	execve(pathname, newargv, newenvp);

	fprintf(stderr, "%s: execve() failed\n", argv[0]);
	return 106; // execve failed
}
