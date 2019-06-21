#include "keygen.h"

int main(int argc, char *argv[])
{ //all errors are handled at the bottom
  ErrorFlag error = NONE;
  long int q, p, e, x, y;
  long int privateKey, phi;

  srand((unsigned int)time(NULL)); //seed rng

  if(argc == 4)
  { //args: 1=p, 2=q, 3=e. such that p and q are primes and e is integer greater than 1 but less than (p-1)(q-1)
    p = atol(argv[1]);
    q = atol(argv[2]);
    e = atol(argv[3]);

    if(p != q && p < 2147483648 && q < 2147483648 && isPrime(p, PRIME_TESTS) && isPrime(q, PRIME_TESTS))
    { //1048576*n > 2 will exceed datatype limit. No possible prime combination will work without integer overflow issues.
      if(p > 65536 || q > 65536)
      { //65537^4 exceeds some datatype limits
        printf("Minor warning: Keys may be invalid due to integer overflow with large primes.\n");
      }

      phi = (p-1)*(q-1); //quick trick, only works if p and q are coprime.

      if(e > 1 && e < phi && gcdEx(e, phi, &x, &y) == 1) //if > 1 and < phi(p*q) and coprime to phi(p*q)
      {
        privateKey = x;

        while(privateKey < 0)
        { //correct for negative key
          privateKey += phi;
        }

        printf("Public key: %ld#%ld\n", p*q, e);
        printf("Private key: %ld#%ld\n", p*q, privateKey);
      }
      else
      {
        error = BAD_E;
      }
    }
    else
    {
      error = BAD_PRIMES;
    }
  }
  else
  {
    if(argc == 2)
    { //automatic/randomly generated mode
      if(!strcmp(argv[1], "-r"))
      {
        do
        {
          p = (rand() % (RAND_PRIME_MAX - RAND_PRIME_MIN)) + RAND_PRIME_MIN;
        }
        while(!isPrime(p, PRIME_TESTS));

        do
        {
          q = (rand() % (RAND_PRIME_MAX - RAND_PRIME_MIN)) + RAND_PRIME_MIN;
        }
        while(!isPrime(q, PRIME_TESTS) || p == q);

        phi = (p-1)*(q-1); //quick trick, only works if p and q are coprime.

        do
        {
          e = (rand() % (E_MAX - 1)) + 1;
        }
        while(e <= 1 || e >= phi || gcdEx(e, phi, &x, &y) != 1);

        privateKey = x;

        while(privateKey < 0)
        { //correct for negative key
          privateKey += phi;
        }

        printf("p:%ld, q:%ld, e:%ld\n", p, q, e);
        printf("Public key: %ld#%ld\n", p*q, e);
        printf("Private key: %ld#%ld\n", p*q, privateKey);
      }
    }
    else
    {
      error = WRONG_ARGS;
    }
  }

  switch(error)
  {
    case NONE:
      break;
    case BAD_PRIMES: fprintf(stderr, "Invalid first and/or second argument. Both must be distinct prime numbers, and due to risks of  integer overflow; less than 2147483648.\n");
      break;
    case BAD_E: fprintf(stderr, "Invalid third argument. Must be an integer greater than one but less than and co-prime with phi of the two primes multiplied.\n");
      break;
    case WRONG_ARGS: fprintf(stderr, "Invalid arguments.\nExpecting -r flag or form: ./keygen p q e, where p and q are primes, and e such that it is an integer greater than one but less than and coprime with phi(p*q).\n");
      break;
  }
}

/*
int isPrime(long int p)
{ //deterministic primality test ; slow
  long int i, cap;
  int prime = 1; //assumed to be true until proven otherwise

  if(p <= 2)
  {
    prime = 0;
  }
  else
  {
    cap = sqrtl(p); //calculate once

    for(i = 2; i < cap; i++)
    {
      if((p % i) == 0)
      {
        prime = 0;
      }
    }
  }

  return prime;
}*/

int isPrime(long int num, int checks)
{ //probability that this is wrong is 1/(2^checks)
  int i;
  long int val, randNum;
  int alwaysOne = 1;
  int prime = 1;
  long int exp = (num - 1)/2;

  i = 1;
  while(i <= checks && prime)
  {
    randNum = (rand() % (num - 1)) + 1;
    val = (int)modPow(randNum, exp, num);

    if(val == num -1)
    {
      alwaysOne = 0;
    }
    else
    {
      if(val != 1)
      {
        prime = 0;
      }
    }

    i++;
  }

  return (prime && !alwaysOne);
}

/*unsigned long int modPow(unsigned int num, long int exp, long int mod) //naive approach
{ //only works for positive numbers. mods as it multiplies - steps combined so as not to exceed datatype limits
  long int i;
  unsigned long int result = 1;

  for(i = 0; i < exp; i++)
  {
    result *= num;
    result %= mod;
  }

  return result;
}*/

unsigned long int modPow(unsigned int num, long int exp, long int mod) //optimized solution
{ //only works for positive numbers. mods as it multiplies - steps combined so as not to exceed datatype limits
  long int i = 0;
  unsigned long int result = 1;

  while((exp >> i) > 0)
  { //find number of bits in exp
    i++;
  }

  while(i >= 0)
  {
    result = (result * result) % mod;
    if((exp >> i) & 0b1) //if bit at offset i is 1
    {
      result = (result * num) % mod;
    }

    i--;
  }

  return result;
}

long int gcdEx(long int a, long int b, long int *x, long int *y)
{ //extended euclidean implementation
  long int val1, val2, ret;

  if(a == 0)
  {
    *x = 0;
    *y = 1;
    ret = b;
  }
  else
  {
    ret = gcdEx((b % a), a, &val1, &val2);
    *x = val2 - (b / a) * val1;
    *y = val1;
  }

  return ret;
}
