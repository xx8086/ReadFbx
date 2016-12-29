#include "ExtractManager.h"


CManager* CManager::m_pInstance = nullptr;
CManager::~CManager()
{
	DestroySdkObjects();
}

CManager::CGarBo::~CGarBo()
{
	if (nullptr != CManager::m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
CManager* CManager::GetInstance()
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new CManager();
		m_pInstance->CreateManager();
	}

	return m_pInstance;
}

bool CManager::CreateManager()
{
	m_pManager = FbxManager::Create();
	if (nullptr == m_pManager)
	{
		return false;
	}

	FbxIOSettings* ios = FbxIOSettings::Create(m_pManager, IOSROOT);
	m_pManager->SetIOSettings(ios);

	return true;
}

bool CManager::DestroySdkObjects()
{
	if (nullptr != m_pManager)
	{
		m_pManager->Destroy();
		m_pManager = nullptr;
	}

	return true;
}

