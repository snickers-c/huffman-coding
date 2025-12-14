#include <stdio.h>
#include <stdlib.h>
#define CHARNUM 256

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
} Code;

Node *new_node(unsigned char symbol, int frequency) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->symbol = symbol;
  node->frequency = frequency;
  node->left = NULL;
  node->right = NULL;

  // (node == NULL) {
  // printf("Failed to create new Node!");
  return node;
  // }
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
  minheap->array[minheap->size] = node;

  int i = minheap->size;
  while (i > 0) {
    if (minheap->array[i]->frequency < minheap->array[(i - 1) / 2]->frequency) {
      swap_nodes(&minheap->array[i], &minheap->array[(i - 1) / 2]);
      i = (i - 1) / 2;
    } else {
      break;
    }
  }

  minheap->size++;
}

Node *push_root_node(Minheap *minheap) {
  Node *temp = minheap->array[0];
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
    }
  }

  return temp;
}

Node *huffman_tree(Symbol_Frequency *symbols, int capacity) {
  Minheap *minheap = new_minheap(capacity);

  for (int i = 0; i < capacity; i++) {
    insert_node_to_minheap(minheap,
                           new_node(symbols[i].symbol, symbols[i].frequency));
  }

  while (minheap->size > 1) {
    Node *left = push_root_node(minheap);
    Node *right = push_root_node(minheap);

    Node *parent = new_node(0, left->frequency + right->frequency);
    parent->left = left;
    parent->right = right;

    insert_node_to_minheap(minheap, parent);
  }

  return push_root_node(minheap);
}

int main(int argc, char *argv[]) {
  FILE *file = fopen("data.txt", "r");
  if (file == NULL) {
    printf("File open failed!");
    return -2;
  }

  int frequency_arr[CHARNUM] = {0};

  int symbol;
  while ((symbol = fgetc(file)) != EOF) {
    frequency_arr[(unsigned char)symbol]++;
  }

  fclose(file);

  int capacity = 0;
  for (int i = 0; i < CHARNUM; i++) {
    if (frequency_arr[i] == 0)
      continue;

    capacity++;
    // printf("%d '%c'\n", frequency_arr[i], i);
  }

  Symbol_Frequency *symbols =
      (Symbol_Frequency *)malloc(capacity * sizeof(Symbol_Frequency));

  int index = 0;
  for (int i = 0; i < CHARNUM; i++) {
    if (frequency_arr[i] > 0) {
      symbols[index].symbol = (unsigned char)i;
      symbols[index].frequency = frequency_arr[i];
      index++;
    }
  }

  Node *huftree = huffman_tree(symbols, capacity);

  // prepared final output
  // int input_bytesize = 5000;
  // int output_bytesize = 3125;
  // float compression_ratio = input_bytesize / (output_bytesize / 1.0);
  // float entropy = 4.12;
  // float avg_code = 4.28;
  // char integrity[] = "OK";

  // printf("vstup: %d B, vystup: %d B\n", input_bytesize,
  // output_bytesize); printf("kompresny pomer: %.2f\n",
  // compression_ratio); printf("entropia: %.2f b/symbol, avg kod: %.2f
  // b/symbol\n", entropy,
  //        avg_code);
  // printf("integrita: %s\n", integrity);

  return 0;
}
