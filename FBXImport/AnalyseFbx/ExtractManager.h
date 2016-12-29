
#ifndef _EXTRACT_MANAGER_H
#define _EXTRACT_MANAGER_H
#include "BaseConfig.h"

class CManager
{
private:
	CManager(){};
	CManager(const CManager &);
	CManager& operator = (const CManager &);
public:
	~CManager();
public:
	static CManager* GetInstance();
	FbxManager* GetCManager(){ return m_pManager; };
protected:
	bool CreateManager();
	bool DestroySdkObjects();
private:
	class CGarBo
	{
	public:
		~CGarBo();
	};

private:
	static CManager* m_pInstance;
	FbxManager* m_pManager;
};

#endif // #ifndef _COMMON_H

