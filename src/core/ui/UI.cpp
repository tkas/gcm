#include <ui/UI.h>
#include <config/AppConfig.h>
#include <util/Log.h>
#include <GC.h>

#ifndef _WIN32
# include <unistd.h>
#endif

using namespace GCM::ui;
using namespace GCM::config;
using namespace GCM;

GCM_API UI::UI(int argc, char **argv): Object(), argc(argc), argv(argv), quit(false), activeTask(NULL) {
	pthread_mutex_init(&this->taskMutex, NULL);
}

GCM_API UI::~UI() {
	pthread_mutex_destroy(&this->taskMutex);
}

void GCM_API UI::processEvents() {
	std::list< GC<Runnable> >::iterator run = uiThreadQueue.begin();
	if (run != uiThreadQueue.end()) {
		(*run)->run();
		uiThreadQueue.pop_front();
	}
}

void GCM_API UI::tryRunNextTask() {
	std::list< GC<BackgroundTask> >::iterator run = uiTaskQueue.begin();
	if (run != uiTaskQueue.end()) {
		this->activeTask = (*run);
		(*run)->run();
		uiTaskQueue.pop_front();
	}
}

void GCM_API UI::processTasks() {
	// No active task
	if (!this->activeTask) {
		this->tryRunNextTask();

	// Active task that has finished.
	} else {
		if (this->activeTask->thread->tryJoin()) {
			this->activeTask->onPostExecute();
			this->activeTask = NULL;
			this->tryRunNextTask();
		}
	}
}

void GCM_API UI::quitMainLoop() {
	this->quit = true;
}

void GCM_API UI::runOnUiThread(GC<Runnable> run) {
	uiThreadQueue.push_back(run);
}

void GCM_API UI::addTask(GC<BackgroundTask> run) {
	uiTaskQueue.push_back(run);
}

void GCM_API UI::mainLoop() {
	this->processEvents();
	this->processTasks();
#ifdef _WIN32
	Sleep(10);
#else
	usleep(10000);
#endif
}

void GCM_API UI::start() {
	while (!this->quit) {
		this->mainLoop();
	}
}
