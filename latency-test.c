#define _GNU_SOURCE

#include<stdio.h>
#include<fcntl.h>
#include<time.h>
#include<string.h>
#include<malloc.h>
#include<stdlib.h>
#include<stdint.h>
#include<pthread.h>
#include<sys/mman.h>
#include<sys/time.h>

#define cpuid(func,ax,bx,cx,dx)\
        __asm__ __volatile__ ("cpuid":\
        "=a" (ax), "=b" (bx), "=c" (cx), "=d" (dx) : "a" (func));

static inline int get_cpuid(void)
{
	int id;
	uint32_t eax = 0;
	uint32_t ebx = 0;
	uint32_t ecx = 1;
	uint32_t edx = 0;

	cpuid(0x0B, eax, ebx, ecx, edx);

	id = (((edx & 1) << 3) + ((edx >> 4) & 1) + (edx & 0xe));

	return id;
}

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
	uint64_t lock[16] = {0};
	uint64_t wr_lock = 0;
	int cpu_id;
	int j, lock_held;

	count = 1000000;
	pthread_rwlock_init(&rw_lock, NULL);
	cpu_id = get_cpuid();
	printf("cpuid: %d\n", cpu_id);

	gettimeofday(&begin, &tz);
	clock_gettime(CLOCK_MONOTONIC, &start);
	for (i = 0; i < count; i++) {
		while(__sync_fetch_and_add(&lock[cpu_id], 1) >= (1 << 31))
			__sync_fetch_and_sub(&lock[cpu_id], 1);

		__sync_fetch_and_sub(&lock[cpu_id], 1);
#if 0
		for (j = 0; j < 16; j++) {		
			while (!__sync_bool_compare_and_swap(&lock[j], 0, 1 << 31))
				;
		}

		for (j = 0; j < 16; j++) {		
			__sync_fetch_and_sub(&lock[j], 1 << 31);
		}
#endif
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	gettimeofday(&finish, &tz);

	time = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
	time1 = (finish.tv_sec - begin.tv_sec) * 1e6 + (finish.tv_usec - begin.tv_usec);
	printf("%lld times,\t %lld nanoseconds,\t latency %lld nanoseconds.\n", count, time, time / count);
	printf("Write process %lld microseconds\n", time1);
	printf("wr_lock: %lld, rw_lock: %lld\n", wr_lock, lock[0]);

	return 0;
}
