#include "BEdit.h"

BEdit::BEdit(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	chatWidget = new QWidget;
	chatWidget->hide();
	chatWidget->setStyleSheet("background:red");
	QObject::connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(showOtherWidget()));
}

void BEdit::showOtherWidget()
{
	chatWidget->show();
}

QWidget* BEdit::GetWidgetLeft()
{
	return ui.widget_left;
}

QWidget * BEdit::GetWidgetMiddle()
{
	return ui.widget_middle;
}

QWidget * BEdit::GetWidgetRight()
{
	return ui.widget_right;
}

QWidget * BEdit::GetWidgetBttom()
{
	return ui.widget_bottom;
}