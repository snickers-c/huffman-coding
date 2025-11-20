#include <stdio.h>
#include <stdlib.h>

// typedef struct {
//   int cnt;  // count
//   char sbl; // symbol
// } keyval;

// keyval *symbol = malloc(sizeof(keyval) * symbol_size);

// zapis hodnôť
// symbol[2].s = 'a';
// symbol[2].c = 5;
// for (int i = 0; i < 25; i++) {
//   symbol[i].c = i;
//   symbol[i].s = 65 + i;
// }

// vypis hodnôt
// for (int i = 0; i < 25; i++) {
// printf("%c - %d\n", symbol[0].sbl, symbol[0].cnt);
// }

// vypis adries symbol
//  for (int i = 0; i < 25; i++) {
//    printf("%p\n", (void *)&symbol[i]);
//  }

// if (symbol[0].cnt == 0) {
//   printf("ja symbol som nulovy");
// }

int main(int argc, char *argv[]) {
  int symbol_size = 52;
  int input_size = 255;

  char a = ' ';
  printf("%c\n", a);
  printf("%d\n", (int)a);

  char *input = (char *)malloc(sizeof(char) * input_size);
  if (input == NULL) {
    printf("Mem alloc failed!");
    return -1;
  }

  int **symbol = (int **)malloc(sizeof(int *) * symbol_size);
  if (symbol == NULL) {
    printf("Mem alloc failed!");
    return -1;
  }

  for (int i = 0; i < symbol_size; i++) {
    symbol[i] = (int *)malloc(sizeof(int) * 2);

    if (symbol[i] == NULL) {
      printf("Mem alloc failed!");
      return -1;
    }

    symbol[i][1] = 0;
  }

  fgets(input, input_size, stdin);

  for (int i = 0; input[i] != '\0' && input[i] != '\n'; i++) {
    for (int j = 0; j < symbol_size; j++) {
      if (symbol[j][0] == (int)input[i] || symbol[j][1] == 0) {
        symbol[j][0] = (int)input[i];
        symbol[j][1]++;
        break;
      }
    }
  }

  for (int i = 0; i < symbol_size; i++) {
    if (symbol[i][1] == 0)
      break;
    printf("%c - count:%d\n", symbol[i][0], symbol[i][1]);
  }

  free(input);
  input = NULL;

  // prepared final output
  // int input_bytesize = 5000;
  // int output_bytesize = 3125;
  // float compression_ratio = input_bytesize / (output_bytesize / 1.0);
  // float entropy = 4.12;
  // float avg_code = 4.28;
  // char integrity[] = "OK";

  // printf("vstup: %d B, vystup: %d B\n", input_bytesize, output_bytesize);
  // printf("kompresny pomer: %.2f\n", compression_ratio);
  // printf("entropia: %.2f b/symbol, avg kod: %.2f b/symbol\n", entropy,
  //        avg_code);
  // printf("integrita: %s\n", integrity);

  return 0;
}
