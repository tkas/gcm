
#include <gcm.h>
#include <Object.h>
#include <pthread.h>
#include <util/Log.h>

using namespace GCM;

Object::Object(): refCount(NULL) {
	pthread_mutex_init(&mutex, NULL);
	this->refCount = new RefCount();
}

Object::~Object(){
	pthread_mutex_destroy(&mutex);
}

void GCM_API Object::enterCriticalSection() {
	pthread_mutex_lock(&mutex);
}

bool GCM_API Object::tryEnterCriticalSection() {
	return pthread_mutex_trylock(&mutex) == 0;
}

void GCM_API Object::leaveCriticalSection() {
	pthread_mutex_unlock(&mutex);
}

void GCM_API Object::incRefCount() {
	this->refCount->lockRef();
	++this->refCount->strongRefCount;
	this->refCount->unlockRef();
}

int GCM_API Object::decRefCount() {
	this->refCount->lockRef();
	int res = --this->refCount->totalRefCount;
	int out = --this->refCount->strongRefCount;
	this->refCount->unlockRef();

	if (res == 0) {
		delete this->refCount;
	}

	if (out == 0) {
		delete this;
	}

	return out;
}

RefCount GCM_API *Object::getRefCount() {
	this->refCount->lockRef();
	++this->refCount->totalRefCount;
	this->refCount->unlockRef();
	return this->refCount;
}

void GCM_API RefCount::returnRefs() {
	this->lockRef();
	if (--this->totalRefCount == 0) {
		this->unlockRef();
		delete this;
	} else {
		this->unlockRef();
	}
}

void GCM_API RefCount::lockRef() {
	pthread_mutex_lock(&refmutex);
}

void GCM_API RefCount::unlockRef() {
	pthread_mutex_unlock(&refmutex);
}

