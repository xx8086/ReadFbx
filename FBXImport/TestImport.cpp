// FBXImport.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "AnalyseFbx\ExtractManager.h"
#include "AnalyseFbx\ExtractScene.h"
#include "AnalyseFbx\ExtractFbxNode.h"
#include "ReadFile.h"
#include "WriteFile.h"
#include "AnalyseFbx\StructDatas.h"

const char* pFileName = "D://Bear_005";
int _tmain(int argc, _TCHAR* argv[])
{
	std::string strFbx(pFileName);
	strFbx.append(".FBX");

	std::string strMole(pFileName);
	strMole.append(".mole");

	CManager* manger = CManager::GetInstance();
	CScene cs(manger->GetCManager(), strFbx.c_str());//pCube_Cs_01	
	CWriteFile cwf(cs.GetScene(), manger->GetCManager()/*, cs.GetFPS()*/);	
	cwf.DoFile(cs.GetRootNode());
	cwf.GetFileName(pFileName);
	cwf.WriteToBinary();

	
	CReadFile cf;
	CDatas _ReadT;
	//��ȡ���� mole�ļ��Ľӿڡ�������CDatas _ReadT�����ȡ��
	cf.ReadFromBDM(strMole.c_str(), _ReadT);

	//��ȡ�����ļ��Ľӿڡ�������CReadFile cf�����ȡ
	cf.ReadFileDatas(pFileName);
	return 0;
}

