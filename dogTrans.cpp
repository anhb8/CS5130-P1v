// --------------------------------------------------------------------
//
// dogTrans.cpp
//
// Base file for input and output for dog transportation problem.
//
// CS5130 Advanced Algorithms
//
// Sharlee Climer, Feb. 2023
//
// ---------------------------------------------------------------------



#include "dogTrans.h"

using namespace std;

void findOptimal(char* dogSize, char* carSize, int* price, int** assignment, int numDogs, int numCars);

int main(int argc, char ** argv) 
{
  if (argc != 2)
    fatal("Usage:\n\n   dogTrans inputFile\n\n");

  timer t;
  t.start("Timer started.");

  cout << "\nCommand line arguments: \n\t";
  for (int i = 0; i < argc; i++)
    cout << argv[i] << " ";
  cout << "\n" << endl;
  
  char *inputFileName = argv[1];
  FILE *logfile; 

  char logfileName[2*STRING_LENGTH]; 
  char outputFileName[2*STRING_LENGTH]; 
  char base[STRING_LENGTH]; // base for logfile and output file names
  const char logSuffix[] = ".log"; 
  const char outSuffix[] = ".out"; 

  // determine filenames
  for (int i = 0; i < STRING_LENGTH; i++) {
    base[i] = inputFileName[i]; // copy input file name
    if (base[i] == '\0') {
	    if(i < 5)
	      fatal("Expected output file name to have at least 5 characters");
	
      base[i-4] = '\0'; // set end of string to not include 4 char suffix 
      break;
      }
  } 
  sprintf(logfileName, "%s%s", base, logSuffix); // string together file names
  sprintf(outputFileName, "%s%s", base, outSuffix);

  if(LOG_FILE) 
    cout << "Screen output will be recorded in '" << logfileName <<"'.\n\n";

  if ((logfile = fopen(logfileName, "w")) == NULL)
      fatal("Log file could not be opened.\n");

  if (LOG_FILE) {
    fprintf(logfile, "\nCommand line arguments: \n\t");
    for (int i = 0; i < argc; i++)
      fprintf(logfile, "%s ", argv[i]);
    fprintf(logfile, "\n\n");
  }
  
  if (!QUIET) 
    cout << "Input will be read from '" << inputFileName << "' and results will be written to '" << outputFileName << "'." << endl;
  
  if(LOG_FILE) 
    fprintf(logfile, "\nInput will be read from '%s' and results will be written to '%s'.\n", inputFileName, outputFileName); 

  int numDogs = 0;
  int numCars = 0;
  
  // read in data, count numbers
  char tempStrng [STRING_LENGTH]; // string for temporary storage
  int tempInt; // temporary int storage
  float tempFloat; // temporary float storage
    
  ifstream inputFile;
  ofstream outputFile;

  inputFile.open(inputFileName);
  outputFile.open(outputFileName);

  if (!inputFile.is_open())
    fatal("Unable to open input file.");

  if (!outputFile.is_open())
    fatal("Unable to open output file.");

  inputFile >> tempStrng; // read in 'Dogs'
  if (strncmp(tempStrng, "Dogs", 4) != 0)
    fatal("Expected input file to begin with 'Dogs'.");
    
  inputFile >> tempStrng; // read in first dog ID
    
  while (strncmp(tempStrng, "Vehicles", 8) != 0) {  
    for (int i = 0; i < 2; i++) // read in 2 strings on each line
      inputFile >> tempStrng;
    numDogs++;
  }
  
  inputFile >> tempStrng; // read in first vehicle ID
    
  while (!inputFile.eof()) {  
    for (int i = 0; i < 3; i++) // read in 3 strings on each line
      inputFile >> tempStrng;
    numCars++;
  }
  
  if(!QUIET)
    cout << "Read in data for " << numDogs << " dogs and " << numCars << " vehicles." <<  endl;
  
    if(LOG_FILE) {
    fprintf(logfile, "Read in data for %d dogs and %d vehicles.\n\n", numDogs, numCars);
  } 

  // check validity of values
  if (numDogs < 2)
    fatal("Must have at least 2 dogs.");
    
  if (numCars < 1)
    fatal ("Must have at least one vehicle.");
  
  inputFile.close();
  
  // allocate memory for arrays holding constants
  char **dogIDs; 
  if ((dogIDs = new char*[numDogs]) == NULL)
    fatal("memory not allocated");
  for (int i = 0; i < numDogs; i++) 
    if ((dogIDs[i] = new char[STRING_LENGTH]) == NULL)
      fatal("memory not allocated");
 
  char **carIDs; 
  if ((carIDs = new char*[numCars]) == NULL)
    fatal("memory not allocated");
  for (int i = 0; i < numCars; i++) 
    if ((carIDs[i] = new char[STRING_LENGTH]) == NULL)
      fatal("memory not allocated");
 
  char *dogSize; 
  if ((dogSize = new char[numDogs]) == NULL)  // use S, M, L for small, medium, large
      fatal("memory not allocated");  
      
  int *price; // array to hold prices of cars for making the trip
  if ((price = new int[numCars]) == NULL)
      fatal("memory not allocated"); 

  char *carSize; // array to hold sizes of dog crates for each car
  if ((carSize = new char[numCars]) == NULL)  // use M, R, J, S for mini, regular, jumbo, and super
      fatal("memory not allocated"); 
  
  // allocate memory for arrays holding assignment variables 
  int **assignment; // assignment[i][j] = 1 if dog i is transported in vehicle j, 0 otherwise
  if ((assignment = new int*[numDogs]) == NULL)
    fatal("memory not allocated");
  for (int i = 0; i < numDogs; i++) 
    if ((assignment[i] = new int[numCars]) == NULL)
      fatal("memory not allocated");
  
  // reopen input file and read in information
  inputFile.open(inputFileName);

  if (!inputFile.is_open())
    fatal("Unable to open input file.");

  inputFile >> tempStrng; // read in 'Dogs' and ignore
  
  for (int i = 0; i < numDogs; i++) {
    inputFile >> dogIDs[i];  
    inputFile >> tempStrng;
    dogSize[i] = tempStrng[0]; // dog size (only keep first character)
    
    if (inputFile.eof())
      fatal ("Less data in input file than expected.");
  }  
  
  inputFile >> tempStrng; // read in 'Vehicles' and ignore
  
  for (int i = 0; i < numCars; i++) {
    inputFile >> carIDs[i];  
    inputFile >> tempStrng;
    carSize[i] = tempStrng[0]; // crate size (only keep first character)
    inputFile >> tempStrng;
    price[i] = atoi(tempStrng);
  }
  
  inputFile.close();
  
  if (VERBOSE) {
    cout << "\nDog IDs and sizes are:" << endl;
    for (int i = 0; i < numDogs; i++)
      cout << dogIDs[i] << "\t" << dogSize[i] << endl;
    cout << endl;
    
    cout << "Vehicle IDs, crate sizes, and prices are:" << endl;
    for (int i = 0; i < numCars; i++)
      cout << carIDs[i] << "\t" << carSize[i] << "\t" << price[i] << endl;
    cout << "\n";
  }

  findOptimal(dogSize, carSize, price, assignment, numDogs, numCars);
  
  
  //determine which cars are used by looking at values in the assignment matrix
  int *driven;
  if ((driven = new int[numCars]) == NULL)
    fatal("memory not allocated"); 

  for (int i = 0; i < numCars; i++) {
    driven[i] = 0;
    for (int j = 0; j < numDogs; j++)
      if (assignment[j][i] == 1)
        driven[i]++;
  }
  
  // output results   
  if (VERBOSE) {
    cout << "\nAssignments are:\n\n\tVehicle\tDogs\tCost\n";
    for (int i = 0; i < numCars; i++) {
      cout << "\t" << carIDs[i] << "\t";
      for (int j = 0; j < numDogs; j++)
        if (assignment[j][i] == 1)
          cout << dogIDs[j] << " ";
      if(driven[i] > 0)
        cout << "\t" << price[i] << "\n";
      else
        cout << "none\t0\n";
    }
    cout << "\n";
  }
    
  outputFile << "Vehicle\tDogs\tCost\n";
  for (int i = 0; i < numCars; i++) {
    outputFile << carIDs[i] << "\t";
    for (int j = 0; j < numDogs; j++)
      if (assignment[j][i] == 1)
        outputFile << dogIDs[j] << " ";
    if(driven[i] > 0)
      outputFile << "\t" << price[i] << "\n";
    else
      outputFile << "none\t0\n";
  }    
  
  // add up costs for all transportation used
  int total = 0;
  for (int i = 0; i < numCars; i++)
    if (driven[i] > 0)
      total += price[i];
      
  if (!QUIET)
    cout << "Total cost for transportation is " << total << ".\n\n";
  
  if(LOG_FILE) {  
    fprintf(logfile, "Total cost for transportation is %d\n", total); 
  }

  outputFile << "Total Cost: " << total << ".\n";
  cout << "Optimal solution printed to '" << outputFileName << "'.\n\n";

  t.stop("\nTimer stopped.");
  double compTime;
  compTime = t.timeVal();
  
  int seconds = 0;
  int min = 0;
  int hour = 0;
  
  seconds = (int)compTime % 60;
  min = (int)compTime / 60;
  hour = min / 60;
  min = min % 60;
  
  cout << hour << " hours, " << min << " minutes, " << seconds << " seconds.\n" << endl;
  outputFile << "Computation Time: " << hour << " hours, " << min << " minutes, " << seconds << " seconds.\n";

  if(LOG_FILE) 
    fprintf(logfile, "\n%d hours, %d minutes, %d seconds.\n\n", hour, min, seconds);
  
  outputFile.close();
  fclose(logfile);

  return 0;
}


// Compute the optimal solution and record the assignments in the 'assignment' matrix
// IMPORTANT: assignment[i][j] = 1 if dog i is transported in vehicle j, and is equal to zero otherwise
void findOptimal(char* dogSize, char* carSize, int* price, int** assignment, int numDogs, int numCars) 
{
  for (int i = 0; i < numDogs; i++)
    for (int j = 0; j < numCars; j++)
      assignment[i][j] = 1; 
}
