#include <gcm.h>
#include <geolib/GenericWaypoint.h>
#include <stl/String.h>
#include <database/Database.h>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace GCM::geolib;
using namespace GCM;

const double GenericWaypoint::DEG2RAD = M_PI / 180.0;
const int GenericWaypoint::RADIUS = 6378135;
const double GenericWaypoint::RAD2DEG = 180.0 / M_PI;

GCM_API GenericWaypoint::GenericWaypoint(): latitudeE6(0), longitudeE6(0), dtCreated(0),
	id(), name(), comment(), type(), distance(Distance(0, 0)) {
}

GCM_API GenericWaypoint::GenericWaypoint(const GenericWaypoint &wpt):
	latitudeE6(wpt.latitudeE6), longitudeE6(wpt.longitudeE6),
	dtCreated(wpt.dtCreated), id(wpt.id), name(wpt.name), comment(wpt.comment),
	type(wpt.type), distance(Distance(0, 0)) {
}

Modifiable<int> GCM_API GenericWaypoint::getLatitudeE6() {
	return this->latitudeE6;
}

double GCM_API GenericWaypoint::getLatitude() {
	return this->latitudeE6 / 1E6;
}

void GCM_API GenericWaypoint::setLatitudeE6(int latitudeE6) {
	this->latitudeE6 = latitudeE6;
}

Modifiable<int> GCM_API GenericWaypoint::getLongitudeE6() {
	return this->longitudeE6;
}

double GCM_API GenericWaypoint::getLongitude() {
	return this->longitudeE6 / 1E6;
}

void GCM_API GenericWaypoint::setLongitudeE6(int longitudeE6) {
	this->longitudeE6 = longitudeE6;
}

Modifiable<int64_t> GCM_API GenericWaypoint::getDtCreated() {
	return this->dtCreated;
}

void GCM_API GenericWaypoint::setDtCreated(int64_t time) {
	this->dtCreated = time;
}

const Modifiable<String> GCM_API GenericWaypoint::getId() {
	return this->id;
}

void GCM_API GenericWaypoint::setId(String id) {
	this->id = id;
}

const Modifiable<String> GCM_API GenericWaypoint::getName() {
	return this->name;
}

void GCM_API GenericWaypoint::setName(String newname) {
	this->name = newname;
}

const Modifiable<String> GCM_API GenericWaypoint::getComment() {
	return this->comment;
}

void GCM_API GenericWaypoint::setComment(String comment) {
	this->comment = comment;
}

const Modifiable<String> GCM_API GenericWaypoint::getType() {
	return this->type;
}

void GCM_API GenericWaypoint::setType(String type) {
	this->type = type;
}

GC<GCM::database::Database> GCM_API GenericWaypoint::getDbProvider() {
	return this->dbProvider;
}

void GCM_API GenericWaypoint::setDbProvider(GC<GCM::database::Database> db) {
	this->dbProvider = db;
}

void GCM_API GenericWaypoint::setDistance(GenericWaypoint::Distance distance) {
	this->distance = distance;
}

void GCM_API GenericWaypoint::setDistance(int distance, int azimuth) {
	this->distance = Distance(distance, azimuth);
}

const GenericWaypoint::Distance GCM_API GenericWaypoint::getDistance() {
	return this->distance;
}

String GCM_API GenericWaypoint::formatCoords(int latE6, int lonE6, CoordsFormat format) {
	double dLat = latE6 / 1E6;
	double dLon = lonE6 / 1E6;
	int degLat, degLon, minLat, minLon;
	switch (format) {
		case FMT_DD:
			return String::format("%c%1.6f\xC2\xB0 %c%1.6f\xC2\xB0",
				dLat >= 0?'N':'S',
				dLat,
				dLon >= 0?'E':'W',
				dLon
			);
			break;

		case FMT_DM:
			degLat = dLat;
			degLon = dLon;
			return String::format("%c%02d\xC2\xB0 %06.3f' %c%03d\xC2\xB0 %06.3f'",
				dLat >= 0?'N':'S',
				degLat, (dLat - degLat) * 60,
				dLon >= 0?'E':'W',
				degLon, (dLon - degLon) * 60
			);
			break;

		case FMT_DMS:
			degLat = dLat;
			degLon = dLon;
			minLat = (dLat - degLat) * 60;
			minLon = (dLon - degLon) * 60;
			return String::format(
				"%c%02d\xC2\xB0 %d' %06.3f\" %c%03d\xC2\xB0 %d' %06.3f\"",
					dLat >= 0?'N':'S',
					degLat, minLat, ((dLat - degLat) * 60 - minLat) * 60,
					dLon >= 0?'E':'W',
					degLon, minLon, ((dLon - degLon) * 60 - minLon) * 60
			);
			break;
	}

	// This should never happen...
	return "";
}

String GCM_API GenericWaypoint::formatCoords(GenericWaypoint::CoordsFormat format) {
	return GenericWaypoint::formatCoords(this->getLatitudeE6(), this->getLongitudeE6(), format);
}

void GenericWaypoint::setDistanceFrom(int latE6, int lonE6) {
	this->setDistance(this->calcDistanceAzimuth(latE6, lonE6));
}

GenericWaypoint::Distance GCM_API GenericWaypoint::calcDistanceAzimuth(int latE61, int lonE61, int latE62, int lonE62) {
	return calcDistanceAzimuth(latE61 / 1E6, lonE61 / 1E6, latE62 / 1E6, lonE62 / 1E6);
}

GenericWaypoint::Distance GCM_API GenericWaypoint::calcDistanceAzimuth(double dLat1, double dLon1, double dLat2, double dLon2) {
	dLat1 *= DEG2RAD;
	dLon1 *= DEG2RAD;
	dLat2 *= DEG2RAD;
	dLon2 *= DEG2RAD;

	// Calculate distance
	double d = sin(dLat1) * sin(dLat2) + cos(dLat1) * cos(dLat2) * cos(dLon1 - dLon2);
	int distance = RADIUS * acos(d);

	double c = acos(sin(dLat2) * sin(dLat1) + cos(dLat2) * cos(dLat1) * cos(dLon2 - dLon1));
	double A = asin(cos(dLat2) * sin(dLon2 - dLon1) / sin(c));

	int azimuth = A * RAD2DEG * 10;

	return Distance(distance, azimuth);
}

int GCM_API GenericWaypoint::calcDistance(int latE61, int lonE61, int latE62, int lonE62) {
	Distance dd = calcDistanceAzimuth(latE61, lonE61, latE62, lonE62);
	return dd.distance;
}

int GCM_API GenericWaypoint::calcDistance(double dLat1, double dLon1, double dLat2, double dLon2) {
	Distance dd = calcDistanceAzimuth(dLat1, dLon1, dLat2, dLon2);
	return dd.distance;
}

int GCM_API GenericWaypoint::calcAzimuth(int latE61, int lonE61, int latE62, int lonE62) {
	Distance dd = calcDistanceAzimuth(latE61, lonE61, latE62, lonE62);
	return dd.azimuth;
}

int GCM_API GenericWaypoint::calcAzimuth(double dLat1, double dLon1, double dLat2, double dLon2) {
	Distance dd = calcDistanceAzimuth(dLat1, dLon1, dLat2, dLon2);
	return dd.azimuth;
}

GenericWaypoint::Distance GCM_API GenericWaypoint::calcDistanceAzimuth(int latE6, int lonE6) {
	return calcDistanceAzimuth(this->getLatitudeE6(), this->getLongitudeE6(), latE6, lonE6);
}

GenericWaypoint::Distance GCM_API GenericWaypoint::calcDistanceAzimuth(double dLat, double dLon) {
	return calcDistanceAzimuth(this->getLatitude(), this->getLongitude(), dLat, dLon);
}

int GCM_API GenericWaypoint::calcDistance(int latE6, int lonE6) {
	Distance dd = this->calcDistanceAzimuth(latE6, lonE6);
	return dd.distance;
}

int GCM_API GenericWaypoint::calcDistance(double dLat, double dLon) {
	Distance dd = this->calcDistanceAzimuth(dLat, dLon);
	return dd.distance;
}

int GCM_API GenericWaypoint::calcAzimuth(int latE6, int lonE6) {
	Distance dd = this->calcDistanceAzimuth(latE6, lonE6);
	return dd.azimuth;
}

int GCM_API GenericWaypoint::calcAzimuth(double dLat, double dLon) {
	Distance dd = this->calcDistanceAzimuth(dLat, dLon);
	return dd.azimuth;
}

String GCM_API GenericWaypoint::formatDistance(int distance, int azimuth) {
	while (azimuth < 0) azimuth += 3600;
	if (azimuth > 3600) azimuth = azimuth % 3600;

	const char *direction = NULL;
	if (azimuth < 225 || azimuth >= 3370) {
		direction = "N";
	} else if (azimuth >= 225 && azimuth < 675) {
		direction = "NE";
	} else if (azimuth >= 675 && azimuth < 1125) {
		direction = "E";
	} else if (azimuth >= 1125 && azimuth < 1575) {
		direction = "SE";
	} else if (azimuth >= 1575 && azimuth < 2025) {
		direction = "S";
	} else if (azimuth >= 2025 && azimuth < 2475) {
		direction = "SW";
	} else if (azimuth >= 2575 && azimuth < 2925) {
		direction = "W";
	} else {
		direction = "NW";
	}

	if (distance == 0) {
		return "Here";
	} else {
		return String::format("%1.1fkm %s", distance / 1000.0, direction);
	}
}

String GCM_API GenericWaypoint::formatDistance() {
	return formatDistance(this->distance.distance, this->distance.azimuth);
}
