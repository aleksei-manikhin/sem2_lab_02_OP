#include "logic.h"

#include "file_loader.h"
#include "metrics.h"

void clearParseInfo(ParseInfo* parseInfo);
void clearMetrics(Metrics* metrics);

void clearParseInfo(ParseInfo* parseInfo) {
  if (parseInfo != NULL) {
    parseInfo->totalRows = 0;
    parseInfo->validRows = 0;
    parseInfo->invalidRows = 0;
  }
}

void clearMetrics(Metrics* metrics) {
  if (metrics != NULL) {
    metrics->min = 0.0;
    metrics->max = 0.0;
    metrics->median = 0.0;
  }
}

void initContext(AppContext* context) {
  if (context != NULL) {
    context->list = NULL;
    clearParseInfo(&context->parseInfo);
    clearMetrics(&context->metrics);
    context->status = STATUS_OK;
  }
}

void disposeContext(AppContext* context) {
  if (context != NULL) {
    if (context->list != NULL) {
      disposeList(context->list);
      context->list = NULL;
    }
    clearParseInfo(&context->parseInfo);
    clearMetrics(&context->metrics);
    context->status = STATUS_OK;
  }
}

Status loadData(AppContext* context, const char* filePath) {
  Status status = STATUS_OK;

  if (context == NULL)
    status = ERR_EMPTY_DATA;
  else {
    if (filePath == NULL)
      status = ERR_FILE_OPEN;
    else {
      if (context->list == NULL)
        context->list = initList(sizeof(DemographyRecord));
      else
        clearList(context->list);

      clearParseInfo(&context->parseInfo);
      clearMetrics(&context->metrics);

      if (context->list == NULL)
        status = MEMORY_ERR;
      else {
        status = loadDemographyData(filePath, context->list, &context->parseInfo);
        if (status == STATUS_OK && context->list->size == 0)
          status = ERR_EMPTY_DATA;
      }
    }
    context->status = status;
  }

  return status;
}

Status calculateMetricsForRegion(AppContext* context, const char* region, Column column) {
  Status status = STATUS_OK;

  if (context == NULL)
    status = ERR_EMPTY_DATA;
  else {
    status = calculateMetrics(context, region, column);
    context->status = status;
  }

  return status;
}
