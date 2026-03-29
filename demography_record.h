#ifndef DEMOGRAPHY_RECORD_H
#define DEMOGRAPHY_RECORD_H

#define REGION_SIZE 128


typedef enum {
    COL_YEAR = 1,
    COL_REGION = 2,
    COL_NPG = 3,
    COL_BIRTH_RATE = 4,
    COL_DEATH_RATE = 5,
    COL_GDW = 6,
    COL_URBANIZATION = 7
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
