#include "../src-library/styleTransfer.cpp"



int main(int argc, char** argv) {

   //both files use this as there root directory
   //std::string root = "/Users/Betty/midi/";

   //test single file to load in
   //std::string filePath = "midiSongs/Classical_Piano_piano-midi.de_MIDIRip/albeniz/alb_esp1.mid";

   //name of the file containing all midi file paths
   //std::string classicalFileName = "classicalPianoNames";

   //from the file path root/classicalFileNames get read all of the names out of the file
   //std::vector<std::string> fileNames = styleTransfer::getFileNames(classicalFileName, root);

   //create a midi file for testing the functions on one file
   //smf::MidiFile firstMidi;
   //read in the single test file
   //firstMidi.read(root + "midiSongs/AMERICANA_FOLK_www.pdmusic.org_MIDIRip/civilwar/cws19.mid");

   //Converter myConverter(fileNames);
   //Song firstSong(firstMidi);

   //std::cout << "num tracks: " << firstSong.m_numTracks << std::endl;

   //for(int x = 0; x < firstSong.m_numTracks; x++){
   //   std::cout << std::dec <<firstSong.m_trackInsterments[x] << std::endl;
   //}

   //std::cout << "TPQN: " << firstSong.m_ticksPerQuarterNotes << std::endl;

   //firstSong.write("/Users/Betty/midiOutputFile/prunedTracks.mid");

   //firstSong.writeToBinaryWithAllPossible("/Users/Betty/midiOutputFile/binaryAllTracks.txt");

   Song loadedFromBinary;
   loadedFromBinary.loadInBinary("/Users/Betty/midiOutputFile/binaryAllTracks.txt");
   std::cout << "Tracks: " << loadedFromBinary.m_midiFile.getTrackCount() << std::endl;
   loadedFromBinary.write("/Users/Betty/midiOutputFile/loadedInMidi.mid");




   //std::cout << firstSong.m_numTracks << std::endl;

   //TODO: cycle through all tracks and get that as the true ending of the midifile!
   //int lastTick = styleTransfer::getLastNoteOff(firstMidi);
   //int minNoteLength = styleTransfer::getMinNoteLength(firstMidi);
   //std::cout << "Last Tick: " << styleTransfer::getLastNoteOff(firstMidi) << std::endl;
   //int numTicksDivMinNote = lastTick/minNoteLength;



   //row and then col idx. key(128) then time (TPQN/MinNote). think about this
   //std::vector<std::vector<bool>> firstSong(128, std::vector<bool> (numTicksDivMinNote, 0));

   //fill in the vector of vectors by using the on notes!
   //styleTransfer::fillInHotEncodedMidi(firstSong, firstMidi, minNoteLength);

   //styleTransfer::printBinarySongToTerminal(firstSong);
   //styleTransfer::printBinarySongToFile(firstSong, "/Users/Betty/midiOutputFile/bSong.txt");

   //styleTransfer::convertBinaryToMidi("/Users/Betty/midiOutputFile/bSong.txt", "/Users/Betty/midiOutputFile/reRead/test.mid");


   //Read in that same file and create a midi file out of it
   //for(int i = 0; i < firstMidi[0].size(); i++){
      //if(firstMidi[0][i].isNoteOn()){
         //std::cout << "channelNib: " << firstMidi[0][i].getChannelNibble() << std::endl;
         //std::cout << "track: " << firstMidi[0][i].track << std::endl;
      //}
   //}


   //int readStatus = firstMidi.status();

   /*
   if(readStatus == 1){
      std::cout << "Read in file correct!" << std::endl;
   }else{
      std::cout << "Read in file wrong" << std::endl;
   }

   std::cout << "Number of Tracks: " << firstMidi.getTrackCount() << std::endl;
   */
   //just load in one file, and try to get it working on that one, then expand it out to others.

   return 0;
}
