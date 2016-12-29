#ifndef UI_SPINSLIDER_H
#define UI_SPINSLIDER_H

#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>

class CSpinSlider
{
public:
	CSpinSlider();
	~CSpinSlider();
private:
	bool Unint();
public:
	bool SetRange(int min, int max);
	bool Init(QWidget *parent);
	bool InsertBoxLayout(QBoxLayout* layout);
	bool SetVlue(int vlue);
	bool SetLabel(const QString & qs);
	int  GetCurrentVlue();
protected:
	QLabel *m_pLabel;;
	QSpinBox *m_pSpinBox;
	QSlider *m_pSlider;
private:
	//Qt::Orientation m_sliderOrient;
	int m_maxVlue;
	int m_minVlue;
	//int m_defultVlue;
};

/*
QSpinBox *spinBox = new QSpinBox;
QSlider *slider = new QSlider(Qt::Horizontal);
spinBox->setRange(0, 130);
slider->setRange(0, 130);


QObject::connect(spinBox, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
QObject::connect(slider, SIGNAL(valueChanged(int)), spinBox, SLOT(setValue(int)));

*/
#endif