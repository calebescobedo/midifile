//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu> 
// Creation Date: Mon Jul 26 13:10:22 PDT 2010
// Last Modified: Mon Jul 26 13:10:26 PDT 2010
// Filename:      ...sig/doc/examples/all/miditime/miditime.cpp
// Syntax:        C++
// 
// Description:   Displays the absolute tick time and absolute time
//                in seconds for MIDI events in a MIDI file, along with
//                the track information.

#include "Options.h"
#include "MidiFile.h"
#include "PerlRegularExpression.h"
#include <stdlib.h>

// function declarations:
void   checkOptions        (Options& opts);
void   example             (void);
void   usage               (const char* command);
void   processMidiFile     (MidiFile& midifile);

// user interface variables:
double starttime = 0.0;    // used with -s option
double endtime   = 0.0;    // used with -e option

///////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
   int       status;          
   MidiFile  inputfile;
   Options   options(argc, argv);

   checkOptions(options);

   status = inputfile.read(options.getArg(1));
   if (status == 0) {
      cout << "Syntax error in file: " << options.getArg(1) << "\n";
   }

   processMidiFile(inputfile);
   return 0;
}

///////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// processMidiFile -- extract track and timing information from the MIDI file.
//

void processMidiFile(MidiFile& midifile) {
   midifile.absoluteTime();
   midifile.joinTracks();
   int i, j;
   int eventcount = midifile.getEventCount(0);
   int track;
   int timeinticks;
   double timeinsecs;
   MFEvent *ptr;
   for (i=0; i<eventcount; i++) {
      ptr = &midifile.getEvent(0,i);
      track       = ptr->track;
      timeinticks = ptr->time;
      timeinsecs  = midifile.getTimeInSeconds(0, i);
      cout << timeinticks << "\t";
      cout << timeinsecs << "\t";
      cout << track << "\t";
      cout << i << "\t";
      for (j=0; j<ptr->data.getSize(); j++) {
         if (j == 0) {
            cout << "0x" << hex << (int)ptr->data[j] << dec << " "; 
         } else {
            cout << (int)ptr->data[j] << " "; 
         }
      }
      cout << endl;
   }
}



//////////////////////////////
//
// checkOptions -- handle command-line options.
//

void checkOptions(Options& opts) {

   opts.define("author=b");
   opts.define("version=b");
   opts.define("example=b");
   opts.define("help=b");
   opts.process();              

   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
           << "craig@ccrma.stanford.edu, July 2010" << endl;
      exit(0);
   }
   if (opts.getBoolean("version")) {
      cout << "midiextract version 1.0" << endl;
      cout << "compiled: " << __DATE__ << endl;
   }
   if (opts.getBoolean("help")) {
      usage(opts.getCommand());
      exit(0);
   }
   if (opts.getBoolean("example")) {
      example();
      exit(0);
   }               

}



//////////////////////////////
//
// example -- gives example calls to the midiexcerpt program.
//

void example(void) {
   cout <<
   "#                                                                        \n"
   << endl;
}
 

//////////////////////////////
//
// usage -- how to run the midiexcerpt program on the command line.
//

void usage(const char* command) {
   cout <<
   "                                                                         \n"
   << endl;               
}


// md5sum: 822f2f40a04df44843b7c9ce27b34f4a miditime.cpp [20100903]
