#include "ReadFile.h"
#include <fstream>

CReadFile::CReadFile():
m_mesh(nullptr)
{}

CReadFile::~CReadFile()
{
	Release();
}

bool CReadFile::Release()
{
	if (nullptr != m_mesh)
	{
		delete m_mesh;
		m_mesh = nullptr;
	}

	m_vceSceneJoints.clear();
	m_vecMatrix.clear();
	return true;
}

bool CReadFile::ReadFromBDM(const char* filePath, CDatas& node)
{
	std::ifstream infile(filePath, std::ifstream::binary);
	if (!infile.is_open())
	{
		return false;
	}

	MBDHeader& MH = node.m_mh;
	MBDDataheader& _rmdh = node.m_mdh;
	infile.read((char*)&MH, sizeof(MH));
	infile.read((char*)&_rmdh, sizeof(_rmdh));

	if (_rmdh.m_matatrilcount > 0)
	{
		node.m_vecMaterial.resize(_rmdh.m_matatrilcount);
	}

	for (int i = 0; i < _rmdh.m_matatrilcount; i++)
	{
		infile.read((char*)(node.m_vecMaterial[i].m_name), sizeof(char)*_max_name_len);
		infile.read((char*)&(node.m_vecMaterial[i].m_ambient), sizeof(vec3));
		infile.read((char*)&(node.m_vecMaterial[i].m_diffuse), sizeof(vec3));
		infile.read((char*)&(node.m_vecMaterial[i].m_specular), sizeof(vec3));
		infile.read((char*)&(node.m_vecMaterial[i].m_emissive), sizeof(vec3));
		infile.read((char*)&(node.m_vecMaterial[i].m_shininess), sizeof(float));
		infile.read((char*)&(node.m_vecMaterial[i].m_transparency), sizeof(float));
		infile.read((char*)&(node.m_vecMaterial[i].m_textcount), sizeof(u32));

		char pText[_max_file_len];
		for (int j = 0; j < node.m_vecMaterial[i].m_textcount; ++j)
		{
			u32 type = 0;
			infile.read((char*)&type, sizeof(u32));
			node.m_vecMaterial[i].m_vecTextType.push_back((TextTureType)type);

			memset(pText, 0, _max_file_len);
			infile.read((char*)pText, _max_file_len);
			node.m_vecMaterial[i].m_vecTetxure.push_back(std::string(pText));
		}
	}


	if (_rmdh.m_meshcount > 0)
	{
		node.m_vceMeshDatas.resize(_rmdh.m_meshcount);
	}
	for (std::vector<MBDMeshBag>::iterator it = node.m_vceMeshDatas.begin();
		it != node.m_vceMeshDatas.end();
		it++)
	{
		infile.read((char*)(it->m_meshname), sizeof(char)*_max_name_len);
		infile.read((char*)&(it->m_materialid), sizeof(u32));
		infile.read((char*)&(it->m_parentmeshid), sizeof(u32));

		infile.read((char*)&(it->m_translation), sizeof(vec3));
		infile.read((char*)&(it->m_rotation), sizeof(vec3));
		infile.read((char*)&(it->m_scale), sizeof(vec3));

		infile.read((char*)&(it->m_isanimated), sizeof(u32));
		infile.read((char*)&(it->m_meshchildcount), sizeof(u32));
		infile.read((char*)&(it->m_vertecount), sizeof(u32));
		infile.read((char*)&(it->m_skeletonvetexcount), sizeof(u32));
		infile.read((char*)&(it->m_indexcount), sizeof(u32));
		//infile.read((char*)&(it->m_KeyFramecount), sizeof(u32));
		//mbdmesh

		if (0 == it->m_isanimated)
		{
			it->mVertexList.resize(it->m_vertecount);
			infile.read((char*)it->mVertexList.data(), sizeof(MBDVertex)* it->m_vertecount);
			//vertexlist
		}
		else
		{
			it->mSkelVertexList.resize(it->m_skeletonvetexcount);
			infile.read((char*)it->mSkelVertexList.data(), sizeof(MBDSkelVertex)* it->m_skeletonvetexcount);
			//skelvertxlist
		}

		if (it->m_indexcount > 0)
		{
			it->mIndex.resize(it->m_indexcount);
			infile.read((char*)it->mIndex.data(), sizeof(u32)*it->mIndex.size());
		}
	}

	if (_rmdh.m_joints > 0)
	{
		node.m_vceSceneJoints.resize(_rmdh.m_joints);
	}

	for (std::vector<MBDJoint>::iterator itJoint = node.m_vceSceneJoints.begin();
		itJoint != node.m_vceSceneJoints.end();
		itJoint++)
	{
		infile.read((char*)&(itJoint->m_id), sizeof(u32));
		infile.read((char*)&(itJoint->m_parentid), sizeof(int));
		infile.read((char*)itJoint->m_name, sizeof(char)*_max_name_len);
		infile.read((char*)&(itJoint->m_parent_name), sizeof(char)*_max_name_len);
		infile.read((char*)&(itJoint->m_rotation), sizeof(vec3));
		infile.read((char*)&(itJoint->m_translation), sizeof(vec3));
		infile.read((char*)&(itJoint->m_scale), sizeof(vec3));

		infile.read((char*)&(itJoint->m_pose_inverse), sizeof(vec4));
		infile.read((char*)&(itJoint->m_globalpose_inverse), sizeof(vec4));

		infile.read((char*)&(itJoint->ChildMeshCount), sizeof(u32));
		infile.read((char*)&(itJoint->keyTranslateCount), sizeof(u32));
		infile.read((char*)&(itJoint->keyRotationCount), sizeof(u32));
		if (itJoint->ChildMeshCount > 0)
		{
			itJoint->m_child_mesh.resize(itJoint->ChildMeshCount);
			infile.read((char*)itJoint->m_child_mesh.data(), sizeof(u32)* itJoint->ChildMeshCount);
		}


		if (itJoint->keyTranslateCount > 0)
		{
			itJoint->m_vecTranslate.resize(itJoint->keyTranslateCount);
			infile.read((char*)itJoint->m_vecTranslate.data(), sizeof(MBDKeyFrame)* itJoint->keyTranslateCount);
		}
		else
		{
			itJoint->keyTranslateCount = _rmdh.m_keyframes;
			itJoint->m_vecTranslate.resize(itJoint->keyTranslateCount);
		}

		if (itJoint->keyRotationCount > 0)
		{
			itJoint->m_vecRotation.resize(itJoint->keyRotationCount);
			infile.read((char*)itJoint->m_vecRotation.data(), sizeof(MBDKeyFrame)* itJoint->keyRotationCount);
		}
		else
		{
			itJoint->keyRotationCount = _rmdh.m_keyframes;
			itJoint->m_vecRotation.resize(itJoint->keyRotationCount);
		}
	}

	
	u32 *pMatrixCount;
	u32 *pMatrixFramenum;
	if (_rmdh.m_matrixcount > 0)
	{
		//node.m_vecMatrix.resize(_rmdh.m_matrixcount);
		pMatrixCount = new u32[_rmdh.m_matrixcount];
		pMatrixFramenum = new u32[_rmdh.m_matrixcount];
		infile.read((char*)pMatrixCount, sizeof(u32)* _rmdh.m_matrixcount);
		infile.read((char*)pMatrixFramenum, sizeof(u32)* _rmdh.m_matrixcount);
	}

	
	for (int iMatrix = 0; iMatrix < _rmdh.m_matrixcount; ++iMatrix)
	{
		char name[_max_file_len];
		infile.read((char*)name, sizeof(char)*_max_file_len);

		MBDMatrix matrix;
		matrix.m_count = pMatrixCount[iMatrix];
		matrix.m_framenum = pMatrixFramenum[iMatrix];
		memcpy(matrix.m_name, name, strlen(name));

		double dtime = 0.0;
		for (int j = 0; j < matrix.m_framenum; ++j)
		{
			infile.read((char*)&dtime, sizeof(double));
			matrix.m_vecTime.push_back(dtime);
		}

		mat4 m4;
		std::vector<mat4>& _vecMat = matrix.m_vecMat;
		for (int j = 0; j < matrix.m_framenum; ++j)
		{
			infile.read((char*)&m4, sizeof(mat4));
			matrix.m_vecMat.push_back(m4);
		}

		node.m_vecMatrix.push_back(matrix);
	}
	
	if (_rmdh.m_matrixcount > 0)
	{
		delete [] pMatrixCount;
		delete [] pMatrixFramenum;
		pMatrixCount = nullptr;
		pMatrixFramenum = nullptr;
	}
	infile.close();

	return true;
}


bool  CReadFile::ReadFileDatas(const char* FileName)// char* MeshFile, char* AniFile, char* MatrixFile)
{
	Release();
	if (nullptr == FileName)
	{
		return false;
	}
	std::string strFileName;
	std::string strName(FileName);
	int iDot = strName.find_last_of(".");
	int iDir1 = strName.find_last_of("\\");
	int iDir2 = strName.find_last_of("/");

	int iDir = iDir1 > iDir2 ? iDir1 : iDir2;
	if (iDot > iDir)
	{
		strFileName.assign(FileName, iDot);
	}
	else
	{
		strFileName = FileName;
	}
	std::string strMesh(strFileName);
	std::string strAnima(strFileName);
	std::string strMatrix(strFileName);
	strMesh.append(MASH_EXT);
	strAnima.append(ANIMATED_EXT);
	strMatrix.append(Matrix_Ext);

	ReadMesh(strMesh.c_str());
	ReadAnima(strAnima.c_str());
	ReadMatrix(strMatrix.c_str());
	return true;
}

bool CReadFile::ReadMesh(const char* MeshFile)
{
	if (nullptr == MeshFile)
	{
		return false;
	}

	std::ifstream infile(MeshFile, std::ifstream::binary);
	if (!infile.is_open())
	{
		return false;
	}

	m_mesh = new CMesh();
	assert(nullptr != m_mesh && " new CMesh() error.");

	MBDHeader& MH = m_mesh->m_mh;
	MBDDataheader& _rmdh = m_mesh->m_mdh;
	infile.read((char*)&MH, sizeof(MH));
	infile.read((char*)&_rmdh, sizeof(_rmdh));

	if (_rmdh.m_matatrilcount > 0)
	{
		m_mesh->m_vecMaterial.resize(_rmdh.m_matatrilcount);
	}

	for (int i = 0; i < _rmdh.m_matatrilcount; i++)
	{
		infile.read((char*)(m_mesh->m_vecMaterial[i].m_name), sizeof(char)*_max_name_len);
		infile.read((char*)&(m_mesh->m_vecMaterial[i].m_ambient), sizeof(vec3));
		infile.read((char*)&(m_mesh->m_vecMaterial[i].m_diffuse), sizeof(vec3));
		infile.read((char*)&(m_mesh->m_vecMaterial[i].m_specular), sizeof(vec3));
		infile.read((char*)&(m_mesh->m_vecMaterial[i].m_emissive), sizeof(vec3));
		infile.read((char*)&(m_mesh->m_vecMaterial[i].m_shininess), sizeof(float));
		infile.read((char*)&(m_mesh->m_vecMaterial[i].m_transparency), sizeof(float));
		infile.read((char*)&(m_mesh->m_vecMaterial[i].m_textcount), sizeof(u32));

		char pText[_max_file_len];
		for (int j = 0; j < m_mesh->m_vecMaterial[i].m_textcount; ++j)
		{
			u32 type = 0;
			infile.read((char*)&type, sizeof(u32));
			m_mesh->m_vecMaterial[i].m_vecTextType.push_back((TextTureType)type);

			memset(pText, 0, _max_file_len);
			infile.read((char*)pText, _max_file_len);
			m_mesh->m_vecMaterial[i].m_vecTetxure.push_back(std::string(pText));
		}
	}


	if (_rmdh.m_meshcount > 0)
	{
		m_mesh->m_vceMeshDatas.resize(_rmdh.m_meshcount);
	}
	for (std::vector<MBDMeshBag>::iterator it = m_mesh->m_vceMeshDatas.begin();
		it != m_mesh->m_vceMeshDatas.end();
		it++)
	{
		infile.read((char*)(it->m_meshname), sizeof(char)*_max_name_len);
		infile.read((char*)&(it->m_materialid), sizeof(u32));
		infile.read((char*)&(it->m_parentmeshid), sizeof(u32));

		infile.read((char*)&(it->m_translation), sizeof(vec3));
		infile.read((char*)&(it->m_rotation), sizeof(vec3));
		infile.read((char*)&(it->m_scale), sizeof(vec3));

		infile.read((char*)&(it->m_isanimated), sizeof(u32));
		infile.read((char*)&(it->m_meshchildcount), sizeof(u32));
		infile.read((char*)&(it->m_vertecount), sizeof(u32));
		infile.read((char*)&(it->m_skeletonvetexcount), sizeof(u32));
		infile.read((char*)&(it->m_indexcount), sizeof(u32));
		//infile.read((char*)&(it->m_KeyFramecount), sizeof(u32));
		//mbdmesh

		if (0 == it->m_isanimated)
		{
			it->mVertexList.resize(it->m_vertecount);
			infile.read((char*)it->mVertexList.data(), sizeof(MBDVertex)* it->m_vertecount);
		}
		else
		{
			it->mSkelVertexList.resize(it->m_skeletonvetexcount);
			infile.read((char*)it->mSkelVertexList.data(), sizeof(MBDSkelVertex)* it->m_skeletonvetexcount);
		}

		if (it->m_indexcount > 0)
		{
			it->mIndex.resize(it->m_indexcount);
			infile.read((char*)it->mIndex.data(), sizeof(u32)*it->mIndex.size());
		}
	}

	return true;
}

bool CReadFile::ReadAnima(const char* MeshFile)
{
	if (nullptr == MeshFile || nullptr == m_mesh)
	{
		return false;
	}

	std::ifstream infile(MeshFile, std::ifstream::binary);
	if (!infile.is_open())
	{
		return false;
	}

	int keyframes = m_mesh->m_mdh.m_keyframes;
	int joints = m_mesh->m_mdh.m_joints;
	if (joints > 0)
	{
		m_vceSceneJoints.resize(joints);
	}

	for (std::vector<MBDJoint>::iterator itJoint = m_vceSceneJoints.begin();
		itJoint != m_vceSceneJoints.end();
		itJoint++)
	{
		infile.read((char*)&(itJoint->m_id), sizeof(u32));
		infile.read((char*)&(itJoint->m_parentid), sizeof(int));
		infile.read((char*)itJoint->m_name, sizeof(char)*_max_name_len);
		infile.read((char*)&(itJoint->m_parent_name), sizeof(char)*_max_name_len);
		infile.read((char*)&(itJoint->m_rotation), sizeof(vec3));
		infile.read((char*)&(itJoint->m_translation), sizeof(vec3));
		infile.read((char*)&(itJoint->m_scale), sizeof(vec3));

		infile.read((char*)&(itJoint->m_pose_inverse), sizeof(vec4));
		infile.read((char*)&(itJoint->m_globalpose_inverse), sizeof(vec4));

		infile.read((char*)&(itJoint->ChildMeshCount), sizeof(u32));
		infile.read((char*)&(itJoint->keyTranslateCount), sizeof(u32));
		infile.read((char*)&(itJoint->keyRotationCount), sizeof(u32));
		if (itJoint->ChildMeshCount > 0)
		{
			itJoint->m_child_mesh.resize(itJoint->ChildMeshCount);
			infile.read((char*)itJoint->m_child_mesh.data(), sizeof(u32)* itJoint->ChildMeshCount);
		}


		if (itJoint->keyTranslateCount > 0)
		{
			itJoint->m_vecTranslate.resize(itJoint->keyTranslateCount);
			infile.read((char*)itJoint->m_vecTranslate.data(), sizeof(MBDKeyFrame)* itJoint->keyTranslateCount);
		}
		else
		{
			itJoint->keyTranslateCount = keyframes;
			itJoint->m_vecTranslate.resize(itJoint->keyTranslateCount);
		}

		if (itJoint->keyRotationCount > 0)
		{
			itJoint->m_vecRotation.resize(itJoint->keyRotationCount);
			infile.read((char*)itJoint->m_vecRotation.data(), sizeof(MBDKeyFrame)* itJoint->keyRotationCount);
		}
		else
		{
			itJoint->keyRotationCount = keyframes;
			itJoint->m_vecRotation.resize(itJoint->keyRotationCount);
		}
	}

	infile.close();
	return true;
}

bool CReadFile::ReadMatrix(const char* MeshFile)
{
	if (nullptr == MeshFile || nullptr == m_mesh)
	{
		return false;
	}

	std::ifstream infile(MeshFile, std::ifstream::binary);
	if (!infile.is_open())
	{
		return false;
	}

	int matrixcount = m_mesh->m_mdh.m_matrixcount;
	u32 *pMatrixCount;
	u32 *pMatrixFramenum;
	if (matrixcount > 0)
	{
		//node.m_vecMatrix.resize(_rmdh.m_matrixcount);
		pMatrixCount = new u32[matrixcount];
		pMatrixFramenum = new u32[matrixcount];
		infile.read((char*)pMatrixCount, sizeof(u32)* matrixcount);
		infile.read((char*)pMatrixFramenum, sizeof(u32)* matrixcount);
	}


	for (int iMatrix = 0; iMatrix < matrixcount; ++iMatrix)
	{
		char name[_max_file_len];
		infile.read((char*)name, sizeof(char)*_max_file_len);

		MBDMatrix matrix;
		matrix.m_count = pMatrixCount[iMatrix];
		matrix.m_framenum = pMatrixFramenum[iMatrix];
		memcpy(matrix.m_name, name, strlen(name));

		double dtime = 0.0;
		for (int j = 0; j < matrix.m_framenum; ++j)
		{
			infile.read((char*)&dtime, sizeof(double));
			matrix.m_vecTime.push_back(dtime);
		}

		mat4 m4;
		std::vector<mat4>& _vecMat = matrix.m_vecMat;
		for (int j = 0; j < matrix.m_framenum; ++j)
		{
			infile.read((char*)&m4, sizeof(mat4));
			matrix.m_vecMat.push_back(m4);
		}

		m_vecMatrix.push_back(matrix);
	}

	if (matrixcount > 0)
	{
		delete[] pMatrixCount;
		delete[] pMatrixFramenum;
		pMatrixCount = nullptr;
		pMatrixFramenum = nullptr;
	}

	infile.close();
	return true;
}