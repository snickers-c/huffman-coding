#ifndef HUFFMAN_H
#define HUFFMAN_H
#define CHARNUM 256
#define VERSION 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
  unsigned char symbol;
  int frequency;
  struct Node *left;
  struct Node *right;
} Node;

typedef struct Minheap {
  int size;
  int capacity;
  Node **array;
} Minheap;

typedef struct Symbol_Frequency {
  unsigned char symbol;
  int frequency;
} Symbol_Frequency;

typedef struct Code {
  unsigned char symbol;
  int bits[CHARNUM];
  int length;
} Code;

Node *new_node(unsigned char symbol, int frequency);

Minheap *new_minheap(int capacity);

void swap_nodes(Node **lesser, Node **greater);

void insert_node_to_minheap(Minheap *minheap, Node *node);

Node *push_root_node(Minheap *minheap);

Node *huffman_tree(Symbol_Frequency *symbols, int capacity);

Node *load_huffman_tree(FILE *file, unsigned char *byte_buffer, int *bits_left);

void generate_codes(Node *root, int *path, int depth, Code *codes,
                    int *code_count);

void write_bit(FILE *out, int bit, unsigned char *byte_buffer,
               int *bits_filled);

void file_describe_huftree(FILE *out, Node *root, unsigned char *byte_buffer,
                           int *bits_filled);

void file_compressed_data(FILE *out, unsigned char *input, size_t input_size,
                          Code *codes, int code_count);

void decompress_data(FILE *file, FILE *out, Node *root, size_t original_size,
                     unsigned char *byte_buffer, int *bits_left);

void file_header(FILE *out, size_t old_size, size_t new_size,
                 unsigned int checksum, int flag);

size_t get_compressed_size(unsigned char *input, size_t input_size, Code *codes,
                           int code_count);

unsigned int get_checksum(unsigned char *input, size_t input_size, Code *codes,
                          int code_count);

double get_entropy(int *frequency_arr, int total_symbols);

double get_avg_code(Code *codes, int code_count, int *frequency_arr,
                    int total_symbols);

int validate_header(FILE *file, int *version, int *flag, size_t *old_size,
                    size_t *new_size, unsigned int *checksum);

int file_integrity(FILE *file);

#endif // HUFFMAN_H
