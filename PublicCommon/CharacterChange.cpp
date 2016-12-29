#include "stdlib.h"
#include "stdio.h"
#include "CharacterChange.h"
#include "ToolsConfig.h"

CCharChange::CCharChange() :
m_c(nullptr), m_wc(nullptr)
{}

CCharChange::~CCharChange()
{
	Unit();
}


wchar_t* CCharChange::C2W(char* c, size_t len)
{
	JUDEG_RETURN((nullptr == c || len < 1), nullptr)
	Unit();
	size_t converted = 0;

	m_wc = (wchar_t*)malloc(len*sizeof(wchar_t));
	mbstowcs_s(&converted, m_wc, len, c, _TRUNCATE);
	return m_wc;
}

char* CCharChange::W2C(wchar_t* w, size_t len)
{
	JUDEG_RETURN((nullptr == w || len < 1), nullptr)
	Unit();
	size_t converted = 0;

	m_c = (char*)malloc(len*sizeof(char));
	wcstombs_s(&converted, m_c, len, w, _TRUNCATE);
	return m_c;
}

bool CCharChange::Release()
{
	return Unit();
}

bool CCharChange::Init()
{
	return Unit();
}

bool CCharChange::Unit()
{
	if (nullptr != m_wc)
	{
		free(m_wc);
		m_wc = nullptr;
	}

	if (nullptr != m_c)
	{
		free(m_c);
		m_c = nullptr;
	}

	return true;
}