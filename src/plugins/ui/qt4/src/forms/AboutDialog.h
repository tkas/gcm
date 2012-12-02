#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QMouseEvent>

namespace Ui {
	class AboutDialog;
}

class AboutDialog : public QDialog {
	Q_OBJECT
public:
	AboutDialog(QWidget *parent = 0);
	~AboutDialog();

protected:
	void changeEvent(QEvent *e);
	virtual void mouseEvent(QMouseEvent *);

private:
	Ui::AboutDialog *ui;

public slots:
	void onBtnCloseClick();
};

#endif // ABOUTDIALOG_H
