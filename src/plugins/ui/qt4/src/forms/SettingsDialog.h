#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <geolib/ReferencePoint.h>
#include <stl/List.h>

namespace Ui {
	class SettingsDialog;
}

class SettingsDialog : public QDialog {
	Q_OBJECT
public:
	SettingsDialog(QWidget *parent = 0);
	~SettingsDialog();

protected:
	void changeEvent(QEvent *e);
	void accept();

private:
	Ui::SettingsDialog *ui;
	GCM::GC< GCM::ArrayList<GCM::geolib::ReferencePoint> > refPoints;

public slots:
	void onBtnAddNickClick();
	void onBtnModifyNickClick();
	void onBtnRemoveNickClick();
	void onNicksSelChanged();

	void onBtnAddRefPointClick();
	void onBtnModifyRefPointClick();
	void onBtnRemoveRefPointClick();
	void onRefPtSelChanged();
};

#endif // SETTINGSDIALOG_H
