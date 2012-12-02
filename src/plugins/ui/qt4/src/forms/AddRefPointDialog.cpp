#include "AddRefPointDialog.h"
#include "ui_AddRefPointDialog.h"

#include <QMessageBox>

#include <stl/List.h>
#include <geolib/ReferencePoint.h>
#include <config/AppConfig.h>

using namespace GCM;
using namespace GCM::geolib;
using namespace GCM::config;

AddRefPointDialog::AddRefPointDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AddRefPointDialog)
{
	ui->setupUi(this);
	this->adjustSize();
	this->editMode = false;
}

AddRefPointDialog::AddRefPointDialog(GCM::GC<GCM::geolib::ReferencePoint> point, QString name, Coords c, QWidget *parent):
	QDialog(parent),
	ui(new Ui::AddRefPointDialog),
	point(point)
{
	ui->setupUi(this);
	this->adjustSize();

	this->editMode = true;

	this->ui->coordinates->setCoords(c);
	this->ui->pointName->setText(name);

	this->setWindowTitle("Modify reference point");
}

AddRefPointDialog::~AddRefPointDialog()
{
	delete ui;
}

void AddRefPointDialog::changeEvent(QEvent *e)
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

void AddRefPointDialog::accept() {
	GCM::GC< ArrayList<ReferencePoint> > points = AppConfig::getInstance()->getReferencePoints();
	ArrayList<ReferencePoint>::iterator i = points->begin();

	while (i != points->end()) {
		// Not the same point, but same name as another.
		if (this->point.getObj() != i->getObj() && (*i)->getName()->equals(this->getName().toUtf8().data())) {
			break;
		}

		i++;
	}

	if (i == points->end()) {
		QDialog::accept();
	} else {
		QMessageBox::critical(this, "Point exists", "Reference point with this name already exists.", QMessageBox::Ok);
	}
}

QString AddRefPointDialog::getName() {
	return this->ui->pointName->text();
}

Coords AddRefPointDialog::getCoords() {
	return this->ui->coordinates->getCoords();
}
