#include "ExtractScene.h"
#include "..\..\PublicCommon\ToolsConfig.h"


#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif

CScene::CScene(FbxManager* pManager, const char* ImportFileName) :
m_pScene(nullptr)
{
	InitScene(pManager, ImportFileName);
}

CScene::~CScene()
{}

bool CScene::InitScene(FbxManager* pManager, const char* pFilename)
{	
	JUDEG_RETURN((nullptr == pManager || !FileExist(pFilename)), false)
	m_pScene = FbxScene::Create(pManager, "LHCreateScene1207");
	return LoadScene(pManager, pFilename);
}

unsigned int CScene::GetFPS()
{
	JUDEG_RETURN((nullptr == m_pScene), 24)
	int fps = FbxGetFrameRate(m_pScene->GetGlobalSettings().GetTimeMode());
	return fps;
}

FbxNode* CScene::GetRootNode() const
{
	JUDEG_RETURN((nullptr == m_pScene), nullptr)
	return m_pScene->GetRootNode();
}

bool CScene::LoadScene(FbxManager* pManager, const char* pFilename)
{
	JUDEG_RETURN((nullptr == m_pScene ||
		nullptr == pManager ||
		!FileExist(pFilename)), 
		false)

	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor, lSDKMinor, lSDKRevision;
	int i, lAnimStackCount;
	bool lStatus;
	//char lPassword[1024];


	FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);
	FbxImporter* lImporter = FbxImporter::Create(pManager, "LHImporterScene1207");
	const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
	if (!lImportStatus)//µ¼³ö´¦Ê§°Ü
	{
		FbxString error = lImporter->GetStatus().GetErrorString();
		int st = lImporter->GetStatus().GetCode();

		return false;
	}
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if (lImporter->IsFBX())
	{
		lAnimStackCount = lImporter->GetAnimStackCount();
		FbxString AnimStr = lImporter->GetActiveAnimStackName();

		for (i = 0; i < lAnimStackCount; i++)
		{
			FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);
			char* name = lTakeInfo->mName.Buffer();
			char* des = lTakeInfo->mDescription.Buffer();
			char* impor = lTakeInfo->mImportName.Buffer();
			bool bSelect = lTakeInfo->mSelect;
		}

		// Import options determine what kind of data is to be imported.
		// The default is true, but here we set the options explictly.
		IOS_REF.SetBoolProp(IMP_FBX_MATERIAL, true);
		IOS_REF.SetBoolProp(IMP_FBX_TEXTURE, true);
		IOS_REF.SetBoolProp(IMP_FBX_LINK, true);
		IOS_REF.SetBoolProp(IMP_FBX_SHAPE, true);
		IOS_REF.SetBoolProp(IMP_FBX_GOBO, true);
		IOS_REF.SetBoolProp(IMP_FBX_ANIMATION, true);
		IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	}

	// Import the scene.
	lStatus = lImporter->Import(m_pScene);
	if (lStatus == false)
	{
		int st = lImporter->GetStatus().GetCode();
	}

	// Destroy the importer
	lImporter->Destroy();
	return lStatus;
}

bool CScene::FileExist(const char* pFilename)
{
	JUDEG_RETURN((nullptr == pFilename ||
		false == FbxFileUtils::Exist(pFilename)), 
		false)

	return true;
}
const char* CScene::GetReaderOFNFilters(FbxManager * pManager)
{
	JUDEG_RETURN((!pManager), nullptr)

	int nbReaders = pManager->GetIOPluginRegistry()->GetReaderFormatCount();
	FbxString s;
	int i = 0;

	for (i = 0; i < nbReaders; i++)
	{
		s += pManager->GetIOPluginRegistry()->
			GetReaderFormatDescription(i);
		s += "|*.";
		s += pManager->GetIOPluginRegistry()->
			GetReaderFormatExtension(i);
		s += "|";
	}

	// replace | by \0
	int nbChar = int(strlen(s.Buffer())) + 1;
	char *filter = new char[nbChar];
	memset(filter, 0, nbChar);

	FBXSDK_strcpy(filter, nbChar, s.Buffer());

	for (i = 0; i < int(strlen(s.Buffer())); i++)
	{
		if (filter[i] == '|')
		{
			filter[i] = 0;
		}
	}

	return filter;
}

const char* CScene::GetWriterSFNFilters(FbxManager * pManager)
{
	int nbWriters =
		pManager->GetIOPluginRegistry()->GetWriterFormatCount();

	FbxString s;
	int i = 0;

	for (i = 0; i < nbWriters; i++)
	{
		s += pManager->GetIOPluginRegistry()->
			GetWriterFormatDescription(i);
		s += "|*.";
		s += pManager->GetIOPluginRegistry()->
			GetWriterFormatExtension(i);
		s += "|";
	}

	// replace | by \0
	int nbChar = int(strlen(s.Buffer())) + 1;
	char *filter = new char[nbChar];
	memset(filter, 0, nbChar);

	FBXSDK_strcpy(filter, nbChar, s.Buffer());

	for (i = 0; i < int(strlen(s.Buffer())); i++)
	{
		if (filter[i] == '|')
		{
			filter[i] = 0;
		}
	}

	// the caller must delete this allocated memory
	return filter;
}
