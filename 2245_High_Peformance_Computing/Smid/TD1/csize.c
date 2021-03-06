#include <inttypes.h>
#include <stdio.h>

int main() {
  printf( "    short int: %zd\n" , sizeof(short int) ) ;
  printf( "          int: %zd\n" , sizeof(int) ) ;
  printf( "        float: %zd\n", sizeof(float) ) ;
  printf( "       double: %zd\n", sizeof(double) ) ;
  printf( "      int64_t: %zd\n", sizeof(int64_t) ) ;
  printf( "     long int: %zd\n", sizeof(long int) ) ;
  printf( "long long int: %zd\n", sizeof(long long int) ) ;
  printf( "       size_t: %zd\n", sizeof(size_t) ) ;
  printf( "        void*: %zd\n\n", sizeof(void *) ) ;
  printf( "         char: %zd\n\n", sizeof(char) ) ;
  return 0;
}
