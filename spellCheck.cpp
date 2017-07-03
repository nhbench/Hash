/***********************************************************************
 * Module:
 *    Week 12, Spell Check
 *    Brother Helfrich, CS 235
 * Author:
 *    <your name here>
 * Summary:
 *    This program will implement the spellCheck() function
 ************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "spellCheck.h"
#include "hash.h"
using namespace std;



/*****************************************
 * S HASH
 * A hash of strings
 ****************************************/
class SHash : public Hash <string>
{
public:
   SHash(int numBuckets)    throw (const char *) : Hash <string> (numBuckets) {}
   SHash(const SHash & rhs) throw (const char *) : Hash <string> (rhs)        {}

   /*****************************************
    * HASH
    * PURPOSE: Provide the index of the bucket
    * REASONING: Page 309 of the BYUI PDF suggests
    * this method as being fast, systematic, and uniform
    ****************************************/
   int hash(const string & data) const
   {
      int add = 0;
      for (int i = 0; i < data.length(); i++)
         add += data[i];
      int index = add % capacity();
      assert(0 <= index && index < capacity());
      return index;
   }
};



string promptUser();
void readFile(SHash & data, const string & fileName);
void checkSpelling(SHash & data, const string & fileName);

/*****************************************
 * SPELL CHECK
 * Prompt the user for a file to spell-check
 ****************************************/
void spellCheck()
{
   // the dictionary
   // Page 312 BYUI PDF: the number of buckets should be 1.5
   // times larger than the number of elements to be stored in the hash.
   // Since dictionary.txt has 2323 entries (2323 * 1.5 == 3485)
   SHash d(3485);
   
   readFile(d, "/home/cs235/week12/dictionary.txt");
   
   string fileName = promptUser();
   
   checkSpelling(d, fileName);
}

string promptUser()
{
	string filename;
	cout << "What file do you want to check? ";
	cin.ignore();
	getline(cin, filename);
	return filename;
}

void readFile(SHash & data, const string & fileName)
{
	string input;
	ifstream fin(fileName.c_str());
	if (fin.fail())
		cerr << "Unable to open file\n";
	else
		while (fin >> input)
		{
         data.insert(input);
		}
	fin.close();
}

void checkSpelling(SHash & data, const string & fileName)
{
	string input;
   vector <string> list; 
	ifstream fin(fileName.c_str());
   
   bool isPerfect = true;
   
	if (fin.fail())
		cerr << "Unable to open file\n";
	else
   {
      while (fin >> input)
      {
         // convert string to lower case
         string text = input;
         std::transform(input.begin(), input.end(), input.begin(), ::tolower);
         
         if(!data.find(input))
         {
            list.push_back(text);
            isPerfect = false;
         }
         
      }
      if (isPerfect)
         cout << "File contains no spelling errors\n";
      else
      {
         cout << "Misspelled: ";
         for (int i = 0; i < list.size(); i++)
         {
            cout << list[i] << (i == list.size() - 1 ? "" : ", ");
         }
         cout << endl;
      }

   }
	fin.close();
}
