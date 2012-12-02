#ifndef DATABASERENAMEDIALOG_H
#define DATABASERENAMEDIALOG_H

#include <QDialog>

namespace Ui {
	class DatabaseRenameDialog;
}

class DatabaseRenameDialog : public QDialog {
	Q_OBJECT
public:
	DatabaseRenameDialog(QWidget *parent = 0);
	~DatabaseRenameDialog();
	QString getDatabaseName();
	void setDatabaseName(QString dbName);

protected:
	void changeEvent(QEvent *e);

private:
	Ui::DatabaseRenameDialog *ui;
};

#endif // DATABASERENAMEDIALOG_H
