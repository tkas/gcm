#ifndef _GCM_OBJECT_H
# define _GCM_OBJECT_H

#ifndef _WIN32
#include <pthread.h>
#else
#include "pthread.h"
#endif
#include <gcm.h>

namespace GCM {

#define synchronized(obj) obj->enterCriticalSection()
#define leave(obj) obj->leaveCriticalSection()

class GCM_API RefCount {
	public:
		pthread_mutex_t refmutex;
		int totalRefCount;
		int strongRefCount;
		
		RefCount(): totalRefCount(0), strongRefCount(0) {
			pthread_mutex_init(&refmutex, NULL);
		}
		
		~RefCount() {
			pthread_mutex_destroy(&refmutex);
		}
		
		void returnRefs();
		void lockRef();
		void unlockRef();
};

/**
 * Synchronizable object.
 */
class GCM_API Object {
	private:
		pthread_mutex_t mutex;
		RefCount *refCount;

	public:
		Object();
		void enterCriticalSection();
		void leaveCriticalSection();
		bool tryEnterCriticalSection();
		void incRefCount();
		int decRefCount();
		RefCount *getRefCount();
		
		virtual ~Object();
};

}

#endif
