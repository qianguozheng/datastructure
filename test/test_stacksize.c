#include <stdio.h> 
 
long int count = 0;
void func(int a, int b) {
  char msg[1024];
  printf("count: %ld\n", count++);
  func(count, count);
}
 
int main(void) 
{
  int a = 0, b = 0;
  func(a, b);
  return 0;
}
