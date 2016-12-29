/********************************************************************************
** Form generated from reading UI file 'BEdit.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BEDIT_H
#define UI_BEDIT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BEditClass
{
public:
    QWidget *centralWidget;
    QWidget *widget_left;
    QWidget *widget_right;
    QLineEdit *lineEdit;
    QLabel *label;
    QLineEdit *lineEdit_2;
    QRadioButton *radioButton;
    QCheckBox *checkBox;
    QCheckBox *checkBox_2;
    QRadioButton *radioButton_2;
    QRadioButton *radioButton_3;
    QPushButton *pushButton;
    QWidget *widget_bottom;
    QWidget *widget_middle;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *BEditClass)
    {
        if (BEditClass->objectName().isEmpty())
            BEditClass->setObjectName(QStringLiteral("BEditClass"));
        BEditClass->resize(604, 417);
        centralWidget = new QWidget(BEditClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        widget_left = new QWidget(centralWidget);
        widget_left->setObjectName(QStringLiteral("widget_left"));
        widget_left->setGeometry(QRect(0, 0, 120, 361));
        widget_right = new QWidget(centralWidget);
        widget_right->setObjectName(QStringLiteral("widget_right"));
        widget_right->setGeometry(QRect(480, 0, 120, 361));
        lineEdit = new QLineEdit(widget_right);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(0, 30, 113, 20));
        label = new QLabel(widget_right);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(0, 10, 54, 12));
        lineEdit_2 = new QLineEdit(widget_right);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(0, 50, 113, 20));
        radioButton = new QRadioButton(widget_right);
        radioButton->setObjectName(QStringLiteral("radioButton"));
        radioButton->setGeometry(QRect(0, 100, 101, 16));
        checkBox = new QCheckBox(widget_right);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        checkBox->setGeometry(QRect(0, 150, 71, 16));
        checkBox_2 = new QCheckBox(widget_right);
        checkBox_2->setObjectName(QStringLiteral("checkBox_2"));
        checkBox_2->setGeometry(QRect(0, 170, 71, 16));
        radioButton_2 = new QRadioButton(widget_right);
        radioButton_2->setObjectName(QStringLiteral("radioButton_2"));
        radioButton_2->setGeometry(QRect(0, 80, 101, 16));
        radioButton_3 = new QRadioButton(widget_right);
        radioButton_3->setObjectName(QStringLiteral("radioButton_3"));
        radioButton_3->setGeometry(QRect(0, 120, 101, 16));
        pushButton = new QPushButton(widget_right);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(0, 200, 75, 23));
        widget_bottom = new QWidget(centralWidget);
        widget_bottom->setObjectName(QStringLiteral("widget_bottom"));
        widget_bottom->setGeometry(QRect(120, 280, 361, 80));
        widget_middle = new QWidget(centralWidget);
        widget_middle->setObjectName(QStringLiteral("widget_middle"));
        widget_middle->setGeometry(QRect(120, 0, 361, 281));
        BEditClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(BEditClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 604, 23));
        BEditClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(BEditClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        BEditClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(BEditClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        BEditClass->setStatusBar(statusBar);

        retranslateUi(BEditClass);

        QMetaObject::connectSlotsByName(BEditClass);
    } // setupUi

    void retranslateUi(QMainWindow *BEditClass)
    {
        BEditClass->setWindowTitle(QApplication::translate("BEditClass", "BEdit", 0));
        label->setText(QApplication::translate("BEditClass", "data1", 0));
        radioButton->setText(QApplication::translate("BEditClass", "RadioButton2", 0));
        checkBox->setText(QApplication::translate("BEditClass", "CheckBox1", 0));
        checkBox_2->setText(QApplication::translate("BEditClass", "CheckBox2", 0));
        radioButton_2->setText(QApplication::translate("BEditClass", "RadioButton1", 0));
        radioButton_3->setText(QApplication::translate("BEditClass", "RadioButton3", 0));
        pushButton->setText(QApplication::translate("BEditClass", "PushButton", 0));
    } // retranslateUi

};

namespace Ui {
    class BEditClass: public Ui_BEditClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BEDIT_H
