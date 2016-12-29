
#ifndef _STRUCT_FBXNODE_H
#define _STRUCT_FBXNODE_H
#include "BaseConfig.h"
#include "TransformModelConfig.h"
using namespace ar_engine;

class CDatas
{
public:
	std::vector<MBDJoint> m_vceSceneJoints;
	std::vector<MBDMeshBag> m_vceMeshDatas;
	std::vector<MBDMaterial> m_vecMaterial;
	std::vector<MBDMatrix> m_vecMatrix;//补间动画

	MBDHeader m_mh;
	MBDDataheader m_mdh;
};

//////////////////////////


//template<bool bIsAnim>
class CMesh 
{
public:
	//std::vector<u32> mIndex;
	std::vector<MBDMeshBag> m_vceMeshDatas;
	std::vector<MBDMaterial> m_vecMaterial;
	//std::vector<MBDSkelVertex> mSkelVertexList;
	//std::vector<MBDVertex> mVertexList;

	MBDHeader m_mh;
	MBDDataheader m_mdh;
};


//class CAnim : public CMesh
//{
//public:
//	std::vector<MBDJoint> m_vceSceneJoints;
//};
//
//class CMad : public CMesh
//{
//public:
//	std::vector<MBDMatrix> m_vecMatrix;//补间动画
//};

/////////////////////
//template<>
//class CMesh<false>
//{
//public:
//	std::vector<MBDVertex> mVertexList;
//};
//
//template<>
//class CMesh<true>
//{
//public:
//	std::vector<MBDSkelVertex> mVertexList;// mSkelVertexList;
//};
#endif
