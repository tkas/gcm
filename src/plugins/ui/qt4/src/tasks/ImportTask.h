#ifndef IMPORTTASK_H
#define IMPORTTASK_H

#include <ui.h>
#include <database/Database.h>
#include <import.h>
#include <config/AppConfig.h>

#include <QObject>

class ImportTask: public GCM::ui::BackgroundTask {
	
private:
	ptrDialog dialog;
	GCM::String fileName;
	GCM::GC<GCM::database::Database> db;
	GCM::GC<GCM::ImportInfo> import;

public:
	ImportTask(GCM::GC<GCM::ImportInfo> plugin, ptrDialog dialog, GCM::String fileName, GCM::GC<GCM::database::Database> db);
	~ImportTask();
	void onPreExecute();
	void onPostExecute();
	void doInBackground();
	void onCancelled(void *);
};

#endif // IMPORTTASK_H
