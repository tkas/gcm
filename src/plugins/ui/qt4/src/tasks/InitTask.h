#ifndef INITTASK_H
#define INITTASK_H

#include <ui.h>
#include "../forms/MainWindow.h"
#include <util/File.h>

class InitTask: public GCM::ui::BackgroundTask {
private:
	MainWindow *wnd;
	GCM::io::FileList filters;
	//GCM::io::FileList databases;

public:
	InitTask(MainWindow *mainWnd);
	void onPreExecute();
	void onPostExecute();
	void doInBackground();
};

#endif // INITTASK_H
