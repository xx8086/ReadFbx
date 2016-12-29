
#include "..\..\PublicCommon\ToolsConfig.h"
#include "SpinSlider.h"



CSpinSlider::CSpinSlider():
m_minVlue(0), m_maxVlue(99)
{
	
	m_pLabel = Q_NULLPTR;
	m_pSpinBox = Q_NULLPTR;
	m_pSlider = Q_NULLPTR;
}

CSpinSlider::~CSpinSlider()
{
	Unint();
}

bool CSpinSlider::Unint()
{
	if (Q_NULLPTR == m_pLabel)
	{
		delete m_pLabel;
		m_pLabel = Q_NULLPTR;
	}

	if (Q_NULLPTR == m_pSpinBox)
	{
		delete m_pSpinBox;
		m_pSpinBox = Q_NULLPTR;
	}

	if (Q_NULLPTR == m_pSlider)
	{
		delete m_pSlider;
		m_pSlider = Q_NULLPTR;
	}

	return true;
}

bool CSpinSlider::Init(QWidget *parent)
{
	Unint();
	m_pLabel = new QLabel(parent);
	m_pSpinBox = new QSpinBox(parent);
	m_pSlider = new QSlider(Qt::Horizontal, parent);

	QObject::connect(m_pSpinBox, SIGNAL(valueChanged(int)), m_pSlider, SLOT(setValue(int)));
	QObject::connect(m_pSlider, SIGNAL(valueChanged(int)), m_pSpinBox, SLOT(setValue(int)));

	return true;
}

bool CSpinSlider::SetLabel(const QString & qs)
{
	m_pLabel->setText(qs);
	return true;
}

bool CSpinSlider::SetRange(int min, int max)
{
	JUDEG_RETURN((min >= max), false)

	m_minVlue = min;
	m_maxVlue = max;
	m_pSpinBox->setRange(m_minVlue, m_maxVlue);
	m_pSlider->setRange(m_minVlue, m_maxVlue);
}

bool CSpinSlider::SetVlue(int vlue)
{
	JUDEG_RETURN((!m_pSpinBox), false)
	JUDEG_RETURN((vlue < m_minVlue || m_maxVlue < vlue), false)
	
	m_pSpinBox->setValue(vlue);
	return true;
}

int  CSpinSlider:: GetCurrentVlue()
{
	return m_pSpinBox->value();
}

bool CSpinSlider::InsertBoxLayout(QBoxLayout* layout)
{
	JUDEG_RETURN(!layout, false)
	
	layout->addWidget(m_pLabel);
	layout->addWidget(m_pSpinBox);
	layout->addWidget(m_pSlider);
	return true;
}