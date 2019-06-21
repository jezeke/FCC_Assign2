#include "rsa.h"

int main(int argc, char *argv[])
{ //all errors are handled at the bottom
  ErrorFlag error = NONE;
  FILE *input = NULL;
  FILE *output = NULL;
  long int n, k;

  if(argc == 5) //no args are optional
  { //args: 1= -e|-d, 2=input filename, 3=output filename, 4=key
    input = fopen(argv[2], "r");
    if(input != NULL)
    {
      output = fopen(argv[3], "w");
      if(output != NULL)
      {
        if(sscanf(argv[4], "%ld#%ld", &n, &k) == 2 && k > 1)
        {
          if(n > (long int)pow(2, BLOCK_SIZE*8))
          {
            if(!strcmp(argv[1], "-d"))
            {
              decrypt(input, output, n, k);
            }
            else
            {
              if(!strcmp(argv[1], "-e"))
              {
                encrypt(input, output, n, k);
              }
              else
              {
                error = BAD_FLAG;
              }
            }
          }
          else
          {
            error = SMALL_KEY;
          }
        }
        else
        {
          error = BAD_KEY;
        }
      }
      else
      {
        error = BAD_OUTPUT;
      }
    }
    else
    {
      error = BAD_INPUT;
    }
  }
  else
  {
    error = WRONG_ARGS;
  }

  switch(error)
  {
    case NONE:
      fclose(input);
      fclose(output);
      break;
    case BAD_KEY: fprintf(stderr, "Invalid key.\nExpected public key in form \"n#k\" with k the public key for encryption or the private key for decryption.\nRun keygen to generate keys if required.\n");
      fclose(input);
      fclose(output);
      break;
    case SMALL_KEY: fprintf(stderr, "Invalid key.\nKey not large enough for set block size. Recompile with a smaller block size or generate a key with larger primes.\n");
      fclose(input);
      fclose(output);
      break;
    case BAD_FLAG: fprintf(stderr, "Invalid flag.\nExpecting first argument to be -e (for encrypt) or -d (for decrypt).");
      break;
    case BAD_INPUT: perror("Unable to load input file:");
      break;
    case BAD_OUTPUT: perror("Unable to open output file:");
      fclose(input);
      break;
    case WRONG_ARGS: fprintf(stderr, "Invalid arguments.\nExpecting form: rsa -e|-d \"/path/to the/input\" \"/path/to the/output\" \"key\"\nGenerate keys with keygen.\n");
      break;
  }
}

void encrypt(FILE *input, FILE *output, long int n, long int k)
{
  char next;
  unsigned long int buffer = 0;
  int loading = 0; //number of bytes loaded

  while(fread(&next, 1, 1, input) != 0) //performance bottleneck
  {
    buffer = buffer | ((unsigned long int)next << (loading * 8));

    loading++;
    if(loading == BLOCK_SIZE)
    { //proceed if blocksize loaded
      buffer = modPow(buffer, k, n);

      fprintf(output, "%lu,", buffer); //print as double, delimitied by commas

      buffer = 0;
      loading = 0;
    }
  }

  if(loading != 0)
  { //if was in middle of block when ran out of input data, print remainder
    buffer = modPow(buffer, k, n);

    fprintf(output, "%lu,", buffer);
  }
}

void decrypt(FILE *input, FILE *output, long int n, long int k)
{
  unsigned long int next;
  char print;
  int i;

  while(fscanf(input, "%lu,", &next) != EOF)
  {
    next = modPow(next, k, n);
    for(i = 0; i < BLOCK_SIZE; i++)
    { //unroll block into chars again
      print = (char)((next >> 8 * i) & 0b11111111);

      if(print != 0)
      { //if block is partial, don't print null characters
        fprintf(output, "%c", print);
      }
    }
  }
}

/*
unsigned long int modPow(unsigned long int num, long int exp, long int mod) //naive solution
{ //only works for positive numbers. mods as it multiplies - steps combined so as not to exceed datatype limits
  long int i;
  unsigned long long int result = 1;

  for(i = 0; i < exp; i++)
  {
    result *= num;
    result %= mod;
  }

  return (unsigned long int)result;
}*/

unsigned long int modPow(unsigned int num, unsigned long int exp, unsigned long int mod) //optimized solution
{ //only works for positive numbers. Fast Modular Exponentiation Algorithm
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
