#include <geolib/filter/CacheName.h>
#include <stl/String.h>
#include <ctype.h>

using namespace GCM::geolib::filter;
using namespace GCM::geolib;
using namespace GCM;

GCM_API CacheName::CacheName(): matchMode(MM_IGNORED) {
}

void GCM_API CacheName::setCacheName(String name) {
	this->name = name->toLowerCase();
	if (this->matchMode == MM_CONTAINS || this->matchMode == MM_DN_CONTAINS) {
		this->updateParts();
	}
}

String GCM_API CacheName::getCacheName() {
	return this->name;
}

void GCM_API CacheName::setMatchMode(CacheName::MatchMode mm) {
	this->matchMode = mm;
	if (this->matchMode == MM_CONTAINS || this->matchMode == MM_DN_CONTAINS) {
		this->updateParts();
	}
}

CacheName::MatchMode GCM_API CacheName::getMatchMode() {
	return this->matchMode;
}

/**
 * Split the string by spaces and fill in parts.
 */
void GCM_API CacheName::updateParts() {
	this->nameParts.clear();
	const char *nm = this->name->c_str();
	size_t lastSpace = 0;
	for (size_t i = 0; i < this->name->length(); i++) {
		if (isspace(nm[i])) {
			if (lastSpace != i) {
				// Fill in part.
				this->nameParts.push_back(String(nm + lastSpace, i - lastSpace));
			}

			lastSpace = i + 1;
		}
	}

	// Last part
	if (lastSpace != this->name->length()) {
		this->nameParts.push_back(String(nm + lastSpace));
	}

	// Debug parts
	/*std::list<String>::iterator i = this->nameParts.begin();
	while (i != this->nameParts.end()) {
		GCM::util::Log::d("CacheName", (*i)->c_str());

		i++;
	}*/
}

bool GCM_API CacheName::contains(String cacheName) {
	std::list<String>::iterator i = this->nameParts.begin();

	while (i != this->nameParts.end()) {
		size_t pos = cacheName->find((*i)->c_str());
		if (pos == std::string::npos) return false;
		i++;
	}

	return true;
}

int GCM_API CacheName::getId() const {
	return CacheName::ID;
}

String GCM_API CacheName::getName() const {
	return "CacheName";
}

bool GCM_API CacheName::matchCache(GC<Geocache> cache) {
	switch (this->matchMode) {
		case MM_IGNORED: return true;
		case MM_CONTAINS: return this->contains(cache->getName()->toLowerCase());
		case MM_DN_CONTAINS: return !this->contains(cache->getName()->toLowerCase());
		case MM_BEGINS: return cache->getName()->toLowerCase()->startsWith(this->name);
		case MM_ENDS: return cache->getName()->toLowerCase()->endsWith(this->name);
		case MM_EQUALS: return cache->getName()->toLowerCase()->equals(this->name);
		default: return true;
	}
}

/**
 * Waypoints are ignored by cache filter.
 */
bool GCM_API CacheName::matchWaypoint(GC<GeocacheWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

/**
 * Generic waypoints are ignored by cache filter.
 */
bool GCM_API CacheName::matchGenericWaypoint(GC<GenericWaypoint> waypoint) {
	return true;
	(void)waypoint;
}

void GCM_API CacheName::toXml(xmlNodePtr root) {
	if (this->matchMode != MM_IGNORED) {
		xmlAddChild(root, xmlNewText((const xmlChar *)this->name->c_str()));
		switch (this->matchMode) {
			case MM_CONTAINS: xmlNewProp(root, (const xmlChar *)"mode", (const xmlChar *)"contains"); break;
			case MM_DN_CONTAINS: xmlNewProp(root, (const xmlChar *)"mode", (const xmlChar *)"notcontains"); break;
			case MM_BEGINS: xmlNewProp(root, (const xmlChar *)"mode", (const xmlChar *)"begins"); break;
			case MM_ENDS: xmlNewProp(root, (const xmlChar *)"mode", (const xmlChar *)"ends"); break;
			case MM_EQUALS: xmlNewProp(root, (const xmlChar *)"mode", (const xmlChar *)"equals"); break;
			default: break;
		}
	}
}

bool GCM_API CacheName::fromXml(xmlNodePtr root) {
	xmlChar *cMode = xmlGetProp(root, (const xmlChar *)"mode");
	if (cMode) {
		String mode = (const char *)cMode;
		xmlFree(cMode);

		if (mode->equals("contains")) this->matchMode = MM_CONTAINS;
		else if (mode->equals("notcontains")) this->matchMode = MM_DN_CONTAINS;
		else if (mode->equals("begins")) this->matchMode = MM_BEGINS;
		else if (mode->equals("ends")) this->matchMode = MM_ENDS;
		else if (mode->equals("equals")) this->matchMode = MM_EQUALS;
		else return false;

		// Load the name part.
		xmlChar *content = xmlNodeListGetString(root->doc, root->children, 1);
		if (content) {
			this->setCacheName((const char *)content);
			xmlFree(content);
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}
