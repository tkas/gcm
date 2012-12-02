#ifndef ADDREFPOINTDIALOG_H
#define ADDREFPOINTDIALOG_H

#include <QDialog>
#include "widgets/CoordinatesWidget.h"
#include <geolib/ReferencePoint.h>

namespace Ui {
	class AddRefPointDialog;
}

class AddRefPointDialog : public QDialog {
	Q_OBJECT
public:
	AddRefPointDialog(QWidget *parent = 0);
	AddRefPointDialog(GCM::GC<GCM::geolib::ReferencePoint> point, QString, Coords, QWidget *parent = 0);
	~AddRefPointDialog();
	QString getName();
	Coords getCoords();

protected:
	void changeEvent(QEvent *e);
	void accept();

private:
	Ui::AddRefPointDialog *ui;
	GCM::GC<GCM::geolib::ReferencePoint> point;
	bool editMode;
};

#endif // ADDREFPOINTDIALOG_H
