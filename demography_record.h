#ifndef DEMOGRAPHY_RECORD_H
#define DEMOGRAPHY_RECORD_H

#define REGION_SIZE 128
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
