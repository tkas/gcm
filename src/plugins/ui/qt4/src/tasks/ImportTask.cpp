#include "ImportTask.h"
#include <config/AppConfig.h>
#include <Callback.h>

using namespace GCM::config;

ImportTask::ImportTask(GCM::GC<GCM::ImportInfo> plugin, ptrDialog dialog, GCM::String fileName, GCM::GC<GCM::database::Database> db):
	BackgroundTask(), dialog(dialog), fileName(fileName), db(db), import(plugin) {

		GCM::util::Log::d("ImportTask", "Created new import task for file %s", fileName->c_str());
}

ImportTask::~ImportTask() {
}

void ImportTask::onPreExecute() {
	GCM::util::Log::d("ImportTask", "onPreExecute() %s", this->fileName->c_str());

	dialog->show();
	dialog->setCancellable(true);
	dialog->events.bind("cancelled", GCM::BaseCallback(new GCM::Callback<ImportTask>(this, &ImportTask::onCancelled)));
}

void ImportTask::onPostExecute() {
	GCM::util::Log::d("ImportTask", "onPostExecute() %s", this->fileName->c_str());

	dialog->dismiss();
	dialog->events.unbind("cancelled", GCM::BaseCallback(new GCM::Callback<ImportTask>(this, &ImportTask::onCancelled)));
}

void ImportTask::doInBackground() {
	GCM::util::Log::d("ImportTask", "doInBackground() %s", this->fileName->c_str());

	// Do not start new import, if the import has already been cancelled.
	if (!dialog->isCancelled()) {
		GCM::util::Log::d("ImportTask", "Importing...");
		import->import(
			fileName->c_str(),
			AppConfig::getInstance()->getWorkingDatabase(),
			dialog
		);
	} else {
		GCM::util::Log::d("ImportTask", "Import is cancelled, skipping.");
	}
}

void ImportTask::onCancelled(void *) {
	GCM::util::Log::d("ImportTask", "Import cancelled by pressing the cancel button.");
	import->cancel();
}
