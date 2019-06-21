#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <string.h>

#define BLOCK_SIZE 2 //max is 8. probably won't work above that unless you increase long ints to long longs

typedef enum ErrorFlag {NONE, BAD_KEY, SMALL_KEY, BAD_FLAG, BAD_OUTPUT, BAD_INPUT, WRONG_ARGS} ErrorFlag;

void encrypt(FILE *input, FILE *output, long int n, long int k);

void decrypt(FILE *input, FILE *output, long int n, long int k);

unsigned long int modPow(unsigned int num, unsigned long int exp, unsigned long int mod);
