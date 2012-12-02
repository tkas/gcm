#ifndef _GCM_GEOLIB_ATTRIBUTESET_H
# define _GCM_GEOLIB_ATTRIBUTESET_H

#include "Attribute.h"
#include "GC.h"
#include <map>

namespace GCM {
	namespace geolib {

class GCM_API AttributeSet: public Object, public std::map< int, GC<Attribute> > {
	public:
		AttributeSet();
		virtual ~AttributeSet();
		virtual void append(GC<Attribute> attribute);
};

	}
}

#endif
