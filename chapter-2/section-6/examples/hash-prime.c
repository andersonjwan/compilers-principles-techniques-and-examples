#include <math.h>

#include "hash-prime.h"

int is_prime(const int num) {
  if(num < 2) {
    return -1;
  }

  if(num < 4) {
    return 1;
  }

  if((num % 2) == 0) {
    return 0;
  }

  for(int i = 3; i <= floor(sqrt((double) num)); i += 2) {
    if((num % i) == 0) {
      return 0;
    }
  }

  return 1;
}

int next_prime(int num) {
  while(is_prime(num) != 1) {
    ++num;
  }

  return num;
}
