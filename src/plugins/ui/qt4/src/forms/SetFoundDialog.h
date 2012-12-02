#ifndef SETFOUNDDIALOG_H
#define SETFOUNDDIALOG_H

#include <QDialog>
#include <geolib/Geocache.h>

namespace Ui {
	class SetFoundDialog;
}

class SetFoundDialog : public QDialog {
	Q_OBJECT
public:
	SetFoundDialog(GCM::GC<GCM::geolib::Geocache> cache, QWidget *parent = 0);
	~SetFoundDialog();

protected:
	void changeEvent(QEvent *e);
	void accept();

private:
	Ui::SetFoundDialog *ui;
	GCM::GC<GCM::geolib::Geocache> cache;
};

#endif // SETFOUNDDIALOG_H
