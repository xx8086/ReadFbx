#ifndef _EXTRACT_FBXNODE_H
#define _EXTRACT_FBXNODE_H
#include "BaseConfig.h"
#include "GetVerTextices.h"

class CRootNode :public CGetVerTextices
{
public:
	CRootNode(FbxScene* secen, FbxManager* _fManager, int fps = 24);
	virtual	~CRootNode();
	
public:
	bool ProcessJointHierarchy(FbxNode* inputRoot);
	bool ProcessNodeAttribute(FbxNode* pmRootNode);
	bool ProcessMeshNode(FbxNode* pmRootNode);
	bool ParseMatrix();

private:
	
	bool RecursiveChildTraversal(FbxNode* inputNode);
	bool ProcessMesh(FbxMesh* inputMesh, MBDMeshBag& mMeshDatas);

	bool ProcessMaterials(FbxMesh* inputMesh, MBDMeshBag& mMeshDatas);
	bool ProcessTextures(FbxSurfaceMaterial* pMaterial, int MaterialID);
	bool ProcessTransformations(FbxMesh* inputMesh, MBDMeshBag& mMeshDatas);
	bool ProcessJoints(FbxMesh * inputMesh, MBDMeshBag& mMeshDatas);
	
private:
	void ParseLayer(FbxNode* pNode, FbxAnimLayer* pLayer);
	void ParseChannel(FbxNode* pNode, FbxAnimLayer* pLayer);
private:
	const char* FindAndDisplayTextureInfoByProperty(const char* pName ,FbxProperty pProperty, bool& pDisplayHeader, int pMaterialIndex);
	bool CheckMaterialName(const char* materialName, MBDMeshBag& mMeshDatas);
	bool MeshCalculate(MBDMeshBag& mMeshDatas);
	bool MakeSumWeights(MBDMeshBag& mMeshDatas);
	bool ProcessInverse(FbxSkin* currSkin);
	bool ProcessKeyFrame2();
	bool ProcessKeyFrame(FbxSkin* currSkin, FbxMesh* inputMesh, MBDMeshBag& mMeshDatas);
	bool ConvertFbxMatrixToFloatArray(FbxAMatrix inputMatrix, float inputArray[16]);
	unsigned int FindJointIndexByName(const char * jointName);
	bool ProcessTextMaps(FbxProperty tProp, char** ppText, const int Len);
private:
	FbxMesh* ToTriangleMesh(FbxMesh* inputMesh);
	bool Uint();
	bool RecursiveJointHierarchyTraversal(int currIndex, int inNodeParentIndex, FbxNode * inNode, const char* pParentName = nullptr);

public:
	std::vector<MBDJoint> m_vceSceneJoints;
	std::vector<MBDMeshBag> m_vceMeshDatas;
	std::vector<MBDMaterial> m_vecMaterial;
	//std::vector<MBDMatrix> m_vecMatrix;

	std::vector<std::vector<double> > m_vecMatrixTime;
	std::vector<std::vector<mat4> > m_vecmatrixMat4;
	std::vector<std::string > m_vecMatrixName;
	std::vector<u32> m_vecMatrixCount;
	std::vector<u32> m_vecMatrixFramenum;

	int m_fps;
	int m_MaxKeyFramecount;
private:
	u32 m_MeshCount;
	FbxScene* m_pScene;
	FbxManager* m_fManager;
	
	bool m_FirstProcess;
};

#endif