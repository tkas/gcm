#include "ExportTask.h"
#include "../models/CacheTreeItem.h"
#include "../main.h"

using namespace GCM::io;

ExportTask::ExportTask(CacheTreeView *tree, GCM::GC<GCM::ExportInfo> ei, File out):
	BackgroundTask(), ei(ei), tree(tree), out(out) {}

void ExportTask::onPreExecute() {
	progress = ::ui->getDialogBuilder()->buildProgressDialog();
	progress->setTitle("Exporting...");
	progress->show();
}

void ExportTask::onPostExecute() {
	progress->dismiss();
}

void ExportTask::doInBackground() {
	progress->setMessage("Building list for export...");

	// Build list from current tree
	GCM::GC<GCM::geolib::GeocacheList> list = new GCM::geolib::GeocacheList();
	for (int i = 0; i < tree->model()->rowCount(); ++i) {
		QModelIndex index = tree->model()->index(i, 0);
		if (!tree->isRowHidden(i, index.parent())) {
			CacheTreeGeocache *item = (CacheTreeGeocache *)index.internalPointer();
			list->append(item->getCache());
		}
	}

	progress->setMessage("Exporting...");
	ei->exportList(list, out, progress);
}
