#ifndef _GCM_GC_H
#define _GCM_GC_H 1

#include <gcm.h>
#include <cstddef>
#include <stdlib.h>
#include <stdio.h>
#include <Object.h>
#include <stl/Exception.h>

namespace GCM {

#define GC_DEFAULT_CONSTRUCTORS(T, S) \
	T(S *obj): GC<S>(obj) {}

template<class T> class GC;

class NullPointerException: public Exception {
	public:
		const char *what() const throw() {
			return "Exception thrown while trying to work with NULL pointer.";
		}
};

/**
 * Garbage-collectable object. The type T must derive from Object base class
 * that contains the reference counter.
 */
template<class T> class WeakGC {
	protected:
		/**
		 * Instance of the object, that should be automatically managed.
		 */
		T *mObj;
		RefCount *refCount;

	public:
		WeakGC(): mObj(NULL), refCount(NULL) {
		}

		WeakGC(const T *obj): mObj((T *)obj) {
			if (this->mObj != NULL) {
				this->refCount = this->mObj->getRefCount();
			} else {
				this->refCount = NULL;
			}
		}

		WeakGC(const GC<T> &obj): mObj(obj.getObj()) {
			if (this->mObj != NULL) {
				this->refCount = this->mObj->getRefCount();
			} else {
				this->refCount = NULL;
			}
		}

		WeakGC(const WeakGC<T> &obj): mObj(obj.mObj) {
			if (this->mObj != NULL) {
				this->refCount = this->mObj->getRefCount();
			} else {
				this->refCount = NULL;
			}
		}
		
		/**
		 * Dereference operator that is used to access the object itself.
		 */
		T *operator->() const {
			T* out = this->getObj();
			if (out == NULL || this->refCount == NULL || this->refCount->strongRefCount == 0) throw NullPointerException();
			return out;
		}
		
		T &operator*() const {
			T *out = this->getObj();
			if (out == NULL || this->refCount == NULL ||  this->refCount->strongRefCount == 0) throw NullPointerException();
			return *out;
		}

		/**
		 * Destructor. Dereferences the object and if refcount == 0, disposes
		 * the object.
		 */
		~WeakGC() {
			if (this->mObj == NULL) return;
			this->refCount->returnRefs();
		}

		T *getObj() const {
			if (mObj != NULL && this->refCount != NULL && this->refCount->strongRefCount > 0) {
				return mObj;
			} else {
				// Call NULL pointer exception here?
				return NULL;
			}

		}

		operator bool() const {
			return this->refCount != NULL && this->refCount->strongRefCount > 0;
		}

		WeakGC<T> &operator =(T *val) {
			if (this->mObj != NULL) {
				this->refCount->returnRefs();
			}

			this->mObj = val;

			if (this->mObj != NULL) {
				this->refCount = this->mObj->getRefCount();
			}

			return *this;
		}

		WeakGC<T> &operator =(const GC<T> val) {
			if (this->mObj != NULL) {
				this->refCount->returnRefs();
			}

			this->mObj = val.getObj();

			if (this->mObj != NULL) {
				this->refCount = this->mObj->getRefCount();
			}

			return *this;
		}

		WeakGC<T> &operator =(const WeakGC<T> val) {
			if (this->mObj != NULL) {
				this->refCount->returnRefs();
			}

			this->mObj = val.mObj;

			if (this->mObj != NULL) {
				this->refCount = this->mObj->getRefCount();
			}

			return *this;
		}
};

template<class T> class GC {
	protected:
		T *mObj;
		RefCount *refCount;

	public:
		GC() {
			this->mObj = NULL;
			this->refCount = NULL;
		}

		GC(const T *obj) {
			this->mObj = (T *)obj;

			if (this->mObj != NULL) {
				this->refCount = this->mObj->getRefCount();
				this->mObj->incRefCount();
			} else {
				this->refCount = NULL;
			}
		}

		GC(const GC<T> &obj) {
			this->mObj = obj.mObj;

			if (this->mObj != NULL) {
				this->refCount = this->mObj->getRefCount();
				this->mObj->incRefCount();
			} else {
				this->refCount = NULL;
			}
		}

		GC(const WeakGC<T> &obj) {
			this->mObj = obj.getObj();
			
			if (this->mObj != NULL) {
				this->refCount = this->mObj->getRefCount();
				this->mObj->incRefCount();
			} else {
				this->refCount = NULL;
			}
		}

		/**
		 * Dereference operator that is used to access the object itself.
		 */
		T *operator->() const {
			T* out = this->getObj();
			if (out == NULL || this->refCount->strongRefCount == 0) throw NullPointerException();
			return out;
		}
		 
		T &operator*() const {
			T *out = this->getObj();
			if (out == NULL || this->refCount->strongRefCount == 0) throw NullPointerException();
			return *out;
		}

		~GC() {
			if (this->mObj != NULL) {
				this->mObj->decRefCount();
			}
		}

		T *getObj() const {
			return this->mObj;
		}

		operator bool() const {
			return this->mObj != NULL && this->refCount != NULL && this->refCount->strongRefCount > 0;
		}

		GC<T> &operator =(T *val) {
			if (this->mObj != NULL) {
				this->mObj->decRefCount();
			}

			this->mObj = val;

			if (this->mObj != NULL) {
				this->refCount = this->mObj->getRefCount();
				this->mObj->incRefCount();
			} else {
				this->refCount = NULL;
			}

			return *this;
		}

		GC<T> &operator =(const GC<T> val) {
			if (this->mObj != NULL) {
				this->mObj->decRefCount();
			}

			this->mObj = val.getObj();

			if (this->mObj != NULL) {
				this->refCount = this->mObj->getRefCount();
				this->mObj->incRefCount();
			} else {
				this->refCount = NULL;
			}

			return *this;
		}

		GC<T> &operator =(const WeakGC<T> val) {
			if (this->mObj != NULL) {
				this->mObj->decRefCount();
			}

			this->mObj = val.getObj();

			if (this->mObj != NULL) {
				this->refCount = this->mObj->getRefCount();
				this->mObj->incRefCount();
			} else {
				this->refCount = NULL;
			}

			return *this;
		}

		bool operator==(const GC<T> &item) { return mObj == item.mObj; }
};

}

#endif

