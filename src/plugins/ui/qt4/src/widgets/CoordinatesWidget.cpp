#include "CoordinatesWidget.h"
#include "ui_CoordinatesWidget.h"

CoordinatesWidget::CoordinatesWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CoordinatesWidget)
{
	ui->setupUi(this);

	connect(this->ui->wgsFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(wgsFormatChanged(int)));
	this->ui->wgsFormat->setCurrentIndex(1);

	// DD
	connect(this->ui->wgsDDLat, SIGNAL(textChanged(QString)), this, SLOT(wgsDDChanged()));
	connect(this->ui->wgsDDLon, SIGNAL(textChanged(QString)), this, SLOT(wgsDDChanged()));
	connect(this->ui->wgsDDNS, SIGNAL(currentIndexChanged(int)), this, SLOT(wgsDDChanged()));
	connect(this->ui->wgsDDEW, SIGNAL(currentIndexChanged(int)), this, SLOT(wgsDDChanged()));

	// DM
	connect(this->ui->wgsDMDLat, SIGNAL(textChanged(QString)), this, SLOT(wgsDMChanged()));
	connect(this->ui->wgsDMDLon, SIGNAL(textChanged(QString)), this, SLOT(wgsDMChanged()));
	connect(this->ui->wgsDMMLat, SIGNAL(textChanged(QString)), this, SLOT(wgsDMChanged()));
	connect(this->ui->wgsDMMLon, SIGNAL(textChanged(QString)), this, SLOT(wgsDMChanged()));
	connect(this->ui->wgsDMNS, SIGNAL(currentIndexChanged(int)), this, SLOT(wgsDMChanged()));
	connect(this->ui->wgsDMEW, SIGNAL(currentIndexChanged(int)), this, SLOT(wgsDMChanged()));

	// DMS
	connect(this->ui->wgsDMSDLat, SIGNAL(textChanged(QString)), this, SLOT(wgsDMSChanged()));
	connect(this->ui->wgsDMSDLon, SIGNAL(textChanged(QString)), this, SLOT(wgsDMSChanged()));
	connect(this->ui->wgsDMSMLat, SIGNAL(textChanged(QString)), this, SLOT(wgsDMSChanged()));
	connect(this->ui->wgsDMSMLon, SIGNAL(textChanged(QString)), this, SLOT(wgsDMSChanged()));
	connect(this->ui->wgsDMSSLat, SIGNAL(textChanged(QString)), this, SLOT(wgsDMSChanged()));
	connect(this->ui->wgsDMSSLon, SIGNAL(textChanged(QString)), this, SLOT(wgsDMSChanged()));
	connect(this->ui->wgsDMSNS, SIGNAL(currentIndexChanged(int)), this, SLOT(wgsDMSChanged()));
	connect(this->ui->wgsDMSEW, SIGNAL(currentIndexChanged(int)), this, SLOT(wgsDMSChanged()));

	this->ui->wgsFormat->setCurrentIndex(1);
}

CoordinatesWidget::~CoordinatesWidget()
{
	delete ui;
}

void CoordinatesWidget::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

Coords CoordinatesWidget::getWgsDMCoords() {
	Coords out;

	//GCM::util::Log::d("WaypointManager", "wgsDMDLat = '%s'", (const char *)this->ui->wgsDMDLat->text().toLatin1());

	// Get N/S
	int ns = (this->ui->wgsDMNS->currentIndex() == 0)?1:-1;
	int dlat = atoi(this->ui->wgsDMDLat->text().toLatin1());
	out.mlat = atof(this->ui->wgsDMMLat->text().toLatin1());
	out.slat = (out.mlat - (int)out.mlat) * 60;
	out.dlat = ns * (dlat + out.mlat / 60);

	// Get E/W
	int ew = (this->ui->wgsDMEW->currentIndex() == 0)?1:-1;
	int dlon = atoi(this->ui->wgsDMDLon->text().toLatin1());
	out.mlon = atof(this->ui->wgsDMMLon->text().toLatin1());
	out.slon = (out.mlon - (int)out.mlon) * 60;
	out.dlon = ew * (dlon + out.mlon / 60);

	return out;
}

Coords CoordinatesWidget::getWgsDDCoords() {
	Coords out;

	int ns = (this->ui->wgsDDNS->currentIndex() == 0)?1:-1;
	out.dlat = ns * atof(this->ui->wgsDDLat->text().toLatin1());

	int ew = (this->ui->wgsDDEW->currentIndex() == 0)?1:-1;
	out.dlon = ew * atof(this->ui->wgsDDLon->text().toLatin1());

	out.mlat = (out.dlat - (int)out.dlat) * 60;
	out.mlon = (out.dlon - (int)out.dlon) * 60;
	out.slat = (out.mlat - (int)out.mlat) * 60;
	out.slon = (out.mlon - (int)out.mlon) * 60;

	return out;
}

Coords CoordinatesWidget::getWgsDMSCoords() {
	Coords out;

	// Get N/S
	int ns = (this->ui->wgsDMSNS->currentIndex() == 0)?1:-1;
	int dlat = atoi(this->ui->wgsDMSDLat->text().toLatin1());
	int mlat = atoi(this->ui->wgsDMSMLat->text().toLatin1());

	out.slat = atof(this->ui->wgsDMSSLat->text().toLatin1());
	out.dlat = ns * (dlat + mlat / 60 + out.slat / 3600);
	out.mlat = mlat + out.slat / 60;

	// Get E/W
	int ew = (this->ui->wgsDMSEW->currentIndex() == 0)?1:-1;
	int dlon = atoi(this->ui->wgsDMSDLon->text().toLatin1());
	int mlon = atoi(this->ui->wgsDMSMLon->text().toLatin1());

	out.slon = atof(this->ui->wgsDMSSLon->text().toLatin1());
	out.dlon = ew * (dlon + mlon / 60 + out.slon / 3600);
	out.mlon = mlon + out.slon * 60;

	return out;
}

void CoordinatesWidget::setWgsDDCoords(Coords c) {
	QString str;

	if (c.dlat < 0) {
		this->ui->wgsDDNS->setCurrentIndex(1);
		this->ui->wgsDDLat->setText(str.sprintf("%09.6lf", -1 * c.dlat));
	} else {
		this->ui->wgsDDNS->setCurrentIndex(0);
		this->ui->wgsDDLat->setText(str.sprintf("%09.6lf", c.dlat));
	}

	if (c.dlon < 0) {
		this->ui->wgsDDEW->setCurrentIndex(1);
		this->ui->wgsDDLon->setText(str.sprintf("%010.6lf", -1 * c.dlon));
	} else {
		this->ui->wgsDDEW->setCurrentIndex(0);
		this->ui->wgsDDLon->setText(str.sprintf("%010.6lf", c.dlon));
	}
}

void CoordinatesWidget::setWgsDMCoords(Coords c) {
	QString str;

	c.mlat = (c.dlat - (int)c.dlat)*60;
	c.mlon = (c.dlon - (int)c.dlon)*60;

	if (c.dlat < 0) {
		this->ui->wgsDMNS->setCurrentIndex(1);
		this->ui->wgsDMDLat->setText(str.sprintf("%02d", (int)(-1 * c.dlat)));
	} else {
		this->ui->wgsDMNS->setCurrentIndex(0);
		this->ui->wgsDMDLat->setText(str.sprintf("%02d", (int)c.dlat));
	}
	if (c.dlon < 0) {
		this->ui->wgsDMSEW->setCurrentIndex(1);
		this->ui->wgsDMDLon->setText(str.sprintf("%03d", (int)(-1 * c.dlon)));
	} else {
		this->ui->wgsDMSEW->setCurrentIndex(0);
		this->ui->wgsDMDLon->setText(str.sprintf("%03d", (int)c.dlon));
	}

	this->ui->wgsDMMLat->setText(str.sprintf("%06.3lf", c.mlat));
	this->ui->wgsDMMLon->setText(str.sprintf("%06.3lf", c.mlon));
}

void CoordinatesWidget::setWgsDMSCoords(Coords c) {
	QString str;

	c.mlat = (c.dlat - (int)c.dlat)*60;
	c.mlon = (c.dlon - (int)c.dlon)*60;
	c.slat = (c.mlat - (int)c.mlat)*60;
	c.slon = (c.mlon - (int)c.mlon)*60;

	if (c.dlat < 0) {
		this->ui->wgsDMSNS->setCurrentIndex(1);
		this->ui->wgsDMSDLat->setText(str.sprintf("%02d", (int)(-1 * c.dlat)));
	} else {
		this->ui->wgsDMSNS->setCurrentIndex(0);
		this->ui->wgsDMSDLat->setText(str.sprintf("%02d", (int)c.dlat));
	}
	if (c.dlon < 0) {
		this->ui->wgsDMSEW->setCurrentIndex(1);
		this->ui->wgsDMSDLon->setText(str.sprintf("%03d", (int)(-1 * c.dlon)));
	} else {
		this->ui->wgsDMSEW->setCurrentIndex(0);
		this->ui->wgsDMSDLon->setText(str.sprintf("%03d", (int)c.dlon));
	}

	this->ui->wgsDMSMLat->setText(str.sprintf("%02d", (int)c.mlat));
	this->ui->wgsDMSSLat->setText(str.sprintf("%06.3lf", c.slat));
	this->ui->wgsDMSMLon->setText(str.sprintf("%02d", (int)c.mlon));
	this->ui->wgsDMSSLon->setText(str.sprintf("%06.3lf", c.slon));
}

void CoordinatesWidget::wgsDMChanged() {
	if (this->ui->wgsFormat->currentIndex() != 1) return;

	Coords c = this->getWgsDMCoords();
	//GCM::util::Log::d("CoordinatesWidget", "dlat = %lf, mlat = %lf, slat = %lf, dlon = %lf, mlon = %lf, slon = %lf", c.dlat, c.mlat, c.slat, c.dlon, c.mlon, c.slon);
	this->setWgsDDCoords(c);
	this->setWgsDMSCoords(c);
}

void CoordinatesWidget::wgsDDChanged() {
	if (this->ui->wgsFormat->currentIndex() != 0) return;

	Coords c = this->getWgsDDCoords();
	this->setWgsDMCoords(c);
	this->setWgsDMSCoords(c);
}

void CoordinatesWidget::wgsDMSChanged() {
	if (this->ui->wgsFormat->currentIndex() != 2) return;

	Coords c = this->getWgsDMSCoords();
	this->setWgsDDCoords(c);
	this->setWgsDMCoords(c);
}

void CoordinatesWidget::wgsFormatChanged(int index) {
	switch (index) {
	case 1: // DM
		this->ui->wgsDD->hide();
		this->ui->wgsDM->show();
		this->ui->wgsDMS->hide();
		break;

	case 2: // DMS
		this->ui->wgsDD->hide();
		this->ui->wgsDM->hide();
		this->ui->wgsDMS->show();
		break;

	default: // DD as fallback
		this->ui->wgsDD->show();
		this->ui->wgsDM->hide();
		this->ui->wgsDMS->hide();
		break;
	}
	this->adjustSize();
}

Coords CoordinatesWidget::getCoords() {
	switch (this->ui->wgsFormat->currentIndex()) {
	case 1:
		return this->getWgsDMCoords();
	case 2:
		return this->getWgsDMSCoords();
	default:
		return this->getWgsDDCoords();
	}
}

void CoordinatesWidget::setCoords(Coords c) {
	switch (this->ui->wgsFormat->currentIndex()) {
	case 1: this->setWgsDMCoords(c); break;
	case 2: this->setWgsDMSCoords(c); break;
	default: this->setWgsDDCoords(c); break;
	}
}
