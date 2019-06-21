#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define PRIME_TESTS 20 //number of tests done against primes
#define RAND_PRIME_MAX 10000
#define RAND_PRIME_MIN 1000
#define E_MAX 2147483648

typedef enum ErrorFlag {NONE, BAD_PRIMES, BAD_E, WRONG_ARGS} ErrorFlag;

long int gcdEx(long int a, long int b, long int *x, long int *y);

//int isPrime(long int p);

int isPrime(long int num, int checks);

unsigned long int modPow(unsigned int num, long int exp, long int mod);
