#ifndef STARTUPDIALOG_H
#define STARTUPDIALOG_H

#include <QDialog>

namespace Ui {
    class StartupDialog;
}

class StartupDialog : public QDialog {
    Q_OBJECT
public:
    StartupDialog(QWidget *parent = 0);
    ~StartupDialog();

protected:
    void changeEvent(QEvent *e);

public:
	void setMessage(const char *msg);

private:
    Ui::StartupDialog *ui;
};

#endif // STARTUPDIALOG_H
