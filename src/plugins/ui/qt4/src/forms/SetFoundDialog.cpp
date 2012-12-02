#include "SetFoundDialog.h"
#include "ui_SetFoundDialog.h"
#include <time.h>
#include <database/Database.h>

SetFoundDialog::SetFoundDialog(GCM::GC<GCM::geolib::Geocache> cache, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SetFoundDialog),
	cache(cache)
{
	ui->setupUi(this);

	if (cache->isFound()) {
		struct tm tmTime;
		time_t tmFound = cache->getDtFound();
		localtime_r(&tmFound, &tmTime);

		this->ui->foundDate->setDate(QDate(tmTime.tm_year + 1900, tmTime.tm_mon + 1, tmTime.tm_mday));
		this->ui->foundTime->setTime(QTime(tmTime.tm_hour, tmTime.tm_min, 0));
	} else {
		this->ui->foundDate->setDate(QDate::currentDate());
		this->ui->foundTime->setTime(QTime::currentTime());
	}
}

SetFoundDialog::~SetFoundDialog()
{
	delete ui;
}

void SetFoundDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void SetFoundDialog::accept() {
	if (this->ui->foundGroup->isChecked()) {
		struct tm tmTime;

		QDate dt = this->ui->foundDate->date();
		QTime tm = this->ui->foundTime->time();

		tmTime.tm_sec = 0;
		tmTime.tm_min = tm.minute();
		tmTime.tm_hour = tm.hour();
		tmTime.tm_mday = dt.day();
		tmTime.tm_mon = dt.month() - 1;
		tmTime.tm_year = dt.year() - 1900;

		time_t tmFound = mktime(&tmTime);
		this->cache->setDtFound(tmFound);

		GCM::util::Log::d("SetFoundDialog", "Set found to %ld.", tmFound);
	} else {
		this->cache->setDtFound(0);
		GCM::util::Log::d("SetFoundDialog", "Set not found.");
	}

	if (this->cache->getDbProvider()) {
		this->cache->getDbProvider()->saveCache(this->cache);
	}

	QDialog::accept();
}
