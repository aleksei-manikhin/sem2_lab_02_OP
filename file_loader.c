#include "file_loader.h"

#include <stdio.h>

ParserError processLines(FILE* file, List* list, ParseInfo* info);

ParserError loadDemographyData(const char* filePath, List* list, ParseInfo* info) {
  ParserError error = PARSER_OK;
  FILE* file = NULL;
  char buffer[LINE_SIZE];

  if (filePath == NULL || list == NULL || info == NULL) {
    error = PARSER_FILE_ERROR;
  } else {
    info->totalRows = 0;
    info->validRows = 0;
    info->invalidRows = 0;

    file = fopen(filePath, "r");

    if (file == NULL) {
      error = PARSER_FILE_ERROR;
    } else {
      if (fgets(buffer, sizeof(buffer), file) == NULL || !validateCsvHeader(buffer)) {
        error = PARSER_HEADER_ERROR;
      } else {
        error = processLines(file, list, info);
      }

      fclose(file);
    }
  }

  return error;
}

ParserError processLines(FILE* file, List* list, ParseInfo* info) {
  ParserError error = PARSER_OK;
  char buffer[LINE_SIZE];
  DemographyRecord record;

  while (fgets(buffer, sizeof(buffer), file) != NULL) {
    info->totalRows++;

    if (parseDemographyLine(buffer, &record)) {
      if (pushBack(list, &record)) {
        info->validRows++;
      } else {
        error = PARSER_MEMORY_ERROR;
        break;
      }
    } else {
      info->invalidRows++;
    }
  }

  return error;
}
