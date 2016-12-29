#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_BEdit.h"

class BEdit : public QMainWindow
{
    Q_OBJECT

public:
    BEdit(QWidget *parent = Q_NULLPTR);
public:
	QWidget * GetWidgetLeft();
	QWidget * GetWidgetMiddle();
	QWidget * GetWidgetRight();
	QWidget * GetWidgetBttom();
private slots:
	void showOtherWidget();
	
private:
	QWidget *chatWidget;
    Ui::BEditClass ui;
};
