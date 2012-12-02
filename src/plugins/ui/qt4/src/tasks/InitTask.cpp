#include "InitTask.h"
#include <util/File.h>
#include <config/AppConfig.h>
#include <geolib/Filter.h>
#include <geolib/filter/MatchAll.h>

using namespace GCM::io;
using namespace GCM::config;
using namespace GCM::geolib;

class FilterXml: public FileFilter {
public:
	virtual bool accept(File dir, File file) const {
		return file->getBasename()->endsWith(".xml");
		(void)dir;
	}
};

InitTask::InitTask(MainWindow *mainWnd): wnd(mainWnd) {}

void InitTask::onPreExecute() {
	GCM::util::Log::d("Task", "InitTask");
	wnd->init();
}

void InitTask::onPostExecute() {
	FileList::iterator i = filters->begin();

	bool foundActive = false;

	File activeFilter(AppConfig::getInstance()->getString("active_filter", ""));

	if (!activeFilter->exists() || !activeFilter->isFile()) {
		activeFilter = File("filters/default.xml");
		AppConfig::getInstance()->set("active_filter", "filters/default.xml");
	}

	GCM::String afStr = activeFilter->getAbsolutePath();

	while (i != filters->end()) {
		//GCM::util::Log::d("Qt4GUI|Filters", "Filter %s, %s.", (*i)->toString()->c_str(), (*i)->getBasename(".xml")->c_str());
		wnd->addFilter(*i);

		// Load last used filter.
		if ((*i)->getAbsolutePath()->equals(afStr)) {
			if ((*i)->exists()) {
				AppConfig::getInstance()->setActiveFilter(Filter::loadFromFile((*i)->toString()));
			} else {
				AppConfig::getInstance()->setActiveFilter(new GCM::geolib::filter::MatchAll());
			}
			foundActive = true;
		}

		i++;
	}

	if (!foundActive) {
		wnd->addFilter(activeFilter);
	}

	wnd->updateCurrentFilter();
	wnd->updateCurrentDatabase();
}

void InitTask::doInBackground() {
	// Load available filters from filters/ directory.
	File filterDir = File("filters/");
	if (!filterDir->exists()) {
		filterDir->mkdirs();
	}
	this->filters = filterDir->listFiles(new FilterXml());

	// Load reference points to the combo
}
