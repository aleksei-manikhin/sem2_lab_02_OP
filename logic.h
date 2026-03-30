#ifndef LOGIC_H
#define LOGIC_H

#include "appcontext.h"
#include "demography_record.h"

#ifdef __cplusplus
extern "C" {
#endif

void initContext(AppContext* context);
void disposeContext(AppContext* context);
Status runLoadDataTask(AppContext* context, const char* filePath);
Status runCalculateMetricsTask(AppContext* context, const char* region, Column column);

#ifdef __cplusplus
}
#endif

#endif // LOGIC_H
