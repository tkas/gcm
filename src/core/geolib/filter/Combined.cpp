#include <geolib/filter/Combined.h>
#include <stl/String.h>

using namespace GCM::geolib::filter;
using namespace GCM::geolib;
using namespace GCM;

int GCM_API Combined::getId() const {
	return Combined::ID;
}

String GCM_API Combined::getName() const {
	return "Combined";
}

bool GCM_API Combined::matchCache(GC<Geocache> cache) {
	Combined::iterator i = this->begin();
	while (i != this->end()) {
		if (!(*i)->matchCache(cache)) return false;
		i++;
	}

	return true;
}

bool GCM_API Combined::matchWaypoint(GC<GeocacheWaypoint> waypoint) {
	Combined::iterator i = this->begin();
	while (i != this->end()) {
		if (!(*i)->matchWaypoint(waypoint)) return false;
		i++;
	}

	return true;
}

bool GCM_API Combined::matchGenericWaypoint(GC<GenericWaypoint> waypoint) {
	Combined::iterator i = this->begin();
	while (i != this->end()) {
		if (!(*i)->matchGenericWaypoint(waypoint)) return false;
		i++;
	}

	return true;
}

void GCM_API Combined::toXml(xmlNodePtr root) {
	Combined::iterator i = this->begin();
	while (i != this->end()) {
		GCM::GC<Filter> f = *i;
		xmlNodePtr fXml = xmlNewChild(root, NULL, (const xmlChar *)f->getName()->c_str(), NULL);
		f->toXml(fXml);

		i++;
	}
}

bool GCM_API Combined::fromXml(xmlNodePtr root) {
	xmlNodePtr child = root->children;
	
	// Use this to correctly load empty combined filters.	
	int loaded = 0;
	
	while (child != NULL) {
		if (child->type == XML_ELEMENT_NODE) {
			loaded++;
			GCM::GC<GCM::geolib::Filter> f = Filter::loadFromXml(child);
			if (f) {
				this->push_back(f);
			}
		}

		child = child->next;
	}

	return loaded == 0 || this->size() > 0;
}
