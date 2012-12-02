#include "LoaderTask.h"
#include <config/AppConfig.h>
#include "../main.h"
#include "../ui_MainWindow.h"

using namespace GCM::config;

LoaderTask::LoaderTask(MainWindow *mainWnd, GCM::GC<GCM::geolib::Filter> filter, MainWindow::AppendMode mode): wnd(mainWnd), filter(filter), mode(mode) {}

void LoaderTask::onPreExecute() {
	this->dlg = ui->getDialogBuilder()->buildProgressDialog();
	this->dlg->setMessage("Loading geocaches...");
	this->dlg->show();
}

void LoaderTask::onPostExecute() {
	this->wnd->setCaches(list, mode);
	this->dlg->dismiss();
}

void LoaderTask::doInBackground() {
	wnd->updateStats();
	if (!filter) {
		filter = AppConfig::getInstance()->getActiveFilter();
	}

	this->list = AppConfig::getInstance()->getWorkingDatabase()->listCaches(filter, this->dlg);

	if (wnd->ui->filterGCID->text() != "") {
		wnd->filterGCID(wnd->ui->filterGCID->text());
	}

	if (wnd->ui->filterName->text() != "") {
		wnd->filterName(wnd->ui->filterName->text().toLower());
	}
}
