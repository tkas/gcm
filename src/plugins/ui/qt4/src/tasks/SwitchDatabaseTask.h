#ifndef SWITCHDATABASETASK_H
#define SWITCHDATABASETASK_H

#include <ui.h>
#include "LoaderTask.h"
#include <config/AppConfig.h>
#include "../forms/MainWindow.h"
#include <util/File.h>

class SwitchDatabaseTask: public GCM::ui::BackgroundTask
{
private:
	MainWindow *wnd;
	GCM::GC<GCM::database::DatabaseInfo> info;
	GCM::io::File newDatabase;
	ptrDialog dlg;

public:
	SwitchDatabaseTask(MainWindow *wnd, GCM::GC<GCM::database::DatabaseInfo> info, GCM::io::File newDatabase);

protected:
	virtual void onPreExecute();
	virtual void doInBackground();
	virtual void onPostExecute();
};

#endif // SWITCHDATABASETASK_H
