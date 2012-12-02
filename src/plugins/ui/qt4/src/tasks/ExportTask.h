#ifndef EXPORTTASK_H
#define EXPORTTASK_H

#include <GC.h>
#include <export.h>
#include <ui.h>
#include "../widgets/CacheTreeView.h"

class ExportTask: public GCM::ui::BackgroundTask {
private:
	GCM::GC<GCM::ExportInfo> ei;
	ptrDialog progress;
	CacheTreeView *tree;
	GCM::io::File out;

public:
	ExportTask(CacheTreeView *tree, GCM::GC<GCM::ExportInfo> ei, GCM::io::File out);
	void onPreExecute();
	void onPostExecute();
	void doInBackground();
};

#endif // EXPORTTASK_H
