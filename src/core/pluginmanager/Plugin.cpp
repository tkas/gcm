
#include <pluginmanager/Plugin.h>
#include <util/Log.h>

using namespace GCM::pluginmanager;

bool GCM_API Plugin::operator==(const char *name) const {
	return this->name == name;
}

bool GCM_API Plugin::operator<(const Plugin &plugin) const {
	return this->name < plugin.name;
}

GCM_API Plugin::~Plugin() {
}
