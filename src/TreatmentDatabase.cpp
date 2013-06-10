#include "TreatmentDatabase.h"

TreatmentDatabase::TreatmentDatabase(const QString & filename)
{
//   createTestTreatments();
  loadTreatmentsFromFile(filename);
  return;
}

void TreatmentDatabase::createTestTreatments()
{
  parcelTreatments.push_back(ALWAYS_SPRAY);
  parcelTreatments.push_back(NEVER_SPRAY);
  parcelTreatments.push_back(MODICOVI_THRESHOLD1);
  parcelTreatments.push_back(MODICOVI_THRESHOLD2);
  parcelTreatments.push_back(MODICOVI_THRESHOLD3);
  parcelTreatments.push_back(MODICOVI_THRESHOLD4);
  parcelTreatments.push_back(MODICOVI_THRESHOLD5);
  parcelTreatments.push_back(RULE_OF_THUMB_THRESHOLD1);
  parcelTreatments.push_back(RULE_OF_THUMB_THRESHOLD2);
  parcelTreatments.push_back(RULE_OF_THUMB_THRESHOLD3);
  parcelTreatments.push_back(RULE_OF_THUMB_THRESHOLD4);
  parcelTreatments.push_back(RULE_OF_THUMB_THRESHOLD5);
}

void TreatmentDatabase::loadTreatmentsFromFile(const QString& filename)
{
  // Open file for reading
  std::ifstream infile(filename.toStdString().c_str());
  std::string line;
  
  // Parse the file line by line
  while (std::getline(infile, line))
  {
    std::istringstream iss(line);
    int parcelNumber, seededWeeds, cameraPosition, plannedTreatment, repetition;
    // Read information from the line
    if (!(iss >> parcelNumber >> seededWeeds >> cameraPosition >> plannedTreatment >> repetition)) { 
      std::cout << "Bad line in input file" << std::endl;
      assert(1 == 0);
      break; 
    } // error

    // Convert to treatment type enum and store in list.
    TreatmentType treatment = (TreatmentType) (plannedTreatment - 1);
    parcelTreatments.push_back(treatment);
  }
}

TreatmentType TreatmentDatabase::getTreatmentOfParcel(int parcelNumber)
{
  // Check if the parcel number if known
  assert(parcelNumber > 0);
  assert(parcelNumber < parcelTreatments.size() + 1);

  // Convert index to a zero indexed array
  int index = parcelNumber - 1;
  return parcelTreatments[index];
}

