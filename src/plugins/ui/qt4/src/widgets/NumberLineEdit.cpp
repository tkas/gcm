#include "NumberLineEdit.h"
#include <QKeyEvent>
#include <util/Log.h>

NumberLineEdit::NumberLineEdit(QWidget *parent): QLineEdit(parent)
{
	connect(this, SIGNAL(cursorPositionChanged(int,int)), this, SLOT(onCursorPositionChanged(int, int)));
}

void NumberLineEdit::keyPressEvent(QKeyEvent *event) {
	switch (event->key()) {
	case Qt::Key_Delete:
	case Qt::Key_Backspace: {
		QKeyEvent e(event->type(), Qt::Key_0, event->modifiers(), "0", event->isAutoRepeat(), event->count());
		QLineEdit::keyPressEvent(&e);

		if (event->key() == Qt::Key_Backspace) {
			this->setCursorPosition(this->cursorPosition() - 2);
		}

		break;
	}

	default:
		QLineEdit::keyPressEvent(event);
	}
}

void NumberLineEdit::onCursorPositionChanged(int o, int n) {
	if (n >= this->maxLength()) {
		this->setCursorPosition(this->maxLength() - 1);
	}
	(void)o;
}
