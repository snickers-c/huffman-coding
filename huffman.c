#include "huffman.h"
#include <math.h>

Node *new_node(unsigned char symbol, int frequency) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->symbol = symbol;
  node->frequency = frequency;
  node->left = NULL;
  node->right = NULL;

  return node;
}

Minheap *new_minheap(int capacity) {
  Minheap *minheap = (Minheap *)malloc(sizeof(Minheap));
  minheap->size = 0;
  minheap->capacity = capacity;
  minheap->array = (Node **)malloc(capacity * sizeof(Node *));
  return minheap;
}

void swap_nodes(Node **lesser, Node **greater) {
  Node *tmp = *lesser;
  *lesser = *greater;
  *greater = tmp;
}

void insert_node_to_minheap(Minheap *minheap, Node *node) {
  int i = minheap->size;
  minheap->array[i] = node;
  minheap->size++;

  while (i > 0) {
    if (minheap->array[i]->frequency < minheap->array[(i - 1) / 2]->frequency) {
      swap_nodes(&minheap->array[i], &minheap->array[(i - 1) / 2]);
      i = (i - 1) / 2;
    } else {
      break;
    }
  }
}

Node *push_root_node(Minheap *minheap) {
  Node *root = minheap->array[0];
  minheap->array[0] = minheap->array[minheap->size - 1];
  minheap->size--;

  int i = 0;
  while (i < minheap->size) {
    int min = i;
    int lc_index = 2 * i + 1;
    int rc_index = 2 * i + 2;

    if (lc_index < minheap->size &&
        minheap->array[lc_index]->frequency < minheap->array[min]->frequency) {
      min = lc_index;
    }

    if (rc_index < minheap->size &&
        minheap->array[rc_index]->frequency < minheap->array[min]->frequency) {
      min = rc_index;
    }

    if (min != i) {
      swap_nodes(&minheap->array[min], &minheap->array[i]);
      i = min;
    } else {
      break;
    }
  }

  return root;
}

Node *huffman_tree(Symbol_Frequency *symbols, int capacity) {
  Minheap *minheap = new_minheap(capacity);

  for (int i = 0; i < capacity; i++) {
    insert_node_to_minheap(minheap,
                           new_node(symbols[i].symbol, symbols[i].frequency));
    // printf("pridal som node do minheapu\n");
  }

  while (minheap->size > 1) {
    // printf("zacal som spracovavat parenta\n");
    Node *left = push_root_node(minheap);
    Node *right = push_root_node(minheap);

    Node *parent = new_node(0, left->frequency + right->frequency);
    parent->left = left;
    parent->right = right;

    insert_node_to_minheap(minheap, parent);
    // printf("pridal som parent node do minheapu\n");
  }

  return push_root_node(minheap);
}

Node *load_huffman_tree(FILE *file, unsigned char *byte_buffer,
                        int *bits_left) {
  if (*bits_left == 0) {
    int c = fgetc(file);
    if (c == EOF)
      return NULL;
    *byte_buffer = (unsigned char)c;
    *bits_left = 8;
  }

  int bit = (*byte_buffer & 0x80) != 0;
  // printf("%d", bit);
  *byte_buffer <<= 1;
  (*bits_left)--;

  if (bit) {
    unsigned char symbol = 0;
    for (int i = 0; i < 8; i++) {
      if (*bits_left == 0) {
        *byte_buffer = fgetc(file);
        *bits_left = 8;
      }
      symbol <<= 1;
      symbol |= (*byte_buffer & 0x80) ? 1 : 0;
      *byte_buffer <<= 1;
      (*bits_left)--;
    }
    return new_node(symbol, 0);
  } else {
    Node *node = new_node(0, 0);
    node->left = load_huffman_tree(file, byte_buffer, bits_left);
    node->right = load_huffman_tree(file, byte_buffer, bits_left);
    return node;
  }
}

void generate_codes(Node *root, int *path, int depth, Code *codes,
                    int *code_count) {
  if (!root)
    return;

  if (root->left == NULL && root->right == NULL) {
    codes[*code_count].symbol = root->symbol;
    codes[*code_count].length = depth;

    for (int i = 0; i < depth; i++) {
      codes[*code_count].bits[i] = path[i];
    }

    (*code_count)++;
    return;
  }

  path[depth] = 0;
  generate_codes(root->left, path, depth + 1, codes, code_count);

  path[depth] = 1;
  generate_codes(root->right, path, depth + 1, codes, code_count);
}

void write_bit(FILE *out, int bit, unsigned char *byte_buffer,
               int *bits_filled) {

  // printf("%d", bit);
  *byte_buffer = (*byte_buffer << 1) | (bit & 1);
  (*bits_filled)++;

  if (*bits_filled == 8) {
    fputc(*byte_buffer, out);
    *bits_filled = 0;
    *byte_buffer = 0;
  }
}

void file_describe_huftree(FILE *out, Node *root, unsigned char *byte_buffer,
                           int *bits_filled) {
  if (!root)
    return;

  if (root->left == NULL && root->right == NULL) {
    write_bit(out, 1, byte_buffer, bits_filled);

    for (int i = 0; i < 8; i++) {
      int bit = (root->symbol >> (7 - i)) & 1;
      write_bit(out, bit, byte_buffer, bits_filled);
    }

    return;
  }

  write_bit(out, 0, byte_buffer, bits_filled);

  file_describe_huftree(out, root->left, byte_buffer, bits_filled);
  file_describe_huftree(out, root->right, byte_buffer, bits_filled);
}

void file_compressed_data(FILE *out, unsigned char *input, size_t input_size,
                          Code *codes, int code_count) {
  unsigned char byte_buffer = 0;
  int bits_filled = 0;

  for (int i = 0; i < input_size; i++) {
    unsigned char c = input[i];

    for (int j = 0; j < code_count; j++) {
      if (codes[j].symbol == c) {
        for (int k = 0; k < codes[j].length; k++) {
          write_bit(out, codes[j].bits[k], &byte_buffer, &bits_filled);
        }
        break;
      }
    }
  }

  if (bits_filled > 0) {
    byte_buffer <<= (8 - bits_filled);
    fputc(byte_buffer, out);
  }
}

void decompress_data(FILE *file, FILE *out, Node *root, size_t original_size,
                     unsigned char *byte_buffer, int *bits_left) {
  Node *node = root;
  size_t written = 0;

  while (written < original_size) {
    if (*bits_left == 0) {
      int c = fgetc(file);
      if (c == EOF)
        break;
      *byte_buffer = (unsigned char)c;
      *bits_left = 8;
    }

    if ((*byte_buffer & 0x80) != 0) {
      node = node->right;
    } else {
      node = node->left;
    }

    *byte_buffer <<= 1;
    (*bits_left)--;

    if (node->left == NULL && node->right == NULL) {
      // printf("%c", node->symbol);
      fputc(node->symbol, out);
      written++;
      node = root;
    }
  }
}

void file_header(FILE *out, size_t old_size, size_t new_size,
                 unsigned int checksum, int flag) {
  fwrite("HUF1", 1, 4, out);
  fputc(VERSION, out);
  fputc(flag, out);
  for (int i = 0; i < 2; i++) {
    fputc(0, out);
  }

  for (int i = 0; i < 8; i++) {
    fputc((old_size >> (i * 8)) & 0xFF, out);
  }

  for (int i = 0; i < 8; i++) {
    fputc((new_size >> (i * 8)) & 0xFF, out);
  }

  for (int i = 0; i < 4; i++) {
    fputc((checksum >> (i * 8)) & 0xFF, out);
  }

  for (int i = 0; i < 4; i++) {
    fputc(0, out);
  }
}

size_t get_compressed_size(unsigned char *input, size_t input_size, Code *codes,
                           int code_count) {
  int bits_count = 0;

  for (size_t i = 0; i < input_size; i++) {
    unsigned char c = input[i];

    for (int j = 0; j < code_count; j++) {
      if (codes[j].symbol == c) {
        bits_count += codes[j].length;
        break;
      }
    }
  }

  return (bits_count + 7) / 8;
}

unsigned int get_checksum(unsigned char *input, size_t input_size, Code *codes,
                          int code_count) {
  unsigned char byte_buffer = 0;
  int bits_filled = 0;
  unsigned int checksum = 0;

  for (size_t i = 0; i < input_size; i++) {
    unsigned char c = input[i];
    for (int j = 0; j < code_count; j++) {
      if (codes[j].symbol == c) {
        for (int k = 0; k < codes[j].length; k++) {
          byte_buffer = (byte_buffer << 1) | codes[j].bits[k];
          bits_filled++;

          if (bits_filled == 8) {
            checksum += byte_buffer;
            bits_filled = 0;
            byte_buffer = 0;
          }
        }
        break;
      }
    }
  }

  if (bits_filled > 0) {
    byte_buffer <<= (8 - bits_filled);
    checksum += byte_buffer;
  }

  return checksum;
}

double get_entropy(int *frequency_arr, int total_symbols) {
  double entropy = 0;

  for (int i = 0; i < CHARNUM; i++) {
    if (frequency_arr[i] > 0) {
      double p = (double)frequency_arr[i] / total_symbols;
      entropy -= p * log2(p);
    }
  }

  return entropy;
}

double get_avg_code(Code *codes, int code_count, int *frequency_arr,
                    int total_symbols) {
  double avg_code = 0;

  for (int i = 0; i < code_count; i++) {
    unsigned char sym = codes[i].symbol;
    int frequency = frequency_arr[sym];
    avg_code += ((double)frequency / total_symbols) * codes[i].length;
  }

  return avg_code;
}

int validate_header(FILE *file, int *version, int *flag, size_t *old_size,
                    size_t *new_size, unsigned int *checksum) {
  char ftag[4];
  if (fread(ftag, 1, 4, file) != 4)
    return 0;
  if (strncmp(ftag, "HUF1", 4) != 0)
    return 0;

  int fver = fgetc(file);
  if (fver == EOF)
    return 0;
  *version = fver;

  int fflag = fgetc(file);
  if (fflag == EOF)
    return 0;
  *flag = fflag;

  fseek(file, 2, SEEK_CUR);

  *old_size = 0;
  for (int i = 0; i < 8; i++) {
    int size = fgetc(file);
    if (size == EOF)
      return 0;
    *old_size |= ((size_t)size << (i * 8));
  }

  *new_size = 0;
  for (int i = 0; i < 8; i++) {
    int size = fgetc(file);
    if (size == EOF)
      return 0;
    *new_size |= ((size_t)size << (i * 8));
  }

  *checksum = 0;
  for (int i = 0; i < 4; i++) {
    int size = fgetc(file);
    if (size == EOF)
      return 0;
    *checksum |= ((unsigned int)size << (i * 8));
  }

  fseek(file, 4, SEEK_CUR);

  return 1;
}

int file_integrity(FILE *file) {
  int version, flag;
  size_t original_size, compressed_size;
  unsigned int checksum;

  if (!validate_header(file, &version, &flag, &original_size, &compressed_size,
                       &checksum)) {
    printf("Subor nie je validny HUF format.\n");

    return -5;
  }

  printf("HUF verzia: %d\n", version);
  printf("Flagy: %d\n", flag);
  printf("Povodna velkost: %zu B\n", original_size);
  printf("Komprimovana velkost: %zu B\n", compressed_size);
  printf("Checksum: %u\n", checksum);

  return 1;
}
