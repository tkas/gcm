#include <geolib/AttributeSet.h>

using namespace GCM::geolib;
using namespace GCM;

GCM_API AttributeSet::AttributeSet() {
}

GCM_API AttributeSet::~AttributeSet() {
}

void GCM_API AttributeSet::append(GC<Attribute> attribute) {
	(*this)[attribute->getId()] = attribute;
}
