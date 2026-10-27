#ifndef IO_H
#define IO_H

#include <sys/types.h>
#include <stddef.h>

ssize_t read_full(int fd, void *buffer, size_t count);
ssize_t write_full(int fd,const void *buffer, size_t count);

#endif