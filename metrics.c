#include "metrics.h"

#include <string.h>

#include "iterator.h"
#include "list.h"

int isMetricColumn(Column column);
int compareDoubleValues(const void* left, const void* right);
Status validateMetricsInput(const AppContext* context, const char* region, Column column);
int getColumnValue(const DemographyRecord* record, Column column, double* value);
int insertRegionValues(const AppContext* context, const char* region, Column column, List* sortedValues);
int readValueAt(const List* sortedValues, size_t targetIndex, double* value);
void fillMetricsFromSorted(Metrics* metrics, const List* sortedValues);

int isMetricColumn(Column column) {
  int isValid = 0;

  if (column >= COL_YEAR && column <= COL_URBANIZATION && column != COL_REGION)
    isValid = 1;

  return isValid;
}

int compareDoubleValues(const void* left, const void* right) {
  int result = 0;
  double a = *(const double*)left;
  double b = *(const double*)right;

  if (a < b)
    result = -1;
  else if (a > b)
    result = 1;

  return result;
}

Status validateMetricsInput(const AppContext* context, const char* region, Column column) {
  Status status = STATUS_OK;

  if (context == NULL || context->list == NULL || region == NULL)
    status = ERR_EMPTY_DATA;
  else if (!isMetricColumn(column))
    status = ERR_INVALID_COLUMN;

  return status;
}

int getColumnValue(const DemographyRecord* record, Column column, double* value) {
  int isSuccess = 0;

  if (record != NULL && value != NULL) {
    switch (column) {
      case COL_YEAR: *value = (double)record->year; isSuccess = 1; break;
      case COL_NPG: *value = record->naturalPopulationGrowth; isSuccess = 1; break;
      case COL_BIRTH_RATE: *value = record->birthRate; isSuccess = 1; break;
      case COL_DEATH_RATE: *value = record->deathRate; isSuccess = 1; break;
      case COL_GDW: *value = record->generalDemographicWeight; isSuccess = 1; break;
      case COL_URBANIZATION: *value = record->urbanization; isSuccess = 1; break;
      default: break;
    }
  }

  return isSuccess;
}

int insertRegionValues(const AppContext* context, const char* region, Column column, List* sortedValues) {
  int isSuccess = 1;
  Iterator it = begin(context->list);

  while (isSet(&it) && isSuccess) {
    DemographyRecord* record = (DemographyRecord*)get(&it);
    if (record != NULL && strcmp(record->region, region) == 0) {
      double value = 0.0;
      if (getColumnValue(record, column, &value) &&
          !insertSorted(sortedValues, &value, compareDoubleValues))
        isSuccess = 0;
    }
    next(&it);
  }

  return isSuccess;
}

int readValueAt(const List* sortedValues, size_t targetIndex, double* value) {
  int isSuccess = 0;
  size_t currentIndex = 0;
  Iterator it = begin(sortedValues);

  while (isSet(&it) && !isSuccess) {
    if (currentIndex == targetIndex) {
      *value = *(double*)get(&it);
      isSuccess = 1;
    }
    currentIndex++;
    next(&it);
  }

  return isSuccess;
}

void fillMetricsFromSorted(Metrics* metrics, const List* sortedValues) {
  size_t leftIndex = (sortedValues->size - 1) / 2;
  size_t rightIndex = sortedValues->size / 2;
  double leftValue = 0.0;
  double rightValue = 0.0;

  metrics->min = *(double*)sortedValues->head->data;
  metrics->max = *(double*)sortedValues->tail->data;
  readValueAt(sortedValues, leftIndex, &leftValue);
  readValueAt(sortedValues, rightIndex, &rightValue);
  metrics->median = (leftValue + rightValue) / 2.0;
}

Status calculateMetrics(AppContext* context, const char* region, Column column) {
  Status status = validateMetricsInput(context, region, column);
  List* sortedValues = NULL;

  if (status == STATUS_OK) {
    sortedValues = initList(sizeof(double));
    if (sortedValues == NULL)
      status = MEMORY_ERR;
  }
  if (status == STATUS_OK && sortedValues != NULL) {
    if (!insertRegionValues(context, region, column, sortedValues))
      status = MEMORY_ERR;
    else if (sortedValues->size == 0)
      status = ERR_INVALID_REGION;
    else
      fillMetricsFromSorted(&context->metrics, sortedValues);
    disposeList(sortedValues);
  }
  context->status = status;

  return status;
}
