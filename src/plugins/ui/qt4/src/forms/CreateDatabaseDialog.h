#ifndef CREATEDATABASEDIALOG_H
#define CREATEDATABASEDIALOG_H

#include <QDialog>
#include <database/Database.h>
#include <stl/List.h>

namespace Ui {
	class CreateDatabaseDialog;
}

class CreateDatabaseDialog : public QDialog {
	Q_OBJECT
public:
	CreateDatabaseDialog(QWidget *parent = 0);
	~CreateDatabaseDialog();
	QString getDatabaseName();
	GCM::GC<GCM::database::DatabaseInfo> getDatabaseInfo();

protected:
	void changeEvent(QEvent *e);
	virtual void accept();

private:
	Ui::CreateDatabaseDialog *ui;
	GCM::ArrayList<GCM::database::DatabaseInfo> allInfo;
};

#endif // CREATEDATABASEDIALOG_H
