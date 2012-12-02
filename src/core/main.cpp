/**
 * \file main.cpp
 *
 * GCM main
 *
 * Starts up the libgcm
 *
 * @author Michal Kuchta
 * @version 1.0
 */

#include <gcm.h>

extern void gcmcore_setUI();

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include <GC.h>
#include <Object.h>
#include <util/Log.h>
#include <stl/Exception.h>

using namespace GCM;

int main(int argc, char *argv[]) {
  //set error output to display  
  GCM::util::Log::openErrorOut((char *)"");

  //parse global options from command line
  int option_index = 0;
  int c = 0;
  static struct option long_options[] = {
    {"debug",   optional_argument, 0, 'd' },
    {0,         0,                 0, 0   }
  };

  while((c = getopt_long_only(argc, argv, "d::", long_options, &option_index)) != -1){

    switch (c) {
      case 'd':
            if(optarg) //debug output to file
              GCM::util::Log::openDebugOut(optarg);
            else //debug output to display
              GCM::util::Log::openDebugOut((char *)"");
            break;
      case '?':
            break;

    }
  }  

	try {
		gcmcore_setUI();
		return gcmcore_run(argc, argv);
	} catch (Exception &e) {
		fprintf(stderr, "Unhandled exception thrown when executing the program: %s.\n", e.what());
		e.printBacktrace(stderr);
		return EXIT_FAILURE;
	}
}

#ifdef _WIN32
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR argv, int argc) {
	return main(argc, (char **)argv);

	(void)hInstance;
	(void)hPrevInstance;
}
#endif
