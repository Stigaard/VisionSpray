#ifndef TreatmentDatabase_H
#define TreatmentDatabase_H

#include <QString>
#include <vector>
#include <assert.h>
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>

enum TreatmentType {
    ALWAYS_SPRAY,
    NEVER_SPRAY,
    MODICOVI_THRESHOLD1,
    MODICOVI_THRESHOLD2,
    MODICOVI_THRESHOLD3,
    MODICOVI_THRESHOLD4,
    MODICOVI_THRESHOLD5,
    RULE_OF_THUMB_THRESHOLD1,
    RULE_OF_THUMB_THRESHOLD2,
    RULE_OF_THUMB_THRESHOLD3,
    RULE_OF_THUMB_THRESHOLD4,
    RULE_OF_THUMB_THRESHOLD5};

class TreatmentDatabase
{
public:
    TreatmentDatabase(const QString & filename);
    TreatmentType getTreatmentOfParcel(int parcelNumber);
private:
    void createTestTreatments();
    void loadTreatmentsFromFile(const QString & filename);
    std::vector<TreatmentType> parcelTreatments;
};

#endif // TreatmentDatabase_H
