#include "../include/styleTransfer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class Song{


  public:
  //member variables
  int m_numTracks;
  int m_ticksPerQuarterNotes;

  std::vector<int> m_trackInsterments;
  smf::MidiFile m_midiFile;

  //constructors
  Song();
  Song(smf::MidiFile & midiFile);
  void write(std::string absFilePath);

  private:
  //these need to be called in this exact order or operations
  void getTrackInsterments();
  void pruneTracks();
  void groupInsterments();

};

Song::Song(){}
Song::Song(smf::MidiFile & midiFile): m_midiFile(midiFile){

  m_midiFile.sortTracks();
  m_midiFile.doTimeAnalysis();
  m_midiFile.absoluteTicks();
  m_midiFile.linkNotePairs();

  m_numTracks = m_midiFile.getTrackCount();
  m_ticksPerQuarterNotes = m_midiFile.getTicksPerQuarterNote();

  getTrackInsterments();
  pruneTracks();
  groupInsterments();
}

//im going to need to use merge tracks to get all the tracks that are in the same family togeather

void Song::groupInsterments(){
  for(int x = 0; x < m_numTracks-1; x++){
    for(int y = x + 1; y < m_numTracks; y++){
      if( (m_trackInsterments[x]/8) == (m_trackInsterments[y]/8) ){
        m_midiFile.mergeTracks(x, y);
        m_numTracks = m_midiFile.getTrackCount();
        m_trackInsterments.erase(m_trackInsterments.begin() + y);
        y--;
      }
    }
  }

}

void Song::getTrackInsterments(){
    m_trackInsterments = std::vector<int>(m_numTracks, -1);//all tracks to have an insterment number of -1

    for(int curTrack = 0; curTrack < m_numTracks; curTrack++){
      for(int curEvent = 0; curEvent < m_midiFile[curTrack].size(); curEvent++){
        if(m_midiFile[curTrack][curEvent].isTimbre()){
          //TODO: what do I do if the patch is changed in the middle?
          if(m_trackInsterments[curTrack] == -1){
            m_trackInsterments[curTrack] =  m_midiFile[curTrack][curEvent].getP1();
          }
        }
      }
    }
  }

  void Song::pruneTracks(){
    for(int x = 0; x < m_trackInsterments.size(); x++){

      if(m_trackInsterments[x] == -1){
        m_midiFile.deleteTrack(x);
        m_trackInsterments.erase(m_trackInsterments.begin() + x);
        x--;

      }
    }
    m_numTracks = m_midiFile.getTrackCount();
  }

  void Song::write(std::string absFilePath){
    m_midiFile.write(absFilePath);
  }

class Converter{

  public:
  std::vector<std::string> m_fileNames;
  std::vector<Song> m_allSongs;


  Converter();
  Converter(std::vector<std::string> & fileNames);

};


Converter::Converter(){}

Converter::Converter(std::vector<std::string> & fileNames):m_fileNames(fileNames){
  //load in the files and set them into midiFiles
  smf::MidiFile curFile;

  for(int curIdx = 0; curIdx < m_fileNames.size(); curIdx++){

    curFile.read(m_fileNames[curIdx]);

    if(curFile.status() == false){

      std::cout << "failed to load in file: " << m_fileNames[curIdx] << std::endl;
      continue;

    }else{

    Song curSong(curFile);
    m_allSongs.push_back(curSong);

    }
  }
}


namespace styleTransfer{
/*
  smf::MidiFile convertBinaryToMidi(std::string fileAbsName, std::string outputName){
    std::ifstream infile(fileAbsName);
    std::string line;

    std::vector<std::string> noteStrings;
    std::vector<std::vector<bool>> boolSong(128);

    while (std::getline(infile, line)){
      noteStrings.push_back(line);
    }


    for(int n = 0; n < noteStrings.size(); n++){
      for(int i = 0; i <  noteStrings[n].size(); i++){
        if(noteStrings[n][i] == '0'){
          boolSong[n].push_back(0);
        }else if(noteStrings[n][i] == '1'){
          boolSong[n].push_back(1);
        }
      }
    }

    std::vector<int> noteOnTimes;
    std::vector<int> noteOffTimes;

    //bool isNoteOn = false;
    bool noteFinished = false;
    int curNoteTicks = 0;
    int noteOnAt = 0;
    int key;

    for(int n = 0; n < boolSong.size(); n++){
      for(int i = 0; i < boolSong.size(); i++){
        if(!isNoteOn && boolSong[n][i] == 1){
          isNoteOn = true;
          noteOnAt = i;
          key = n;
          //curNoteTicks;

        }
      }
    }

    //parse the lines into note on and off events




    smf::MidiFile ret;
    //start of write

    MidiFile midifile;
    int track   = 0;
    int channel = 0;
    int instr   = 0;






    midifile.addTimbre(track, 0, channel, instr);

    int tpq = 60;
    midifile.setTicksPerQuarterNote(tpq);

   for (int i=0; i < count; i++) {

      int starttick =
      int key       =
      int endtick   = starttick + int(duration(mt) / 4.0 * tpq);
      midifile.addNoteOn (track, starttick, channel, key, 80);
      midifile.addNoteOff(track, endtick,   channel, key);

    }
    midifile.sortTracks();  // Need to sort tracks since added events are
                          // appended to track in random tick order.
    string filename = options.getString(outputName);
    midifile.write(filename);

    return ret;
  }
*/
  void printBinarySongToFile(std::vector<std::vector<bool>> & hotEn, std::string fileAbsName){

    std::ofstream outFile;
    outFile.open(fileAbsName);


    for(int i = 0; i < hotEn.size(); i++){
      for(int k = 0; k < hotEn[i].size(); k++){
        if(hotEn[i][k]== 1){
          outFile << "1";
        }else{
          outFile << "0";
        }
        outFile << " ";
      }
      outFile << std::endl;
    }

    outFile.close();
  }

  void printBinarySongToTerminal(std::vector<std::vector<bool>> & hotEn){
    for(int i = 0; i < hotEn.size(); i++){
      for(int k = 0; k < hotEn[i].size(); k++){
        std::cout << hotEn[i][k] << " ";
      }
      std::cout << std::endl;
    }
  }



  void fillInHotEncodedMidi(std::vector<std::vector<bool>> & hotEn, smf::MidiFile & song, int minNote){

    int key;
    int duration;
    int startTime;
    for(int i = 0; i < song[0].size(); i++){
      if(song[0][i].isNoteOn() && song[0][i].track == 1){

        key = song[0][i].getKeyNumber();
        duration = song[0][i].getTickDuration()/minNote;
        startTime = song[0][i].tick/minNote;
        for(int t = startTime; t < startTime + duration; t++){

          hotEn[key][t] = 1;

        }
      }
    }

  }


  std::vector<smf::MidiFile> getMidiFilesWithOneTrack(std::vector<smf::MidiFile> & allSongs){

     std::vector<smf::MidiFile> oneTrackSongs;
     for(int x = 0; x < allSongs.size(); x++){
        if(allSongs[x].getTrackCount() == 1){
           oneTrackSongs.push_back(allSongs[x]);
        }
     }
     return oneTrackSongs;
  }

  int getLastNoteOff(smf::MidiFile & midiFile){

    int lastNote = 0;
    for(int i = 0; i < midiFile[0].size(); i++){
      if(midiFile[0][i].isNoteOff()){
        if(lastNote < midiFile[0][i].tick){
          lastNote = midiFile[0][i].tick;
        }
      }
    }
    return lastNote;

  }

  int getMinNoteLength(smf::MidiFile & midiFile){
     int minDuration = 100000000;
     for(int i = 0; i < midiFile[0].getSize(); i++){

        if(midiFile[0][i].isNoteOn()){
           if(minDuration > midiFile[0][i].getTickDuration()){
              minDuration = midiFile[0][i].getTickDuration();
           }
        }
     }

     return minDuration;
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
        allSongs[x].doTimeAnalysis();
        allSongs[x].absoluteTicks();
        allSongs[x].linkNotePairs();
        std::cout << "Min Note: " <<  getMinNoteLength(allSongs[x]) << " Num events: " << allSongs[x][0].size() << std::endl;
        if(!curLoadStatus) failedLoads++;

     }

     std::cout << "Total Failed Loads: " << failedLoads << "/" << fileNames.size() << std::endl;
     return allSongs;
  }


    void zeroNotesOfVelZero(std::vector<smf::MidiFile> & allSongs){


    }



  std::vector<int> getTrackDistrobution(std::vector<smf::MidiFile> & allSongs){
     std::vector<int> ret(16, 0);
     int curTrackCount = 1;
     for(int x = 0; x < allSongs.size(); x++){
        curTrackCount = allSongs[x].getTrackCount();
        ret[curTrackCount-1]++;
     }
     return ret;
  }


}
