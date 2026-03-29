#ifndef FILE_LOADER_H
#define FILE_LOADER_H

#include "list.h"
#include "parser.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LINE_SIZE 1024

typedef struct {
    size_t totalRows;
    size_t validRows;
    size_t invalidRows;
} ParseInfo;

typedef enum {
    PARSER_OK,
    PARSER_FILE_ERROR,
    PARSER_HEADER_ERROR,
    PARSER_MEMORY_ERROR
} ParserError;

ParserError loadDemographyData(const char* filePath, List* list, ParseInfo* info);

#ifdef __cplusplus
}
#endif

#endif // FILE_LOADER_H
