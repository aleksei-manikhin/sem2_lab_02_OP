#ifndef FILE_LOADER_H
#define FILE_LOADER_H

#include "list.h"
#include "parser.h"
#include "appcontext.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LINE_SIZE 1024

Status loadDemographyData(const char* filePath, List* list, RowCounts* info);

#ifdef __cplusplus
}
#endif

#endif // FILE_LOADER_H
