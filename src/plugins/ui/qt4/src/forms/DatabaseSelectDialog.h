#ifndef DATABASESELECTDIALOG_H
#define DATABASESELECTDIALOG_H

#include <QDialog>
#include <QModelIndex>

#include "../forms/MainWindow.h"
#include "../models/DatabaseSelectModel.h"

#include <util/File.h>

namespace Ui {
	class DatabaseSelectDialog;
}

class DatabaseSelectDialog : public QDialog {
	Q_OBJECT
public:
	DatabaseSelectDialog(MainWindow *parent = 0);
	~DatabaseSelectDialog();
	GCM::io::File getDatabaseFile();
	void setForce(bool force);

protected:
	void changeEvent(QEvent *e);
	void reject();
	void accept();

private:
	Ui::DatabaseSelectDialog *ui;
	GCM::io::File browseFile;
	bool force;
	MainWindow *wnd;

public slots:
	void onItemDblClick(QModelIndex index);
	void onDbCreateClick();
	void onBrowseClick();
};

#endif // DATABASESELECTDIALOG_H
