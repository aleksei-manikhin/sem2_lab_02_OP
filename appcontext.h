#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include "list.h"

typedef enum {
    STATUS_OK,
    ERR_FILE_OPEN,
    ERR_INVALID_HEADER,
    MEMORY_ERR,
    ERR_EMPTY_DATA,
    ERR_INVALID_REGION,
    ERR_INVALID_COLUMN
} Status;

typedef struct {
    size_t totalRows;
    size_t validRows;
    size_t invalidRows;
} ParseInfo;

typedef struct {
    double min;
    double max;
    double median;
} Metrics;

typedef struct {
    List* list;
    ParseInfo parseInfo;
    Status status;
    Metrics metrics;
} AppContext;

#endif // APP_CONTEXT_H
