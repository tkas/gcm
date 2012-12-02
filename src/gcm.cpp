#include <gcm.h>
#include <pluginmanager/PluginManager.h>
using namespace GCM::pluginmanager;

void gcmcore_setUI() {
	PluginManager::Instance()->setPreferred("UI", "qt");
}


