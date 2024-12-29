#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>

#include "matrix_mult.h"

void init_matrix(double * matrix, int dim) {
    double k = 1.0;
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
	    matrix[i * dim + j] = k;
	    k++;
	}
    }
}

void multiply_chunk(const double * const a, const double * const b, double * const c, const int dim, int row_start, int chunk) {
    for (int i = row_start; i < row_start + chunk; i++) {
        for (int j = 0; j < dim; j++) {
	    for (int k = 0; k < dim; k++) {
	        c[i * dim + j] += a[i * dim + k] * b[k * dim + j];
	    }
	}
    }
}

void * task(void * arg) {
    Args * args = (Args *)arg;
    multiply_chunk(args->a, args->b, args->c, args->dim, args->row_start, args->chunk);
    return NULL;
}

void multiply_serial(const double * const a, const double * const b, double * const c, const int dim, const int num_workers) {
    multiply_chunk(a, b, c, dim, 0, dim); 
}

void multiply_parallel_processes(const double * const a, const double * const b, double * const c, const int dim, const int num_workers) {
    int num_procs = num_workers - 1;
    int chunk = dim / num_workers;
    int row_start;
    double * map = (double*)mmap(NULL, sizeof(double) * dim * dim, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, 0, 0);
    for (int i = 0; i < num_procs; i++) {
        pid_t pid = fork();
	if (!pid) {
	    row_start = i * chunk;
	    multiply_chunk(a, b, map, dim, row_start, chunk);
	    exit(EXIT_SUCCESS);
	}
    }
    multiply_chunk(a, b, map, dim, chunk * num_procs, chunk);
    while(wait(NULL) > 0);
    memcpy(c, map, sizeof(double) * dim * dim);
    munmap(map, sizeof(double) * dim * dim);
}

void multiply_parallel_threads(const double * const a, const double * const b, double * const c, const int dim, const int num_workers) {
    int chunk = dim / num_workers;
    int row_start = 0;
    Args args;
    args.a = a;
    args.b = b;
    args.c = c;
    args.dim = dim;
    args.chunk = chunk;
    args.row_start = 0;
    Args arg_set[num_workers];
    pthread_t threads[num_workers];
    for (int i = 0; i < num_workers; i++) {
        arg_set[i] = args;
        arg_set[i].row_start = row_start;
	row_start += chunk;
    }
    for (int i = 0; i < num_workers; i++) {
        pthread_create(&threads[i], NULL, task, &arg_set[i]);
    }
    for (int i = 0; i < num_workers; i++) {
        pthread_join(threads[i], NULL);
    }

}

void print_matrix(double * matrix, int dim) {
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
	    printf("%f ", matrix[i * dim + j]);
	}
	printf("\n");
    }
}

struct timeval time_diff(struct timeval * start, struct timeval * end) {
    struct timeval result;
    result.tv_sec = end->tv_sec - start->tv_sec;
    result.tv_usec = end->tv_usec - start->tv_usec;
    if (result.tv_usec < 0) {
        result.tv_usec += 1000000;
	result.tv_sec -= 1;
    }
    return result;
}

void print_elapsed_time(struct timeval * start, struct timeval * end) {
    struct timeval result = time_diff(start, end);
    printf("%ld second", result.tv_sec);
    if (result.tv_sec != 1) {
        printf("s");
    }
    printf(" and %ld microsecond", result.tv_usec);
    if (result.tv_usec != 1) {
        printf("s");
    }
    printf(".\n");
}   

int verify(const double * const m1, const double * const m2, const int dim) {
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
	    if (m1[i * dim + j] != m2[i * dim + j]) {
	        return FAILURE;
	    }
	}
    }
    return SUCCESS;
}

void printf_verification(const double * const m1, const double * const m2, const int dim, const char * const name) {
    printf("Verification for parallel %s: ", name);
    if (verify(m1, m2, dim) == SUCCESS) {
        printf("success.");
    } else {
        printf("failure.");
    }
    printf("\n");
}

void run_and_time(multiply_function multiply_matrices, const double * const a, const double * const b, double * const c, const double * const gold, const int dim, const char * const name, const int num_workers, const bool verify) {
    struct timeval start;
    struct timeval end;
    gettimeofday(&start, NULL);
    multiply_matrices(a, b, c, dim, num_workers);
    gettimeofday(&end, NULL);
    printf("Algorithm: %s with %d worker", name, num_workers);
    if (num_workers == 1) {
        printf(".\n");
    } else {
        printf("s.\n");
    }
    printf("Time elapsed for %s: ", name);
    print_elapsed_time(&start, &end);
    if (verify == true) {
        printf_verification(c, gold, dim, name);
    }
} 

