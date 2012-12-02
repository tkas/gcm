#include <geolib/ReferencePoint.h>

using namespace GCM;
using namespace GCM::geolib;

GCM_API ReferencePoint::ReferencePoint(): latitudeE6(0), longitudeE6(0), temporary(false) {
}

void GCM_API ReferencePoint::setName(String name) {
	this->name = name;
}

String GCM_API ReferencePoint::getName() {
	return this->name;
}

void GCM_API ReferencePoint::setLatitudeE6(int latitudeE6) {
	this->latitudeE6 = latitudeE6;
}

int GCM_API ReferencePoint::getLatitudeE6() {
	return this->latitudeE6;
}

void GCM_API ReferencePoint::setLongitudeE6(int longitudeE6) {
	this->longitudeE6 = longitudeE6;
}

int GCM_API ReferencePoint::getLongitudeE6() {
	return this->longitudeE6;
}

void GCM_API ReferencePoint::setTemporary(bool temporary) {
	this->temporary = temporary;
}

bool GCM_API ReferencePoint::isTemporary() {
	return this->temporary;
}
