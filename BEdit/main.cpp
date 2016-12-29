#include "BEdit.h"
#include "ShowFbx.h"
#include "Tools/SpinSlider.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSlider>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //BEdit w;
	//ShowFbx w;
	//w.show();
	
	BEdit *window = new BEdit;
	window->setWindowTitle(QStringLiteral("QtWindow"));
	
	CSpinSlider css;
	css.Init(window);
	css.SetLabel(QStringLiteral("Light£º"));
	css.SetRange(0, 180);
	css.SetVlue(56);
	QVBoxLayout* layout = new  QVBoxLayout();
	css.InsertBoxLayout(layout);
	//layout->setGeometry(QRect(0, 0, 120, 60));
	
	window->GetWidgetLeft()->setLayout(layout);
	window->GetWidgetLeft()->setGeometry(QRect(0, 0, 120, 80));
	window->show();

	int rt = a.exec();
    return rt;
}
