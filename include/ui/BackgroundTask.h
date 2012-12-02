#ifndef _GCM_UI_BACKGROUND_TASK_H
# define _GCM_UI_BACKGROUND_TASK_H

#include <GC.h>
#include <Runnable.h>
#include <Thread.h>

namespace GCM {
	namespace ui {

/**
 * Background task is used to easily create new tasks, that should interact
 * with the UI in some way. They are placed on queue such as any other runnable,
 * but they automatically blocks processing of new tasks from queue until they
 * are finished. Rest of UI remains usable.
 * This is abstract class. Inherit from it and implement the doBackgroundJob()
 *
 */
class GCM_API BackgroundTask: public GCM::Runnable {
	friend class UI;

	private:
		GCM::GC<GCM::Thread> thread;

	public:
		BackgroundTask();
		virtual ~BackgroundTask();
		virtual void run();
		virtual void onPreExecute() = 0;
		virtual void onPostExecute() = 0;
		virtual void doInBackground() = 0;
};

	}
}

#endif

