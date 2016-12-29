#ifndef _GET_VER_TEXTICES_H
#define _GET_VER_TEXTICES_H
#include "BaseConfig.h"
//#include "GLInclude.h"
//#include <glm\gtx\rotate_vector.hpp>
//#include <glm\gtc\matrix_transform.hpp>
//#include <glm\gtx\transform.hpp>
//#include <glm\gtx\quaternion.hpp>
class CGetVerTextices
{
public:
	CGetVerTextices();
	~CGetVerTextices();

protected:
	bool GetGeometric(FbxNode* iNode);
	bool processIndices(FbxMesh* inputmesh, MBDMeshBag& mMeshDatas);
	bool ProcessVertices(FbxMesh* inputMesh, MBDMeshBag& mMeshDatas);
	bool ProcessNormal(FbxMesh* inputMesh, MBDMeshBag& mMeshDatas);
	bool ProcessUVs(FbxMesh* inputMesh, MBDMeshBag& mMeshDatas);
	

	bool ProcessVerticesAll(FbxMesh* inputMesh, MBDMeshBag& mMeshDatas);
	bool ProcessNormalsAll(FbxMesh* inputMesh, MBDMeshBag& mMeshDatas);
	bool ProcessUVAll(FbxMesh* inputMesh, MBDMeshBag& mMeshDatas);
private:
	void ReadNormal(FbxMesh* pMesh, int ctrlPointIndex, int vertexCounter, FbxVector4* pNormal);

	
private:
	void UpdateNormal(FbxVector4 v4Put, vec3& v3Get);
	void UpdateUV(FbxVector2 v2Put, vec2& v2Get);

	template<typename VertexType>
	void PushVertices(FbxVector4 v4, std::vector<VertexType>& vecSkelOrVer)
	{
		VertexType ver;
		ver._position[0] = v4.mData[0] + m_GeometricT[0];
		ver._position[1] = v4.mData[1] + m_GeometricT[1];
		ver._position[2] = v4.mData[2] + m_GeometricT[2];

		//ver._position[0] = v4.mData[0];
		//ver._position[1] = v4.mData[1];
		//ver._position[2] = v4.mData[2];
		vecSkelOrVer.push_back(ver);
	}


private:

	FbxVector4 m_GeometricT;
	FbxVector4 m_GeometricR;
	FbxVector4 m_GeometricS;

	FbxDouble4 m_d4r;
	FbxDouble4 m_d4t;
	FbxDouble4 m_d4s;
};







//
//template <>
//void CGetVerTextices::PushVertices<MBDSkelVertex>(FbxVector4* v4, std::vector<MBDSkelVertex>& vecSkelOrVer)
//{
//ver._position[0] = v4.mData[0];
//ver._position[1] = v4.mData[1];
//ver._position[2] = v4.mData[2];
//vecSkelOrVer.push_back(ver);
//}



#endif

