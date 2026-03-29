#include "list.h"

#include <stdlib.h>
#include <string.h>

Node* createNode(size_t dataSize, const void* data);
void appendNode(List* list, Node* node);


List* initList(size_t dataSize) {
  List* list = malloc(sizeof(List));

  if (list != NULL) {
    list->size = 0;
    list->dataSize = dataSize;
    list->head = NULL;
    list->tail = NULL;
  }

  return list;
}

void clearList(List* list) {
  Node* current;
  Node* next;
  if (list) {
    current = list->head;
    while (current != NULL) {
      next = current->next;
      free(current->data);
      free(current);
      current = next;
    }
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
  }
}

void disposeList(List* list) {
  if (list) {
    clearList(list);
    free(list);
  }
}

Node* createNode(size_t dataSize, const void* data) {
  Node* node = NULL;
  if (data && dataSize) {
    node = malloc(sizeof(Node));
    if (node) {
      node->data = malloc(dataSize);
      if (node->data) {
        memcpy(node->data, data, dataSize);
        node->next = NULL;
      }else {
        free(node);
        node = NULL;
      }
    }
  }
  return node;
}

void appendNode(List* list, Node* node) {
  if (list->tail != NULL)
    list->tail->next = node;
  else
    list->head = node;
  list->tail = node;
}


int pushBack(List* list, const void* data) {
  int result = 0;
  if (list) {
    Node* node = createNode(list->dataSize, data);
    if (node != NULL) {
      appendNode(list, node);
      list->size++;
      result = 1;
    }
  }
  return result;
}

