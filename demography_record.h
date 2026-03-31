#ifndef DEMOGRAPHY_RECORD_H
#define DEMOGRAPHY_RECORD_H

#define REGION_SIZE 128

typedef enum {
    COL_YEAR = 1,
    COL_REGION,
    COL_NPG,
    COL_BIRTH_RATE,
    COL_DEATH_RATE,
    COL_GDW,
    COL_URBANIZATION,
} Column;


typedef struct
{
    int year;
    char region[REGION_SIZE];
    double naturalPopulationGrowth;
    double birthRate;
    double deathRate;
    double generalDemographicWeight;
    double urbanization;
} DemographyRecord;

#endif // DEMOGRAPHY_RECORD_H
