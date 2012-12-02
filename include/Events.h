#ifndef _GCM_EVENTS_H
# define _GCM_EVENTS_H

#include <Callback.h>
#include <map>
#include <list>
#include <stl/String.h>

namespace GCM {

class GCM_API Events: public Object {

	typedef std::list< BaseCallback > CallbackList;
	typedef std::map< String, CallbackList > EventList;

protected:
	EventList events;

public:
	void bind(String name, BaseCallback callback) {
		events[name].push_back(callback);
	}

	void unbind(String name, BaseCallback callback) {
		events[name].remove(callback);
	}

	void trigger(String name, void *params) {
		EventList::iterator e = events.find(name);
		if (e != events.end()) {
			CallbackList::iterator i = e->second.begin();
			while (i != e->second.end()) {
				(*i)(params);
				i++;
			}
		}
	}
};

}

#endif