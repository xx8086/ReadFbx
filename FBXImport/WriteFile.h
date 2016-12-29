#ifndef _WRITEFILE_FBXNODE_H
#define _WRITEFILE_FBXNODE_H
#include "AnalyseFbx\ExtractFbxNode.h"

class CWriteFile
{
public:
	CWriteFile(FbxScene* secen, FbxManager* _fManager, int fps = 24);
public:
	bool DoFile(FbxNode * pRoot);
	bool WriteToBinary();
	bool GetFileName(const char* fileName);

public:
	bool WriteMesh();
	bool WriteAnim();
	bool WriteMatrix();
	bool WriteMatrix2();
private:
	bool GetHeader(MBDHeader& MH, MBDDataheader& _mdh);
private:
	std::string m_strFileName;
	CRootNode m_node;
};
#endif