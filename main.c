#include "huffman.h"

int main(int argc, char *argv[]) {
  if (argc == 4) {
    if (strcmp(argv[1], "-c") == 0) {
      char *rfile = argv[2];
      char *wfile = argv[3];

      FILE *file = fopen(rfile, "rb");
      if (file == NULL) {
        printf("File open failed!");
        return -2;
      }

      int frequency_arr[CHARNUM] = {0};
      int symbol;
      int total_symbols = 0;
      while ((symbol = fgetc(file)) != EOF) {
        total_symbols++;
        frequency_arr[(unsigned char)symbol]++;
      }

      fclose(file);

      int capacity = 0;
      for (int i = 0; i < CHARNUM; i++) {
        if (frequency_arr[i] > 0) {
          capacity++;
          // printf("%d '%c'\n", frequency_arr[i], i);
        }
      }
      // printf("dokoncil som pocitanie znakov\n");

      Symbol_Frequency *symbols =
          (Symbol_Frequency *)malloc(capacity * sizeof(Symbol_Frequency));

      int index = 0;
      for (int i = 0; i < CHARNUM; i++) {
        if (frequency_arr[i] > 0) {
          symbols[index].symbol = (unsigned char)i;
          symbols[index].frequency = frequency_arr[i];
          index++;
          // printf("pridal som znak do symbols structu\n");
        }
      }

      Node *root = huffman_tree(symbols, capacity);
      // printf("dokoncil som huffmanov strom\n");

      Code codes[CHARNUM];
      int code_count = 0;
      int path[CHARNUM];

      generate_codes(root, path, 0, codes, &code_count);

      // printf("dokoncil som kody\n");

      // kontrola kodov
      // for (int j = 0; j < code_count; j++) {
      //   printf("\n'%c'", codes[j].symbol);

      //   for (int i = 0; i < codes[j].length; i++)
      //     printf("%d", codes[j].bits[i]);
      // }

      FILE *rfile_b = fopen(rfile, "rb");
      if (rfile_b == NULL) {
        printf("File open failed!");
        return -2;
      }

      fseek(rfile_b, 0, SEEK_END);
      size_t input_size = ftell(rfile_b);
      fseek(rfile_b, 0, SEEK_SET);

      unsigned char *input = malloc(input_size);
      if (input == NULL) {
        printf("Mem alloc failed!");
        return -1;
      }

      fread(input, 1, input_size, rfile_b);
      fclose(rfile_b);

      FILE *output_file = fopen(wfile, "wb");
      if (output_file == NULL) {
        printf("File open failed!");
        return -2;
      }

      size_t compressed_size =
          get_compressed_size(input, input_size, codes, code_count);
      unsigned int checksum =
          get_checksum(input, input_size, codes, code_count);

      int flag = 0;
      file_header(output_file, input_size, compressed_size, checksum, flag);

      unsigned char byte_buffer = 0;
      int bits_filled = 0;

      file_describe_huftree(output_file, root, &byte_buffer, &bits_filled);

      if (bits_filled > 0) {
        byte_buffer <<= (8 - bits_filled);
        fputc(byte_buffer, output_file);
        bits_filled = 0;
        byte_buffer = 0;
      }

      file_compressed_data(output_file, input, input_size, codes, code_count);

      fclose(output_file);

      free(input);
      free(symbols);

      printf("\nKompresia dokoncena: checksum: %u\n", checksum);

      float compression_ratio = input_size / (compressed_size / 1.0);
      float entropy = get_entropy(frequency_arr, total_symbols);
      float avg_code =
          get_avg_code(codes, code_count, frequency_arr, total_symbols);
      char integrity[] = "OK";

      printf("vstup: %d B, vystup: %d B\n", input_size, compressed_size);
      printf("kompresny pomer: %.2f\n", compression_ratio);
      printf("entropia: %.2f b/symbol, avg kod: %.2f, b / symbol\n", entropy,
             avg_code);
      printf("integrita: %s\n", integrity);

    } else if (strcmp(argv[1], "-d") == 0) {
      char *rfile = argv[2];
      char *wfile = argv[3];

      FILE *file = fopen(rfile, "rb");
      if (file == NULL) {
        printf("File open failed!");
        return -2;
      }

      int version, flag;
      size_t original_size, compressed_size;
      unsigned int checksum;

      if (!validate_header(file, &version, &flag, &original_size,
                           &compressed_size, &checksum)) {
        printf("Subor nie je validny HUF format.\n");

        return -5;
      }

      unsigned char byte_buffer = 0;
      int bits_left = 0;

      Node *root = load_huffman_tree(file, &byte_buffer, &bits_left);
      if (!root) {
        printf("chyba pri nacitani stromu\n");
        fclose(file);
        return -4;
      }

      if (bits_left > 0) {
        bits_left = 0;
        byte_buffer = 0;
      }

      FILE *output_file = fopen(wfile, "wb");
      if (output_file == NULL) {
        printf("File open failed!");
        return -2;
      }

      decompress_data(file, output_file, root, original_size, &byte_buffer,
                      &bits_left);

      fclose(output_file);
      fclose(file);
    }
  } else if (argc == 3) {
    if (strcmp(argv[1], "-i") == 0) {
      char *rfile = argv[2];

      FILE *file = fopen(rfile, "rb");
      if (file == NULL) {
        printf("File open failed!");
        return -2;
      }

      file_integrity(file);

      fclose(file);
    }
  } else {
    printf("pouzitie: hcd -c|-d|-i <subor>.txt <vystupny_subor>.huf\n");
    return -4;
  }

  return 0;
}
