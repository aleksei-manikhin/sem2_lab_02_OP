#ifndef LIST_H
#define LIST_H

#include <stddef.h>

typedef struct Node {
    void* data;
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct {
    size_t size;
    size_t dataSize;
    Node *head;
    Node *tail;
} List;

List* initList(size_t dataSize);
void disposeList(List* list);
void clearList(List* list);
int pushBack(List* list, const void* data);
int pushFront(List* list, const void* data);

#endif // LIST_H
