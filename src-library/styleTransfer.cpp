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
  void writeToBinaryWithAllPossible(std::string absFilePath);
  void loadInBinary(std::string absBinaryPath){

    smf::MidiFile loadedInFile;
    loadedInFile.addTrack();
    loadedInFile.setTicksPerQuarterNote(96);
    std::cout << "after creation: " << loadedInFile.getTrackCount()  << std::endl;

    std::ifstream infile(absBinaryPath);
    std::string curLine;

    int curChannel = 0;
    int linesRead = 0;
    int curTrack = 0;
    int velocity = 80;
    int startTick;
    int endTick;

    std::vector<std::string> curChannelVec;
    while(std::getline(infile, curLine)){

      if(curLine.length() == 0){
        //skip the line
        linesRead = 0;
        curChannelVec.clear();
      }else if(curLine.length() == 1){
        curChannel++;
        linesRead = 0;
        curChannelVec.clear();
      }else{
        curChannelVec.push_back(curLine);
        linesRead++;
      }

      if(linesRead == 128){
        for(int curKey = 0; curKey < curChannelVec.size(); curKey++){
          for(int curTime = 0; curTime < curChannelVec[curKey].size(); curTime++){
            if(curChannelVec[curKey][curTime] == '1'){
              startTick = curTime;
              endTick = curTime;
              while(endTick < curChannelVec[curKey].size() && curChannelVec[curKey][endTick] == '1'){
                endTick++;
                curTime++;
              }
              //do the process of reading in this file and creating the tracks
              std::cout << "track: " << curTrack << " startTime: " << startTick << " curChannel" << curChannel << " curKey "  <<  curKey << "Velocity: " << velocity << std::endl;
              loadedInFile.addNoteOn(curTrack, startTick, curChannel, curKey, velocity);
              loadedInFile.addNoteOff(curTrack, endTick, curChannel, curKey);
            }
          }
        }
        curTrack++;
       }
    }

    m_midiFile = loadedInFile;
    m_midiFile.sortTracks();
  }


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
  getTrackInsterments();

  m_midiFile.sortTracks();
  m_midiFile.doTimeAnalysis();
  m_midiFile.absoluteTicks();
  m_midiFile.linkNotePairs();

}

//im going to need to use merge tracks to get all the tracks that are in the same family togeather

std::vector<std::vector<bool>> getBinaryTrack(smf::MidiEventList& eventList, int maxLength, int ticksPQN){

  int key;
  int duration;
  int startTime;

  int minNote = ticksPQN / 8;
  std::vector<std::vector<bool>> ret(128, std::vector<bool>(maxLength, 0));

  for(int i = 0; i < eventList.size(); i++){

    if(eventList[i].isNoteOn()){

      key = eventList[i].getKeyNumber();
      duration = eventList[i].getTickDuration()/minNote;
      startTime = eventList[i].tick/minNote;

      //for the duration of the note turn it on in the binary file
      for(int t = startTime; t < startTime + duration; t++){
        if (t < maxLength){
          ret[key][t] = 1;
          //guard agains indexing out of bounds
        }
      }

    }

  }
  return ret;

}

void printToFile(std::vector<std::vector<bool>> & thingToPrint, std::ofstream & myFile){

  for(int key = 0; key < thingToPrint.size(); key++){
    for(int curTime = 0; curTime < thingToPrint[key].size(); curTime++){
      if(thingToPrint[key][curTime] == 1){
        myFile << "1";
      } else {
        myFile << "0";
      }
    }
    myFile << "\n";
  }
  myFile << "\n";
}


void Song::writeToBinaryWithAllPossible(std::string absFilePath){
  //there are 16 familys of midi isterments
  std::ofstream myfile;
  myfile.open (absFilePath);

  int maxLength = 512;
  bool found = false;
  std::vector<std::vector<bool>> curChannel(128, std::vector<bool>(maxLength, 0));

  for(int curFamily = 0; curFamily < 16; curFamily++){

    for(int x = 0; x < m_trackInsterments.size(); x++){

      if(curFamily == (m_trackInsterments[x]/8)){

        curChannel = getBinaryTrack(m_midiFile[x], maxLength, m_ticksPerQuarterNotes);
        found = true;
      }

    }
    if(found == false){
      curChannel = std::vector<std::vector<bool>>(1, std::vector<bool>(1, 0));
    }
    found = false;

    printToFile(curChannel, myfile);

  }

  myfile.close();
}


void Song::groupInsterments(){

  for(int x = 0; x < m_numTracks-1; x++){
    for(int y = x + 1; y < m_numTracks; y++){
      if( (m_trackInsterments[x]/8) == (m_trackInsterments[y]/8) ){

        m_midiFile.mergeTracks(x, y);
        m_numTracks = m_midiFile.getTrackCount();
        m_trackInsterments.erase(m_trackInsterments.begin() + y);
        m_midiFile.sortTracks();
        m_midiFile.doTimeAnalysis();
        m_midiFile.absoluteTicks();
        m_midiFile.linkNotePairs();
        y--;

      }
    }
  }

}

void Song::getTrackInsterments(){
    m_trackInsterments = std::vector<int>(m_numTracks, -1);//all tracks to have an insterment number of -1

    for(int curChannel = 0; curChannel < m_numTracks; curChannel++){
      for(int curEvent = 0; curEvent < m_midiFile[curChannel].size(); curEvent++){
        if(m_midiFile[curChannel][curEvent].isTimbre()){
          if(m_trackInsterments[curChannel] == -1){
            m_trackInsterments[curChannel] =  m_midiFile[curChannel][curEvent].getP1();
          }else{
            m_midiFile[curChannel][curEvent].setP1(m_trackInsterments[curChannel]);
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
     int curChannelCount = 1;
     for(int x = 0; x < allSongs.size(); x++){
        curChannelCount = allSongs[x].getTrackCount();
        ret[curChannelCount-1]++;
     }
     return ret;
  }


}
