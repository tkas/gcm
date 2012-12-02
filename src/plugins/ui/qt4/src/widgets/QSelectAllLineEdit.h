#ifndef QSELECTALLLINEEDIT_H
#define QSELECTALLLINEEDIT_H

#include <QtGui/QLineEdit>

class QSelectAllLineEdit : public QLineEdit
{
public:
	QSelectAllLineEdit(QWidget *parent = 0);

protected:
	virtual void focusInEvent(QFocusEvent *e);
};

#endif // QSELECTALLLINEEDIT_H
