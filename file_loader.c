#include "file_loader.h"

#include <stdio.h>

Status processLines(FILE* file, List* list, ParseInfo* info);

Status loadDemographyData(const char* filePath, List* list, ParseInfo* info) {
  Status error = STATUS_OK;
  FILE* file = NULL;
  char buffer[LINE_SIZE];

  if (filePath == NULL || list == NULL || info == NULL) {
    error = ERR_FILE_OPEN;
  } else {
    info->totalRows = 0;
    info->validRows = 0;
    info->invalidRows = 0;

    file = fopen(filePath, "r");

    if (file == NULL) {
      error = ERR_FILE_OPEN;
    } else {
      if (fgets(buffer, sizeof(buffer), file) == NULL || !validateCsvHeader(buffer)) {
        error = ERR_INVALID_HEADER;
      } else {
        error = processLines(file, list, info);
      }

      fclose(file);
    }
  }

  return error;
}

Status processLines(FILE* file, List* list, ParseInfo* info) {
  Status error = STATUS_OK;
  char buffer[LINE_SIZE];
  DemographyRecord record;

  while (fgets(buffer, sizeof(buffer), file) != NULL) {
    info->totalRows++;

    if (parseDemographyLine(buffer, &record)) {
      if (pushBack(list, &record)) {
        info->validRows++;
      } else {
        error = MEMORY_ERR;
        break;
      }
    } else {
      info->invalidRows++;
    }
  }

  return error;
}
