#ifndef _GCM_STL_LIST_H
# define _GCM_STL_LIST_H

#include <Object.h>
#include <GC.h>
#include <list>
#include <vector>
#include <map>
#include <util/Log.h>
#include "String.h"

namespace GCM {

template <class T>
class List: public Object, public std::list< GC<T> > {
	public:
		List(): Object(), std::list< GC<T> >() {}

		virtual GC<T> append(GC<T> element) {
			this->push_back(element);
			return element;
		}

		virtual ~List() {}
};

template <class T>
class ArrayList: public Object, public std::vector< GC<T> > {
	public:
		ArrayList(): Object(), std::vector< GC<T> >() {}

		virtual GC<T> append(GC<T> element) {
			this->push_back(element);
			return element;
		}

		virtual ~ArrayList() {
		}
};

template <class Key, class T>
class Map: public Object, public std::map< Key, GC<T> > {
	public:
		Map(): Object(), std::map< Key, GC<T> >() {}
		virtual GC<T> access(Key key) { return (*this)[key]; }
		virtual GC<T> assign(Key key, GC<T> value) { (*this)[key] = value; return value; }
};

template <class Key>
class Map<Key, String>: public Object, public std::map< Key, String> {
	public:
		Map(): Object(), std::map< Key, String >() {}
		virtual String access(Key key) { return (*this)[key]; }
		virtual String assign(Key key, String value) { (*this)[key] = value; return value; }
};

}

#endif
