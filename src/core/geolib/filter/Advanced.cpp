#include <geolib/filter/Advanced.h>
#include <stl/String.h>
#include <string.h>

using namespace GCM::geolib::filter;
using namespace GCM::geolib;
using namespace GCM;

int GCM_API Advanced::getId() const {
	return Advanced::ID;
}

String GCM_API Advanced::getName() const {
	return "Advanced";
}

bool GCM_API Advanced::matchCache(GC<Geocache> cache) {
	bool result = true;

	Advanced::iterator i = this->begin();
	while (i != this->end()) {
		GC<Filter> filter = i->getFilter();
		AdvancedFilterItem::Mode mode = i->getMode();

		bool pResult = filter->matchCache(cache);

		switch (mode) {
			case AdvancedFilterItem::Initial:
				result = pResult;
				break;

			case AdvancedFilterItem::Add:
				result |= pResult;
				break;

			case AdvancedFilterItem::Subtract:
				result &= ~pResult;
				break;

			case AdvancedFilterItem::Intersect:
				result &= pResult;
				break;
		}

		i++;
	}

	return result;
}

bool GCM_API Advanced::matchWaypoint(GC<GeocacheWaypoint> waypoint) {
	bool result = true;

	Advanced::iterator i = this->begin();
	while (i != this->end()) {
		GC<Filter> filter = i->getFilter();
		AdvancedFilterItem::Mode mode = i->getMode();

		bool pResult = filter->matchWaypoint(waypoint);

		switch (mode) {
			case AdvancedFilterItem::Initial:
				result = pResult;
				break;

			case AdvancedFilterItem::Add:
				result |= pResult;
				break;

			case AdvancedFilterItem::Subtract:
				result &= ~pResult;
				break;

			case AdvancedFilterItem::Intersect:
				result &= pResult;
				break;
		}

		i++;
	}

	return result;
}

bool GCM_API Advanced::matchGenericWaypoint(GC<GenericWaypoint> waypoint) {
	bool result = true;

	Advanced::iterator i = this->begin();
	while (i != this->end()) {
		GC<Filter> filter = i->getFilter();
		AdvancedFilterItem::Mode mode = i->getMode();

		bool pResult = filter->matchGenericWaypoint(waypoint);

		switch (mode) {
			case AdvancedFilterItem::Initial:
				result = pResult;
				break;

			case AdvancedFilterItem::Add:
				result |= pResult;
				break;

			case AdvancedFilterItem::Subtract:
				result &= ~pResult;
				break;

			case AdvancedFilterItem::Intersect:
				result &= pResult;
				break;
		}

		i++;
	}

	return result;
}

void GCM_API Advanced::toXml(xmlNodePtr root) {
	Advanced::iterator i = this->begin();
	while (i != this->end()) {
		xmlNodePtr fWrapper = xmlNewChild(root, NULL, (const xmlChar *)"Filter", NULL);

		// Filter name displayed to user
		xmlNewProp(fWrapper, (const xmlChar *)"name", (const xmlChar *)i->getName()->c_str());

		// Filter mode
		switch (i->getMode()) {
			case AdvancedFilterItem::Initial:
				xmlNewProp(fWrapper, (const xmlChar *)"mode", (const xmlChar *)"initial");
				break;

			case AdvancedFilterItem::Add:
				xmlNewProp(fWrapper, (const xmlChar *)"mode", (const xmlChar *)"add");
				break;

			case AdvancedFilterItem::Subtract:
				xmlNewProp(fWrapper, (const xmlChar *)"mode", (const xmlChar *)"subtract");
				break;

			case AdvancedFilterItem::Intersect:
				xmlNewProp(fWrapper, (const xmlChar *)"mode", (const xmlChar *)"intersect");
				break;
		}

		// Actual filter content
		xmlNodePtr fXml = xmlNewChild(fWrapper, NULL, (const xmlChar *)i->getFilter()->getName()->c_str(), NULL);
		i->getFilter()->toXml(fXml);

		i++;
	}
}

bool GCM_API Advanced::fromXml(xmlNodePtr root) {
	xmlNodePtr child = root->children;
	while (child != NULL) {
		if (child->type == XML_ELEMENT_NODE && strcmp((const char *)child->name, "Filter") == 0) {
			AdvancedFilterItem::Mode mode = AdvancedFilterItem::Initial;
			xmlChar *pMode = xmlGetProp(child, (const xmlChar *)"mode");
			if (pMode) {
				if (strcmp((const char *)pMode, "initial") == 0) mode = AdvancedFilterItem::Initial;
				else if (strcmp((const char *)pMode, "add") == 0) mode = AdvancedFilterItem::Add;
				else if (strcmp((const char *)pMode, "subtract") == 0) mode = AdvancedFilterItem::Subtract;
				else if (strcmp((const char *)pMode, "intersect") == 0) mode = AdvancedFilterItem::Intersect;

				xmlFree(pMode);
			}

			xmlChar *pName = xmlGetProp(child, (const xmlChar *)"name");
			const char *name;
			if (pName) {
				name = (const char *)pName;
			} else {
				name = "Unnamed filter";
			}

			xmlNodePtr fNode = child->children;
			while (fNode != NULL) {
				if (fNode->type == XML_ELEMENT_NODE) {
					GCM::GC<GCM::geolib::Filter> f = Filter::loadFromXml(fNode);
					if (f) {
						this->push_back(AdvancedFilterItem(name, mode, f));
						break;
					}
				}
				fNode = fNode->next;
			}

			if (pName) xmlFree(pName);
		}

		child = child->next;
	}

	return this->size() > 0;
}
