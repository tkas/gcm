#include <geolib/Attribute.h>

using namespace GCM::geolib;

GCM_API Attribute::Attribute(): Object(), id(0), positive(false) {
}

GCM_API Attribute::Attribute(int id, bool positive): Object(), id(id), positive(positive) {
}

GCM_API Attribute::~Attribute() {
}

int GCM_API Attribute::getId() {
	return this->id;
}

void GCM_API Attribute::setId(int id) {
	this->id = id;
}

bool GCM_API Attribute::isPositive() {
	return this->positive;
}

void GCM_API Attribute::setPositive(bool positive) {
	this->positive = positive;
}

Attribute::AttributeList GCM_API Attribute::knownAttributes;
bool GCM_API Attribute::knownAttributesLoaded = false;

// In future, this will be loaded from XML file supplied by Groundspeak.
void GCM_API Attribute::loadKnownAttributes() {
	knownAttributes[1] = "Dogs";
	knownAttributes[2] = "Access or parking fee";
	knownAttributes[3] = "Climbing gear";
	knownAttributes[4] = "Boat";
	knownAttributes[5] = "Scuba gear";
	knownAttributes[6] = "Recommended for kids";
	knownAttributes[7] = "Takes less than an hour";
	knownAttributes[8] = "Scenic view";
	knownAttributes[9] = "Significant Hike";
	knownAttributes[10] = "Difficult climbing";
	knownAttributes[11] = "May require wading";
	knownAttributes[12] = "May require swimming";
	knownAttributes[13] = "Available at all times";
	knownAttributes[14] = "Recommended at night";
	knownAttributes[15] = "Available during winter";
	knownAttributes[16] = "Cactus";
	knownAttributes[17] = "Poison plants";
	knownAttributes[18] = "Dangerous Animals";
	knownAttributes[19] = "Ticks";
	knownAttributes[20] = "Abandoned mines";
	knownAttributes[21] = "Cliff / falling rocks";
	knownAttributes[22] = "Hunting";
	knownAttributes[23] = "Dangerous area";
	knownAttributes[24] = "Wheelchair accessible";
	knownAttributes[25] = "Parking available";
	knownAttributes[26] = "Public transportation";
	knownAttributes[27] = "Drinking water nearby";
	knownAttributes[28] = "Public restrooms nearby";
	knownAttributes[29] = "Telephone nearby";
	knownAttributes[30] = "Picnic tables nearby";
	knownAttributes[31] = "Camping available";
	knownAttributes[32] = "Bicycles";
	knownAttributes[33] = "Motorcycles";
	knownAttributes[34] = "Quads";
	knownAttributes[35] = "Off-road vehicles";
	knownAttributes[36] = "Snowmobiles";
	knownAttributes[37] = "Horses";
	knownAttributes[38] = "Campfires";
	knownAttributes[39] = "Thorns";
	knownAttributes[40] = "Stealth required";
	knownAttributes[41] = "Stroller accessible";
	knownAttributes[42] = "Needs maintenance";
	knownAttributes[43] = "Watch for livestock";
	knownAttributes[44] = "Flashlight required";
	knownAttributes[45] = "Lost And Found Tour";
	knownAttributes[46] = "Truck Driver/RV";
	knownAttributes[47] = "Field Puzzle";
	knownAttributes[48] = "UV Light Required";
	knownAttributes[49] = "Snowshoes";
	knownAttributes[50] = "Cross Country Skis";
	knownAttributes[51] = "Special Tool Required";
	knownAttributes[52] = "Night Cache";
	knownAttributes[53] = "Park and Grab";
	knownAttributes[54] = "Abandoned Structure";
	knownAttributes[55] = "Short hike (less than 1km)";
	knownAttributes[56] = "Medium hike (1km-10km)";
	knownAttributes[57] = "Long Hike (+10km)";
	knownAttributes[58] = "Fuel Nearby";
	knownAttributes[59] = "Food Nearby";
	knownAttributes[60] = "Wireless Beacon";
	knownAttributes[61] = "Partnership Cache";
	knownAttributes[62] = "Seasonal Access";
	knownAttributes[63] = "Tourist Friendly";
	knownAttributes[64] = "Tree Climbing";
	knownAttributes[65] = "Front Yard (Private Residence)";
	knownAttributes[66] = "Teamwork Required";
}

Attribute::AttributeList GCM_API &Attribute::getKnownAttributes() {
	if (!knownAttributesLoaded) loadKnownAttributes();
	return knownAttributes;
}
