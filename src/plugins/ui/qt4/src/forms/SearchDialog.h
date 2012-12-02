#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>

namespace Ui {
	class SearchDialog;
}

class SearchDialog : public QDialog {
	Q_OBJECT
public:
	SearchDialog(QWidget *parent = 0);
	~SearchDialog();
	QString getSearchString();

protected:
	void changeEvent(QEvent *e);
	void accept();

private:
	Ui::SearchDialog *ui;
};

#endif // SEARCHDIALOG_H
