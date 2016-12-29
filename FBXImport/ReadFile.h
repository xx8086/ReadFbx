#ifndef _READFILE_FBXNODE_H
#define _READFILE_FBXNODE_H
#include "AnalyseFbx\StructDatas.h"

class CReadFile
{
public:
	CReadFile();
	~CReadFile();
public:
	bool ReadFromBDM(const char* filePath, CDatas& node);
public:
	bool Release();
	bool IsAnimater();
	bool ReadFileDatas(const char* FileName);
private:
	bool ReadMesh(const char* MeshFile);
	bool ReadAnima(const char* MeshFile);
	bool ReadMatrix(const char* MeshFile);
public:
//private:
	CMesh* m_mesh = nullptr;
	std::vector<MBDJoint> m_vceSceneJoints;
	std::vector<MBDMatrix> m_vecMatrix;//²¹¼ä¶¯»­
};
#endif

