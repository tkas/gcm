#include <pluginmanager/Plugin.h>
#include <QApplication>
#include "main.h"

using namespace GCM::pluginmanager;

GCM::GC<GCM::ui::UI> ui;

GCM::GC<GCM::ui::UI> startUI(int argc, char **argv) {
	ui = new Qt4Ui(argc, argv);
	((Qt4Ui *)ui.getObj())->init();
	return ui;
}

PLUGIN_EXPORT void gcm_pluginInit(Plugin *plugin) {
	plugin->author = "Niximor";
	plugin->name = "qt";
	plugin->version.major = 1;
	plugin->version.minor = 1;

	plugin->manager.registerFunction(plugin, "UI", callback(startUI));
	/*QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();*/
}

PLUGIN_EXPORT void gcm_pluginDone(Plugin *plugin) {
	ui = NULL; // Destroy the UI
	QApplication::quit();
	(void)plugin;
}
