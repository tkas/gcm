#include <Thread.h>
#include <stl/Exception.h>

using namespace GCM;

GCM_API Thread::Thread(): restart(false), started(false) {
	this->enterCriticalSection();
#ifdef _WIN32
	this->ended = false;
#endif
	pthread_create(&this->thread, NULL, Thread::startHelper, this);
}

GCM_API Thread::~Thread() {
}

void GCM_API Thread::join() {
	if (this->started) {
		pthread_join(this->thread, NULL);
	}
}

bool GCM_API Thread::tryJoin() {
	if (this->started) {
#ifndef _WIN32
		if (pthread_tryjoin_np(this->thread, NULL) == 0) return true;
#else
		if (this->ended) {
			this->join();
			return true;
		}
#endif
		return false;
	} else {
		return true;
	}
}

void GCM_API Thread::start() {
	if (this->restart) {
		this->enterCriticalSection();
		this->join();
		// Thread has already been started, so it needs to be restarted.
		pthread_create(&this->thread, NULL, Thread::startHelper, this);
		this->started = true;
		this->leaveCriticalSection();
	} else {
		// Proceed with thread execution
		this->started = true;
		this->leaveCriticalSection();
	}
}

void GCM_API Thread::cancel() {
	pthread_cancel(this->thread);
}

void GCM_API *Thread::startHelper(void *param) {
	Thread *thread = (Thread *)param;

	try {
		thread->enterCriticalSection();
			thread->run();
			thread->restart = true;
#ifdef _WIN32
			thread->ended = true;
#endif
		thread->leaveCriticalSection();
	} catch (Exception &e) {
		fprintf(stderr, "Unhandled exception thrown when executing the thread: %s.\n", e.what());
		e.printBacktrace(stderr);
	}

	return NULL;
}
