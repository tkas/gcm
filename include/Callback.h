#ifndef _GCM_CALLBACK_H
# define _GCM_CALLBACK_H

#include <gcm.h>
#include <GC.h>
#include <Object.h>

namespace GCM {

class cBaseCallback: public Object {

public:
	cBaseCallback() {};
	virtual void operator()(void *params) = 0;
	virtual bool operator==(const cBaseCallback &cb) const = 0;
};

class BaseCallback: public GC<cBaseCallback> {
public:
	BaseCallback(cBaseCallback *cb): GC<cBaseCallback>(cb) {}
	BaseCallback(const GC<cBaseCallback> &cb): GC<cBaseCallback>(cb) {}
	BaseCallback(const WeakGC<cBaseCallback> &cb): GC<cBaseCallback>(cb) {}

	virtual bool operator==(const GC<cBaseCallback> &cb) const {
		return *this->getObj() == *cb.getObj();
	}

	virtual void operator()(void *params) {
		(*(this->getObj()))(params);
	}
};

template < class T >
class Callback: public cBaseCallback {
	typedef void (T::*funcall)(void *);
	T *obj;
	funcall f;

public:
	Callback(T *obj, funcall f): cBaseCallback() {
		this->obj = obj;
		this->f = f;
	}

	virtual void operator()(void *params) {
		(obj->*f)(params);
	}

	virtual bool operator==(const cBaseCallback &cb) const {
		return this->obj == ((Callback<T>&)cb).obj && this->f == ((Callback<T>&)cb).f;
	}
};

}

#endif