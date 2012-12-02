#ifndef _GCM_GEOLIB_ATTRIBUTE_H
# define _GCM_GEOLIB_ATTRIBUTE_H

#include <Object.h>
#include <map>
#include <stl/String.h>

namespace GCM {
	namespace geolib {

class GCM_API Attribute: public Object {
	public:
		typedef std::map<int, String> AttributeList;

	private:
		int id;
		bool positive;

		static AttributeList knownAttributes;
		static bool knownAttributesLoaded;

		static void loadKnownAttributes();

	public:
		Attribute();
		Attribute(int id, bool positive);
		~Attribute();
		int getId();
		void setId(int id);
		bool isPositive();
		void setPositive(bool positive);

		static AttributeList &getKnownAttributes();
};

	}
}

#endif
