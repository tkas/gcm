#ifndef NUMBERLINEEDIT_H
#define NUMBERLINEEDIT_H

//#include <QtGui/QLineEdit>
#include <QtWidgets>

class NumberLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	NumberLineEdit(QWidget *parent = 0);

protected:
	virtual void keyPressEvent(QKeyEvent *event);

public slots:
	void onCursorPositionChanged(int, int);

};

#endif // NUMBERLINEEDIT_H
