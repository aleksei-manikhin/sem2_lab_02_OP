#include "parser.h"

#include <stdio.h>
#include <string.h>

int validateCsvHeader(const char* headerLine) {
  int isCorrect = 0;

  if (headerLine != NULL) {
    if (strstr(headerLine, "year") != NULL &&
        strstr(headerLine, "region") != NULL &&
        strstr(headerLine, "birth_rate") != NULL &&
        strstr(headerLine, "death_rate") != NULL &&
        strstr(headerLine, "urbanization") != NULL) {
        isCorrect = 1;
    }
  }

    return isCorrect;
}

int parseDemographyLine(const char* line, DemographyRecord* record) {
  int scannedFields = 0;
  int isCorrect = 0;

  if (line != NULL && record != NULL) {
    scannedFields = sscanf(line, "%d,%127[^,],%lf,%lf,%lf,%lf,%lf",
                           &record->year,
                           record->region,
                           &record->naturalPopulationGrowth,
                           &record->birthRate,
                           &record->deathRate,
                           &record->generalDemographicWeight,
                           &record->urbanization);

    if (scannedFields == FIELDS_COUNT) {
      isCorrect = 1;
    }
  }

  return isCorrect;
}
