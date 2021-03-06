#include "../include/styleTransfer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


class Binary_Converter{

  public:
  //member variables
  int m_numTracks;
  int m_ticksPerQuarterNotes;

  std::vector<int> m_trackInsterments;
  smf::MidiFile m_midiFile;

  //constructors
  Binary_Converter();
  Binary_Converter(smf::MidiFile & midiFile);
  void writeToMidiFile(std::string absFilePath);
  void writeToBinaryWithAllPossible(std::string absFilePath, int maxLength, int channel);
  std::vector<std::vector<bool>> getChannelAsVec(int insterment_number, int max_return_length);
  void printToFile(std::vector<std::vector<bool>> & thingToPrint, std::ofstream & myFile);
  void loadInBinary(std::string absBinaryPath, int channel){

    smf::MidiFile loadedInFile;
    loadedInFile.setTicksPerQuarterNote(96);

    std::ifstream infile(absBinaryPath);
    std::string curLine = "";
    int velocity = 80;
    int startTick;
    int curTrack = 0;
    int endTick;
    std::vector<std::string> curChannelVec;

    while(std::getline(infile, curLine)){
      curChannelVec.push_back(curLine);
    }

        for(int curKey = 0; curKey < (int)curChannelVec.size(); curKey++){
          for(int curTime = 0; curTime < (int)curChannelVec[curKey].size(); curTime++){
            if(curChannelVec[curKey][curTime] == '1'){
              startTick = curTime;
              endTick = curTime;
              while(endTick < (int)curChannelVec[curKey].size() && curChannelVec[curKey][endTick] == '1'){
                endTick++;
                curTime++;
              }
              loadedInFile.addNoteOn(curTrack, startTick*12, channel, curKey, velocity);
              loadedInFile.addNoteOff(curTrack, endTick*12, channel, curKey);
            }
          }
        }
            m_midiFile = loadedInFile;
            m_midiFile.sortTracks();
            m_midiFile.doTimeAnalysis();
            m_midiFile.absoluteTicks();
            m_midiFile.linkNotePairs();
      }

  private:
  //these need to be called in this exact order or operations
  void getTrackInsterments();
  void pruneTracks();
  void groupInsterments();

};

Binary_Converter::Binary_Converter(){}
Binary_Converter::Binary_Converter(smf::MidiFile & midiFile){

  m_midiFile = midiFile;

  m_midiFile.sortTracks();
  m_midiFile.doTimeAnalysis();
  m_midiFile.absoluteTicks();
  m_midiFile.linkNotePairs();

	m_numTracks = m_midiFile.getTrackCount();
	m_ticksPerQuarterNotes = m_midiFile.getTicksPerQuarterNote();

  getTrackInsterments();
  //pruneTracks();
  //groupInsterments();
  //getTrackInsterments();

	m_midiFile.sortTracks();
	m_midiFile.doTimeAnalysis();
	m_midiFile.absoluteTicks();
	m_midiFile.linkNotePairs();

	//std::cout << "DONE" << std::endl;

}

//im going to need to use merge tracks to get all the tracks that are in the same family togeather

std::vector<std::vector<bool>> getBinaryTrack(smf::MidiEventList& eventList, int maxLength, int ticksPQN){

	int key;
	int duration;
	int startTime;

  //std::cout << ticksPQN << std::endl;
  int minNote = ticksPQN / 8;
  std::vector<std::vector<bool>> ret(128, std::vector<bool>(maxLength, 0));

	for(int i = 0; i < eventList.size(); i++){

		if(eventList[i].isNoteOn()){
			key = eventList[i].getKeyNumber();
			duration = (eventList[i].getTickDuration()/minNote);// * (targetTempo/curTempo);
			startTime = (eventList[i].tick/minNote);

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

void Binary_Converter::printToFile(std::vector<std::vector<bool>> & thingToPrint, std::ofstream & myFile){
  for(int key = 0; key < (int)thingToPrint.size(); key++){
    for(int curTime = 0; curTime < (int)thingToPrint[key].size(); curTime++){
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


//This function groups all interments into there general class I divide by eight,
//that should be extracted out of this class
std::vector<std::vector<bool>> Binary_Converter::getChannelAsVec(int insterment_number, int max_return_length){


  int key;
  int duration;
  int start_time;

  int family_num = insterment_number/8;//This gives the family grouping of the interment
  int min_note = m_ticksPerQuarterNotes/8;
  std::vector<std::vector<bool>> ret_channel(128, std::vector<bool>(max_return_length, 0));//Create the return vector

    //I need an extra for loop to go over every track
    for(int cur_track = 0; cur_track < m_midiFile.getTrackCount(); cur_track++){
      for(int cur_event = 0; cur_event < m_midiFile[cur_track].size(); cur_event++){
        //Check that this is in the same family, and that is is a note on event
        if(m_midiFile[cur_track][cur_event].isNoteOn()){
          if(m_trackInsterments[cur_track]/8 == family_num){
            key = m_midiFile[cur_track][cur_event].getKeyNumber();
            duration = m_midiFile[cur_track][cur_event].getTickDuration()/min_note;
            start_time = m_midiFile[cur_track][cur_event].tick/min_note;
            for(int t = start_time; t < start_time + duration; t++){
                if (t < max_return_length){
                  ret_channel[key][t] = 1;
                }
          }
        }
      }
    }
  }
    return ret_channel;
}


void Binary_Converter::writeToBinaryWithAllPossible(std::string absFilePath, int maxLength, int channel){
  //there are 16 familys of midi isterments
  std::ofstream myfile;
  myfile.open (absFilePath);

  bool found = false;
  std::vector<std::vector<bool>> curChannel(128, std::vector<bool>(maxLength, 0));

	for(int curFamily = 0; curFamily < 16; curFamily++){

		for(int x = 0; x < (int)m_trackInsterments.size(); x++){

      if(curFamily == ((m_trackInsterments[x])/8)){

        curChannel = getBinaryTrack(m_midiFile[x], maxLength, m_ticksPerQuarterNotes);
        found = true;
        //std::cout << "get Binary Track: " << m_trackInsterments[x]  << std::endl;
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
/*
void Binary_Converter::write_to_binary_long_form(std::string absFilePath){
	//where are 16 groups of midi files
	std::ofstream myfile;
	myfile.open (absFilePath);

	int maxLength = 512;
	bool found = false;

	std::vector<std::vector<bool>> curChannel(128, std::vector<bool>(maxLength, 0));

	for(int curFamily = 0; curFamily < 16; curFamily++){

		for(int x = 0; x < (int)m_trackInsterments.size(); x++){

			if(curFamily == (m_trackInsterments[x]/8)){

				curChannel = getBinaryTrack(m_midiFile[x], maxLength, m_ticksPerQuarterNotes);
				found = true;
			}

		}
		if(found == false){
			curChannel = std::vector<std::vector<bool>>(128, std::vector<bool>(maxLength, 0));
		}

		found = false;

		printToFile(curChannel, myfile);

	}

	myfile.close();
}
*/



void Binary_Converter::groupInsterments(){

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

void Binary_Converter::getTrackInsterments(){
    m_trackInsterments = std::vector<int>(m_numTracks, -1);//all tracks to have an insterment number of -1

    for(int curChannel = 0; curChannel < m_numTracks; curChannel++){
      for(int curEvent = 0; curEvent < m_midiFile[curChannel].size(); curEvent++){
        if(m_midiFile[curChannel][curEvent].isTimbre()){
          if(m_trackInsterments[curChannel] == -1){
            if(curChannel != 0 && curChannel-1 != m_midiFile[curChannel][curEvent].getChannel()){
              m_trackInsterments[curChannel] = 1000;
            }else{
            m_trackInsterments[curChannel] =  m_midiFile[curChannel][curEvent].getP1();
            }
            //std::cout << "channel1:" << curChannel << " " << m_midiFile[curChannel][curEvent].getChannel() << " " << m_midiFile[curChannel][curEvent].getP1() << std::endl;
          }else{
            //std::cout << "channel2:" << curChannel << " " << m_midiFile[curChannel][curEvent].getChannel()  << " " << m_midiFile[curChannel][curEvent].getP1() << std::endl;
            m_midiFile[curChannel][curEvent].setP1(m_trackInsterments[curChannel]);
          }
        }
      }
    }
    //for(int x = 0; x < m_trackInsterments.size(); x++){
    //  std::cout << m_trackInsterments[x] << std::endl;
    //}
  }

  void Binary_Converter::pruneTracks(){
    for(int x = 0; x < (int)m_trackInsterments.size(); x++){
      if(m_trackInsterments[x] == -1){
        m_midiFile.deleteTrack(x);
        m_trackInsterments.erase(m_trackInsterments.begin() + x);
        x--;
      }
    }
    m_numTracks = m_midiFile.getTrackCount();
  }

  void Binary_Converter::writeToMidiFile(std::string absFilePath){
    m_midiFile.write(absFilePath);
  }


namespace styleTransfer{

	void printBinarySongToFile(std::vector<std::vector<bool>> & hotEn, std::string fileAbsName){

		std::ofstream outFile;
		outFile.open(fileAbsName);


		for(int i = 0; i < (int)hotEn.size(); i++){
			for(int k = 0; k < (int)hotEn[i].size(); k++){
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
		for(int i = 0; i < (int)hotEn.size(); i++){
			for(int k = 0; k < (int)hotEn[i].size(); k++){
				//std::cout << hotEn[i][k] << " ";
			}
			//std::cout << std::endl;
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
		for(int x = 0; x < (int)allSongs.size(); x++){
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
		for(int x = 0; x < (int)fileNames.size(); x++){

			allSongs[x] = smf::MidiFile(fileNames[x]);
			curLoadStatus = allSongs[x].status();
			allSongs[x].doTimeAnalysis();
			allSongs[x].absoluteTicks();
			allSongs[x].linkNotePairs();
			//std::cout << "Min Note: " <<  getMinNoteLength(allSongs[x]) << " Num events: " << allSongs[x][0].size() << std::endl;
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
		for(int x = 0; x < (int)allSongs.size(); x++){
			curChannelCount = allSongs[x].getTrackCount();
			ret[curChannelCount-1]++;
		}
		return ret;
	}


}
