#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **args) {
  printf("%s\n", args[0]);
  printf("%s\n", args[1]);

  int input_bytesize = 5000;
  int output_bytesize = 3125;
  float compression_ratio = input_bytesize / (output_bytesize / 1.0);
  float entropy = 4.12;
  float avg_code = 4.28;
  char integrity[] = "OK";

  printf("vstup: %d B, vystup: %d B\n", input_bytesize, output_bytesize);
  printf("kompresny pomer: %.2f\n", compression_ratio);
  printf("entropia: %.2f b/symbol, avg kod: %.2f b/symbol\n", entropy,
         avg_code);
  printf("integrita: %s\n", integrity);

  // malloc(4);

  return 0;
}
