#ifndef _EXTRACT_SCENE_H
#define _EXTRACT_SCENE_H
#include "BaseConfig.h"

class CScene
{
public:
	CScene(FbxManager* pManager, const char* ImportFileName);
	~CScene();
public:
	FbxScene* GetScene(){ return m_pScene; }
	FbxNode* GetRootNode() const;
	unsigned int GetFPS();
private:
	const char* GetReaderOFNFilters(FbxManager * pManager);
	const char *GetWriterSFNFilters(FbxManager * pManager);
private:
	bool FileExist(const char* pFilename);
	bool InitScene(FbxManager* pManager, const char* pFilename);
	bool LoadScene(FbxManager* pManager, const char* pFilename);
private:
	FbxScene* m_pScene;
	
};


#endif