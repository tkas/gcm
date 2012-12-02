#ifndef _GCM_UI_UI_H
# define _GCM_UI_UI_H 1

#include <gcm.h>
#include <GC.h>
#include <Runnable.h>
#include <ui.h>
#include <list>

#include "BackgroundTask.h"

namespace GCM {
	namespace ui {

/**
 * Abstract class that encapsulates the communication between UI
 * and the core.
 */
class GCM_API UI: public Object {
	protected:
		int argc;
		char **argv;
		std::list< GC<Runnable> > uiThreadQueue;
		std::list< GC<BackgroundTask> > uiTaskQueue;
		pthread_mutex_t taskMutex;
		bool quit;

		GC<BackgroundTask> activeTask;
	
		void tryRunNextTask();

	public:
		/**
		 * Constructor.
		 */
		UI(int argc, char **argv);

		/**
		 * Process the events in the main loop.
		 */
		virtual void processEvents();

		virtual void processTasks();

		/**
		 * Call this to terminate the execution of the application.
		 */
		virtual void quitMainLoop();

		/**
		 * Initialize the UI
		 */
		virtual void start();
		virtual void mainLoop();

		virtual void runOnUiThread(GC<Runnable> run);

		virtual void addTask(GC<BackgroundTask> run);

		/**
		 * Return the DialogBuilder, that can be used to display various
		 * information (loading, splash screen, alert, ...
		 */
		virtual ptrDialogBuilder getDialogBuilder() = 0;

		virtual ~UI();
};

	}
}

#include "DialogBuilder.h"

#endif
