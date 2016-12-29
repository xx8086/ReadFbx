#ifndef _SHOWFBX_H
#define _SHOWFBX_H

#include <QWidget>
#include "ui_ShowFbx.h"

class ShowFbx : public QWidget
{
	Q_OBJECT

public:
	ShowFbx(QWidget *parent = Q_NULLPTR);
	~ShowFbx();

private:
	Ui::ShowFbx ui;
};


#endif