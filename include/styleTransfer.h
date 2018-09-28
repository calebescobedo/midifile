#include "MidiFile.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


namespace styleTransfer{

  //parsing the midiFiles
  std::vector<std::string> getFileNames(std::string fileName, std::string root );
  std::vector<smf::MidiFile> loadMidiFiles(std::vector<std::string> & fileNames);


  //functions on vectors of midiFiles
    std::vector<int> getTrackDistrobution(std::vector<smf::MidiFile> & allSongs);
    std::vector<smf::MidiFile> getMidiFilesWithOneTrack(std::vector<smf::MidiFile> & allSongs);
    std::vector<int> getTrackDistrobution(std::vector<smf::MidiFile> & allSongs);

    void zeroNotesOfVelZero(std::vector<smf::MidiFile> & allSongs);

  //Functions on single midiFiles
  //TODO: Try everyfunction on a single midifile before scaling it up
  int getMinNoteLength(smf::MidiFile & midiFile);

}
