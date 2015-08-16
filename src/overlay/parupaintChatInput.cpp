
#include "parupaintChatInput.h"

#include <QKeyEvent>

ParupaintChatInput::ParupaintChatInput(QWidget * parent) : QLineEdit(parent)
{
	this->setFocusPolicy(Qt::ClickFocus);
	this->setObjectName("ChatEntry");
}

void ParupaintChatInput::focusInEvent(QFocusEvent*)
{
	emit focusIn();
}
void ParupaintChatInput::focusOutEvent(QFocusEvent*)
{
	emit focusOut();
}


void ParupaintChatInput::keyPressEvent(QKeyEvent * event)
{
	if(event->key() == Qt::Key_Return){
		if(!this->text().isEmpty()){
			emit returnPressed();
		}
		event->accept();
		return;
	}
	if(event->key() == Qt::Key_PageUp || event->key() == Qt::Key_PageDown){
		emit pageNavigation((event->key() == Qt::Key_PageUp), (event->modifiers() & Qt::SHIFT));
		event->accept();
		return;
	}
	QLineEdit::keyPressEvent(event);
}
