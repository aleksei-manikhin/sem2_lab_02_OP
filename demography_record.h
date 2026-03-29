#ifndef DEMOGRAPHY_RECORD_H
#define DEMOGRAPHY_RECORD_H

typedef struct
{
    int year;
    char* region;

    double naturalPopulationGrowth;
    double birthRate;
    double deathRate;
    double generalDemographicWeight;
    double urbanization;
} DemographyRecord;

#endif // DEMOGRAPHY_RECORD_H
