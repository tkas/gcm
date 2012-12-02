#include <ui/BackgroundTask.h>
#include <Thread.h>
#include <util/Log.h>

using namespace GCM::ui;
using namespace GCM;

class BackgroundTaskThread: public Thread {
	private:
		BackgroundTask *mTask;

	public:
		BackgroundTaskThread(BackgroundTask *task): Thread(), mTask(task) {
		}

		virtual void run() {
			this->mTask->doInBackground();
		}
};

GCM_API BackgroundTask::BackgroundTask(): Runnable() {
}

GCM_API BackgroundTask::~BackgroundTask() {
}

void GCM_API BackgroundTask::run() {
	this->thread = new BackgroundTaskThread(this);
	this->onPreExecute();
	this->thread->start();
}
