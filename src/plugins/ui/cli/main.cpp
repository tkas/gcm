#include <gcm.h>
#include <pluginmanager/Plugin.h>
#include <GC.h>
#include <util/Log.h>
#include <Thread.h>
#include <Runnable.h>
#include <import.h>
#include <config/AppConfig.h>
#include <signal.h>

using namespace GCM;
using namespace GCM::pluginmanager;
using namespace GCM::config;

#include "CLIUI.h"

static CLIUI *ui = NULL;

void sighandler(int signum) {
	if (signum == SIGINT && ::ui != NULL) {
		::ui->quitMainLoop();
	}
}

GC<GCM::ui::UI> startUI(int argc, char **argv) {
	return ::ui = new CLIUI(argc, argv);
}

PLUGIN_EXPORT void gcm_pluginInit(Plugin *plugin) {
	plugin->author = "Niximor";
	plugin->version.major = 1;
	plugin->version.minor = 0;
	plugin->manager.registerFunction(plugin, "UI", callback(startUI));
}

