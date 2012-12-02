#include <geolib/Filter.h>
#include <libxml/tree.h>
#include <string.h>

#include <geolib/filter/Advanced.h>
#include <geolib/filter/CacheName.h>
#include <geolib/filter/Combined.h>
#include <geolib/filter/Difficulty.h>
#include <geolib/filter/Distance.h>
#include <geolib/filter/DoesNotHaveCorrectedOrFinal.h>
#include <geolib/filter/Found.h>
#include <geolib/filter/GeocacheType.h>
#include <geolib/filter/HasCorrectedOrFinal.h>
#include <geolib/filter/MatchAll.h>
#include <geolib/filter/MyNotOwn.h>
#include <geolib/filter/MyOwn.h>
#include <geolib/filter/NotFound.h>
#include <geolib/filter/PlacedBy.h>
#include <geolib/filter/Size.h>
#include <geolib/filter/StatusArchived.h>
#include <geolib/filter/StatusAvailable.h>
#include <geolib/filter/StatusDisabled.h>
#include <geolib/filter/Terrain.h>
#include <geolib/filter/WaypointType.h>

using namespace GCM::geolib;
using namespace GCM::geolib::filter;
using namespace GCM;

const int Filter::USER = 32768;

void GCM_API Filter::saveToFile(GCM::GC<Filter> f, String fileName) {
	// Create the resulting document
	xmlDocPtr doc = xmlNewDoc((const xmlChar *)"1.0");

	// Create the root node of the document and append it to the doc.
	xmlNodePtr root = xmlNewNode(NULL, (const xmlChar *)"Filter");
	xmlDocSetRootElement(doc, root);

	// Create node for the specified filter, and save it to XML.
	xmlNodePtr filterRoot = xmlNewChild(root, NULL, (const xmlChar *)f->getName()->c_str(), NULL);
	f->toXml(filterRoot);

	// Save document to file
	xmlSaveFormatFileEnc(fileName->c_str(), doc, "utf-8", 1);

	xmlFreeDoc(doc);
}

GCM::GC<Filter> GCM_API Filter::loadFromXml(xmlNode *filterNode) {
	if (filterNode->type == XML_ELEMENT_NODE) {
		String nodeName = (const char *)filterNode->name;

		Filter *f = NULL;

		if (nodeName->equals("Advanced")) f = new Advanced();
		else if (nodeName->equals("Combined")) f = new Combined();
		else if (nodeName->equals("CacheName")) f = new CacheName();
		else if (nodeName->equals("Difficulty")) f = new Difficulty();
		else if (nodeName->equals("DoesNotHaveCorrectedOrFinal")) f = new DoesNotHaveCorrectedOrFinal();
		else if (nodeName->equals("Found")) f = new Found();
		else if (nodeName->equals("GeocacheType")) f = new GeocacheType();
		else if (nodeName->equals("HasCorrectedOrFinal")) f = new HasCorrectedOrFinal();
		else if (nodeName->equals("MatchAll")) f = new MatchAll();
		else if (nodeName->equals("MyNotOwn")) f = new MyNotOwn();
		else if (nodeName->equals("MyOwn")) f = new MyOwn();
		else if (nodeName->equals("NotFound")) f = new NotFound();
		else if (nodeName->equals("PlacedBy")) f = new PlacedBy();
		else if (nodeName->equals("Size")) f = new Size();
		else if (nodeName->equals("StatusArchived")) f = new StatusArchived();
		else if (nodeName->equals("StatusAvailable")) f = new StatusAvailable();
		else if (nodeName->equals("StatusDisabled")) f = new StatusDisabled();
		else if (nodeName->equals("Terrain")) f = new Terrain();
		else if (nodeName->equals("WaypointType")) f = new WaypointType();
		else if (nodeName->equals("Distance")) f = new Distance();
		else {
			GCM::util::Log::e("Filter", "Unable to load filter - unknown filter type %s.", nodeName->c_str());
		}

		if (f != NULL) {
			if (!f->fromXml(filterNode)) {
				GCM::util::Log::e("Filter", "Filter %s failed to load.", nodeName->c_str());
				delete f;
				f = NULL;
			}
		}

		return f;
	}

	return NULL;
}

GCM::GC<Filter> GCM_API Filter::loadFromFile(String fileName) {
	xmlDoc *doc = xmlReadFile(fileName->c_str(), "utf-8", XML_PARSE_NOENT | XML_PARSE_NONET | XML_PARSE_NOCDATA);
	if (doc == NULL) {
		GCM::util::Log::e("Filter", "Unable to load filter: %s", fileName->c_str());
		return NULL;
	}

	GCM::GC<Filter> out;

	// root node should be Filter.
	xmlNodePtr root = xmlDocGetRootElement(doc);
	if (root->type == XML_ELEMENT_NODE && strcmp((const char *)root->name, "Filter") == 0) {
		out = Filter::loadFromXml(xmlFirstElementChild(root));
	} else {
		GCM::util::Log::e("Filter", "File %s does not contain filter.", fileName->c_str());
	}

	xmlFreeDoc(doc);
	return out;
}
