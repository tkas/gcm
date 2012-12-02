#include "SwitchDatabaseTask.h"
#include "InitTask.h"

#include <config/AppConfig.h>
#include "../main.h"

using namespace GCM::io;
using namespace GCM::config;

SwitchDatabaseTask::SwitchDatabaseTask(MainWindow *wnd, GCM::GC<GCM::database::DatabaseInfo> info, File newDatabase):
		wnd(wnd), info(info), newDatabase(newDatabase)
{
}

void SwitchDatabaseTask::onPreExecute() {
	dlg = ::ui->getDialogBuilder()->buildProgressDialog();
	dlg->setMessage("Opening database...");
	dlg->show();
}

void SwitchDatabaseTask::doInBackground() {
	GCM::GC<GCM::database::Database> db = info->open(newDatabase->toString()->c_str());

	AppConfig *cfg = AppConfig::getInstance();
	cfg->setWorkingDatabase(db);
	cfg->set("database_file", db->getConnectionString());
	cfg->set("database_format", db->getId());
}

void SwitchDatabaseTask::onPostExecute() {
	dlg->dismiss();
	::ui->addTask(new InitTask(wnd));
	::ui->addTask(new LoaderTask(wnd, AppConfig::getInstance()->getActiveFilter()));
}
