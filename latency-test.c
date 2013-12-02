#define _GNU_SOURCE

#include<stdio.h>
#include<fcntl.h>
#include<time.h>
#include<string.h>
#include<malloc.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/mman.h>
#include<sys/time.h>

int main(int argc, char **argv)
{
	int fd, i;
	long long time, time1;
	unsigned long long FILE_SIZE;
	size_t len;
	off_t offset = 0;
	char c = 'a';
	char unit;
	struct timespec start, end;
	struct timeval begin, finish;
	struct timezone tz;
	int size = 4096;
	unsigned long long count;
	void *buf1 = NULL;
	char *buf;
	FILE *output;
	pthread_rwlock_t rw_lock;

	count = 1000000;
	pthread_rwlock_init(&rw_lock, NULL);

	gettimeofday(&begin, &tz);
	clock_gettime(CLOCK_MONOTONIC, &start);
	for (i = 0; i < count; i++) {
		pthread_rwlock_wrlock(&rw_lock);
		offset += size;
//		__sync_fetch_and_add(&offset, size);
		pthread_rwlock_unlock(&rw_lock);
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	gettimeofday(&finish, &tz);

	time = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
	time1 = (finish.tv_sec - begin.tv_sec) * 1e6 + (finish.tv_usec - begin.tv_usec);
	printf("%lld times,\t %lld nanoseconds,\t latency %lld nanoseconds.\n", count, time, time / count);
	printf("Write process %lld microseconds\n", time1);

	return 0;
}
