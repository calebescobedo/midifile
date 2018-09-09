//
// Programmer:    Caleb Escobedo <cescobed@trinity.edu>
// Creation Date: Sat Sep 8 12:30:00 CST 2018
// Last Modified: Sat Sep 8 12:30:00 CST 2018
// Filename:      midifile/src-programs/mid2Binary.cpp
// Website:       http://midifile.sapp.org
// Syntax:        C++11
//
// Description:   Converts a single midi channel to binary file and back
#include "MidiFile.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

std::vector<std::string> getFileNames(std::string fileName, std::string root );
std::vector<smf::MidiFile> loadMidiFiles(std::vector<std::string> & fileNames);
std::vector<int> getTrackDistrobution(std::vector<smf::MidiFile> & allSongs){
   std::vector<int> ret(16, 0);
   int curTrackCount = 1;
   for(int x = 0; x < allSongs.size(); x++){
      curTrackCount = allSongs[x].getTrackCount();
      ret[curTrackCount-1]++;
   }
   return ret;
}

int main(int argc, char** argv) {
   std::string root = "/Users/Betty/midi/";
   std::string classicalFileName = "classicalPianoNames";


   std::vector<std::string> fileNames = getFileNames(classicalFileName, root);
   std::vector<smf::MidiFile> allSongs = loadMidiFiles(fileNames);
   std::vector<int> trackDistrobution = getTrackDistrobution(allSongs);

   for(int x = 0; x < trackDistrobution.size(); x++){
      std::cout << "Songs with " << x+1 << " tracks : " << trackDistrobution[x] << std::endl;
   }

   smf::MidiFile midifile;

   std::string binaryMidi= midifile.convertToBinary();
   midifile.convertBinaryToMidi(binaryMidi);
   return 0;

}


std::vector<std::string> getFileNames(std::string fileName, std::string root ){

   std::vector<std::string> retNames;

   std::ifstream file(root + fileName);

   std::string line;
   while (file){
      std::getline(file, line);
      retNames.push_back(root + line);
   }
      retNames.pop_back();//one extra file name that is just the root?
   file.close();

   return retNames;
}

std::vector<smf::MidiFile> loadMidiFiles(std::vector<std::string> & fileNames){

   std::vector<smf::MidiFile> allSongs(fileNames.size());

   bool curLoadStatus;
   int failedLoads = 0;
   for(int x = 0; x < fileNames.size(); x++){
      allSongs[x] = smf::MidiFile(fileNames[x]);
      curLoadStatus = allSongs[x].status();
      if(!curLoadStatus) failedLoads++;
   }

   std::cout << "Total Failed Loads: " << failedLoads << "/" << fileNames.size() << std::endl;
   return allSongs;
}
