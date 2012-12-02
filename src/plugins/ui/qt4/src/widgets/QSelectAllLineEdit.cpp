#include "QSelectAllLineEdit.h"

QSelectAllLineEdit::QSelectAllLineEdit(QWidget *parent): QLineEdit(parent)
{
}

void QSelectAllLineEdit::focusInEvent(QFocusEvent *e) {
	QLineEdit::focusInEvent(e);
	this->selectAll();
}
