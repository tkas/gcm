#ifndef ADDOTHERNICKDIALOG_H
#define ADDOTHERNICKDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
	class AddOtherNickDialog;
}

class AddOtherNickDialog : public QDialog {
	Q_OBJECT
public:
	AddOtherNickDialog(QWidget *parent = 0);
	AddOtherNickDialog(QListWidgetItem *item, QWidget *parent = 0);
	~AddOtherNickDialog();
	QString getNick();

protected:
	void changeEvent(QEvent *e);
	void accept();

private:
	Ui::AddOtherNickDialog *ui;
	bool editMode;
	QListWidgetItem *item;
};

#endif // ADDOTHERNICKDIALOG_H
