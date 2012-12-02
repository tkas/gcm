#ifndef _GCM_GEOLIB_REFERENCEPOINT_H
# define _GCM_GEOLIB_REFERENCEPOINT_H

#include <stl/String.h>

namespace GCM {
	namespace geolib {

class GCM_API ReferencePoint: public Object {
	protected:
		String name;
		int latitudeE6;
		int longitudeE6;
		bool temporary;
		
	public:
		ReferencePoint();
	
		void setName(String name);
		String getName();
		
		void setLatitudeE6(int latitudeE6);
		int getLatitudeE6();
		void setLongitudeE6(int longitudeE6);
		int getLongitudeE6();
		
		void setTemporary(bool temporary);
		bool isTemporary();
};

	}
}

#endif
