#ifndef LOADERTASK_H
#define LOADERTASK_H

#include <ui.h>
#include "../forms/MainWindow.h"

#include <GC.h>
#include <geolib/Filter.h>
#include <geolib/GeocacheList.h>

class LoaderTask: public GCM::ui::BackgroundTask {
public:

private:
	ptrDialog dlg;
	MainWindow *wnd;
	GCM::GC<GCM::geolib::GeocacheList> list;
	GCM::GC<GCM::geolib::Filter> filter;
	MainWindow::AppendMode mode;

public:

	LoaderTask(MainWindow *mainWnd, GCM::GC<GCM::geolib::Filter> filter = NULL, MainWindow::AppendMode mode = MainWindow::Replace);
	virtual void onPreExecute();
	virtual void onPostExecute();
	virtual void doInBackground();
};

#endif // LOADERTASK_H
