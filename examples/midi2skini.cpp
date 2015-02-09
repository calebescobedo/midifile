//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Thu Jun  6 20:41:53 PDT 2002
// Last Modified: Mon Jun 10 08:04:06 PDT 2002
// Filename:      ...sig/examples/all/midi2skini.cpp
// Web Address:   http://sig.sapp.org/examples/museinfo/midi/midi2skini.cpp
// Syntax:        C++; museinfo
//
// Description:   Description: Converts a Standard MIDI file into the 
//                SKINI data format.
//

#include "MidiFile.h"
#include "Options.h"

#ifndef OLDCPP
   #include <iomanip>
   using namespace std;
#else
   #include <iomanip.h>
#endif


// user interface variables:
Options options;
int     track    = -1;         // track to extract from (starting from 0)
int     debugQ   = 0;          // use with --debug option
int     maxcount = 100000;     // maxiumum number of notes expected

// function declarations:
void      checkOptions          (Options& opts, int argc, char** argv);
void      example               (void);
void      printMidiAsSkini      (MidiFile& midifile);
void      processEvent          (MFEvent& event, double& tempo, 
                                 double& curtime);
void      usage                 (const char* command);

//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
   checkOptions(options, argc, argv);
   MidiFile midifile;
   midifile.read(options.getArg(1));
   midifile.absoluteTime();
   midifile.joinTracks();
   printMidiAsSkini(midifile);
   return 0;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// printMidiAsSkini -- go thorough all events in the MIDI file
//    and print the data.  Input MIDI file is assumed to be in
//    type 0 format (single track).
//

void printMidiAsSkini(MidiFile& midifile) {
   int      tpq      = midifile.getTicksPerQuarterNote();
   int      oldticks = 0;          // absolute ticks of last event
   int      ticks    = 0;          // absolute ticks of current event
   double   tempo    = 120.0;      // time units will be in seconds
   MFEvent  event;                 // temporary event for printing
   double  curtime  = 0.0;         // current time in seconds
   int      i;

   for (i=0; i<midifile.getNumEvents(0); i++) {
      oldticks = ticks;
      event = midifile.getEvent(0, i);
      ticks = event.time;
      if (i>0) {
         curtime += (ticks - oldticks) * 60.0 / tempo / tpq;
      }
      processEvent(event, tempo, curtime);
   }
}



//////////////////////////////
//
// processEvent -- check for tempo markings, and print MIDI event.
//

void processEvent(MFEvent& event, double& tempo, double& curtime) {
   int i;

   if (((event.data[0] & 0xf0) == 0x80) || (((event.data[0] & 0xf0) == 0x90) && 
         (event.data[2] == 0)) ) {
      // note-off MIDI message
      if (track >= 0 && track != event.track) { return; }
      cout << "NoteOff\t\t=";
      cout.width(9);
      cout.setf(ios::left);
      cout << curtime << "\t" << event.track << "\t"
           << (int)event.data[1] << "\t" << (int)event.data[2] << endl;
   } else if ((event.data[0] & 0xf0) == 0x90) {
      // note-on MIDI message
      if (track >= 0 && track != event.track) { return; }
      cout << "NoteOn\t\t=";
      cout.width(9);
      cout.setf(ios::left);
      cout << curtime << "\t" << event.track << "\t"
           << (int)event.data[1] << "\t" << (int)event.data[2] << endl;
   } else if ((event.data[0] & 0xf0) == 0xb0) {
      // continuous controller MIDI message
      if (track >= 0 && track != event.track) { return; }
      if (event.data[1] == 7) {
         cout << "Volume\t\t=";
         cout.width(9);
         cout.setf(ios::left);
         cout << curtime << "\t" << event.track << "\t"
              << "\t" << (int)event.data[2] << endl;
      } else {
         cout << "ControlChange\t=";
         cout.width(9);
         cout.setf(ios::left);
         cout << curtime << "\t" << event.track << "\t"
              << (int)event.data[1] << "\t" << (int)event.data[2] << endl;
      }
   } else if ((event.data.getSize() > 3) && (event.data[0] == 0xff) &&
         (event.data[1] == 0x51)) {
      // Tempo meta event
      int microseconds = (unsigned int)event.data[3];
      microseconds = microseconds << 8;
      microseconds |= (unsigned int)event.data[4];
      microseconds = microseconds << 8;
      microseconds |= (unsigned int)event.data[5];
      tempo = 1000000.0 / microseconds * 60.0;
      cout << "// time:=" << curtime << " tempo: " << tempo << endl;
   } else if ((event.data.getSize() > 3) && (event.data[0] == 0xff) &&
         (event.data[1] == 0x03)) {
      // Track name MIDI meta event
      if (track >= 0 && track != event.track) { return; }
      cout << "//";
      cout << " time:=" << curtime;
      cout << " track:" << event.track;
      cout << " text: ";
      for (i=3; i<event.data.getSize(); i++) {
         cout << (char)event.data[i];
      }
      cout << endl;
   } else {
      // a MIDI event with an unknown mapping into SKINI, print as comment
      if (track >= 0 && track != event.track) { return; }
      cout << "//"
           << " time:=" << curtime 
           << " track:" << event.track 
           << " midi-data: ";
      for (i=0; i<event.data.getSize(); i++) {
           cout << (int)event.data[i] << " ";
      }
      cout << endl;
   }
}



//////////////////////////////
//
// checkOptions -- process command-line options.
//

void checkOptions(Options& opts, int argc, char* argv[]) {
   opts.define("author=b",  "author of program"); 
   opts.define("version=b", "compilation info");
   opts.define("example=b", "example usages");   
   opts.define("h|help=b",  "short description");
   opts.define("t|track=i:-1", "which track to extract");
   opts.define("max=i:100000", "maximum number of notes expected in input");
   opts.define("debug=b",  "debug mode to find errors in input file");
   opts.process(argc, argv);
   
   // handle basic options:
   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
           << "craig@ccrma.stanford.edu, 6 Jun 2002" << endl;
      exit(0);
   } else if (opts.getBoolean("version")) {
      cout << argv[0] << ", version: 10 Jun 2002" << endl;
      cout << "compiled: " << __DATE__ << endl;
      exit(0);
   } else if (opts.getBoolean("help")) {
      usage(opts.getCommand());
      exit(0);
   } else if (opts.getBoolean("example")) {
      example();
      exit(0);
   }

   track    = opts.getInteger("track");
   debugQ   = opts.getBoolean("debug");
   maxcount = opts.getInteger("max"); 

   if (opts.getArgCount() != 1) {
      usage(opts.getCommand());
      exit(1);
   }
}



//////////////////////////////
//
// example -- give example calls to the program.
//

void example(void) {
   cout <<
   "midi2skini file.mid              == convert file.mid to the SKINI format\n"
   "midi2skini file.mid >file.skini  == save output SKINI data to file.skini\n"
   "midi2skini -t1 file.mid          == extract only track 1 from the tdata\n"
   ;
}



//////////////////////////////
//
// usage --
//

void usage(const char* command) {
   cout << "Usage: " << command << " [-t #|-m #] midifile\n";
   cout << "   -t #  == extract only specified track (offset from 0).\n";
   cout << "   -m #  == number of events to allocate for MIDI \n";
   cout << endl;
}



// md5sum: 98a724a80b9410bc053e07d72fc0c8d1 midi2skini.cpp [20110711]
