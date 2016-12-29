/********************************************************************************
** Form generated from reading UI file 'ShowFbx.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHOWFBX_H
#define UI_SHOWFBX_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ShowFbx
{
public:

    void setupUi(QWidget *ShowFbx)
    {
        if (ShowFbx->objectName().isEmpty())
            ShowFbx->setObjectName(QStringLiteral("ShowFbx"));
        ShowFbx->resize(400, 300);

        retranslateUi(ShowFbx);

        QMetaObject::connectSlotsByName(ShowFbx);
    } // setupUi

    void retranslateUi(QWidget *ShowFbx)
    {
        ShowFbx->setWindowTitle(QApplication::translate("ShowFbx", "ShowFbx", 0));
    } // retranslateUi

};

namespace Ui {
    class ShowFbx: public Ui_ShowFbx {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHOWFBX_H
