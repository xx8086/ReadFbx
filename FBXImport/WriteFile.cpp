#include "WriteFile.h"
#include <fstream>

CWriteFile::CWriteFile(FbxScene* secen, FbxManager* _fManager, int fps):
m_node(secen, _fManager, fps)
{}


bool CWriteFile::DoFile(FbxNode * pRoot)
{
	m_node.ProcessJointHierarchy(pRoot);
	m_node.ProcessNodeAttribute(pRoot);
	m_node.ParseMatrix();
	return true;
}

bool CWriteFile::WriteMesh()
{
	MBDHeader MH;
	MBDDataheader _mdh;
	GetHeader(MH, _mdh);

	std::string strMeshFile(m_strFileName);
	strMeshFile.append(MASH_EXT);
	std::ofstream outfile(strMeshFile.c_str(), std::ofstream::binary);
	outfile.write((const char*)&MH, sizeof(MH));
	outfile.write((const char*)&_mdh, sizeof(_mdh));

	for (std::vector<MBDMaterial>::iterator itMaterial = m_node.m_vecMaterial.begin();
		itMaterial != m_node.m_vecMaterial.end();
		++itMaterial)
	{
		outfile.write((const char*)(itMaterial->m_name), sizeof(char)*_max_name_len);
		outfile.write((const char*)&(itMaterial->m_ambient), sizeof(vec3));
		outfile.write((const char*)&(itMaterial->m_diffuse), sizeof(vec3));
		outfile.write((const char*)&(itMaterial->m_specular), sizeof(vec3));
		outfile.write((const char*)&(itMaterial->m_emissive), sizeof(vec3));
		outfile.write((const char*)&(itMaterial->m_shininess), sizeof(float));
		outfile.write((const char*)&(itMaterial->m_transparency), sizeof(float));
		outfile.write((const char*)&(itMaterial->m_textcount), sizeof(u32));

		char pText[_max_file_len];
		for (int i = 0; i < itMaterial->m_textcount; ++i)
		{
			outfile.write((const char*)&(itMaterial->m_vecTextType.at(i)), sizeof(u32));
			memset(pText, 0, _max_file_len);
			strcpy_s((char*)pText, _max_file_len - 1, itMaterial->m_vecTetxure.at(0).c_str());
			outfile.write((const char*)pText, sizeof(char)*_max_file_len);
		}
	}

	for (std::vector<MBDMeshBag>::iterator it = m_node.m_vceMeshDatas.begin();
		it != m_node.m_vceMeshDatas.end();
		it++)
	{
		outfile.write((const char*)(it->m_meshname), sizeof(char)*_max_name_len);
		outfile.write((const char*)&(it->m_materialid), sizeof(u32));
		outfile.write((const char*)&(it->m_parentmeshid), sizeof(u32));

		outfile.write((const char*)&(it->m_translation), sizeof(vec3));
		outfile.write((const char*)&(it->m_rotation), sizeof(vec3));
		outfile.write((const char*)&(it->m_scale), sizeof(vec3));

		outfile.write((const char*)&(it->m_isanimated), sizeof(u32));
		outfile.write((const char*)&(it->m_meshchildcount), sizeof(u32));
		outfile.write((const char*)&(it->m_vertecount), sizeof(u32));
		outfile.write((const char*)&(it->m_skeletonvetexcount), sizeof(u32));
		outfile.write((const char*)&(it->m_indexcount), sizeof(u32));

		if (0 == it->m_isanimated)
		{
			outfile.write((const char*)it->mVertexList.data(), sizeof(MBDVertex)* it->m_vertecount);
		}
		else
		{
			outfile.write((const char*)it->mSkelVertexList.data(), sizeof(MBDSkelVertex)* it->m_skeletonvetexcount);
		}

		if (it->m_indexcount > 0)
		{
			outfile.write((const char*)it->mIndex.data(), sizeof(u32)*it->mIndex.size());
		}
	}

	outfile.close();
	return true;
}

bool CWriteFile::WriteAnim()
{
	//std::vector<MBDJoint> m_vceSceneJoints;
	std::string strMeshFile(m_strFileName);
	strMeshFile.append(ANIMATED_EXT);
	std::ofstream outfile(strMeshFile.c_str(), std::ofstream::binary);


	for (std::vector<MBDJoint>::iterator itJoint = m_node.m_vceSceneJoints.begin();
		itJoint != m_node.m_vceSceneJoints.end();
		itJoint++)
	{
		outfile.write((const char*)&(itJoint->m_id), sizeof(u32));
		outfile.write((const char*)&(itJoint->m_parentid), sizeof(int));
		outfile.write((const char*)itJoint->m_name, sizeof(char)*_max_name_len);
		outfile.write((const char*)&(itJoint->m_parent_name), sizeof(char)*_max_name_len);
		outfile.write((const char*)&(itJoint->m_rotation), sizeof(vec3));
		outfile.write((const char*)&(itJoint->m_translation), sizeof(vec3));
		outfile.write((const char*)&(itJoint->m_scale), sizeof(vec3));

		outfile.write((const char*)&(itJoint->m_pose_inverse), sizeof(vec4));
		outfile.write((const char*)&(itJoint->m_globalpose_inverse), sizeof(vec4));

		outfile.write((const char*)&(itJoint->ChildMeshCount), sizeof(u32));
		outfile.write((const char*)&(itJoint->keyTranslateCount), sizeof(u32));
		outfile.write((const char*)&(itJoint->keyRotationCount), sizeof(u32));

		if (itJoint->ChildMeshCount > 0)
		{
			outfile.write((const char*)itJoint->m_child_mesh.data(), sizeof(u32)* itJoint->ChildMeshCount);
		}

		if (itJoint->keyTranslateCount > 0)
		{
			outfile.write((const char*)itJoint->m_vecTranslate.data(), sizeof(MBDKeyFrame)* itJoint->keyTranslateCount);
		}
		if (itJoint->keyRotationCount > 0)
		{
			outfile.write((const char*)itJoint->m_vecRotation.data(), sizeof(MBDKeyFrame)* itJoint->keyRotationCount);
		}
	}

	outfile.close();
	return true;
}

bool CWriteFile::WriteMatrix2()
{
	std::string strMeshFile(m_strFileName);
	strMeshFile.append(".txt");
	std::ofstream outfile(strMeshFile.c_str(), std::ofstream::trunc | std::ofstream::in | std::ofstream::out);

	int iMatrixCount = m_node.m_vecMatrixName.size();
	outfile << iMatrixCount << std::endl;
	char pName[_max_file_len];
	for (int iMatrix = 0; iMatrix < iMatrixCount; ++iMatrix)
	{
		outfile << m_node.m_vecMatrixName.at(iMatrix).c_str() << std::endl;
		int iSize = m_node.m_vecMatrixFramenum[iMatrix];
		outfile << iSize << std::endl;
		for (int j = 0; j < iSize; ++j)
		{
			outfile << m_node.m_vecMatrixTime[iMatrix][j] << " ";
		}
		outfile << std::endl;
		for (int j = 0; j < iSize; ++j)
		{
			mat4 m4 = m_node.m_vecmatrixMat4[iMatrix][j];
			vec4 v4;
			for (int z = 0; z < 4; z++)
			{
				for (int w = 0; w < 4; w++)
				{
					outfile << m4[z][w] << " ";
				}
			}
		}
		outfile << std::endl;
	}


	outfile.close();
	return true;
}
bool CWriteFile::WriteMatrix()
{
	std::string strMeshFile(m_strFileName);
	strMeshFile.append(Matrix_Ext);
	std::ofstream outfile(strMeshFile.c_str(), std::ofstream::binary);

	int iMatrixCount = m_node.m_vecMatrixName.size();
	if (iMatrixCount > 0)
	{
		outfile.write((const char*)m_node.m_vecMatrixCount.data(), sizeof(u32)* m_node.m_vecMatrixCount.size());
		outfile.write((const char*)m_node.m_vecMatrixFramenum.data(), sizeof(u32)* m_node.m_vecMatrixFramenum.size());
	}

	char pName[_max_file_len];
	for (int iMatrix = 0; iMatrix < iMatrixCount; ++iMatrix)
	{
		memset(pName, 0, _max_file_len);
		strcpy_s((char*)pName, _max_file_len - 1, m_node.m_vecMatrixName.at(iMatrix).c_str());
		outfile.write((const char*)pName, sizeof(char)*_max_file_len);

		int iSize = m_node.m_vecMatrixFramenum[iMatrix];
		for (int j = 0; j < iSize; ++j)
		{
			outfile.write((const char*)&(m_node.m_vecMatrixTime[iMatrix][j]), sizeof(double));
		}

		for (int j = 0; j < iSize; ++j)
		{
			outfile.write((const char*)&(m_node.m_vecmatrixMat4[iMatrix][j]), sizeof(mat4));
		}
	}


	outfile.close();
	return true;
}

bool CWriteFile::GetFileName(const char* fileName)
{
	if (nullptr == fileName)
	{
		return false;
	}

	std::string strName(fileName);
	int iDot = strName.find_last_of(".");
	int iDir1 = strName.find_last_of("\\");
	int iDir2 = strName.find_last_of("/");

	int iDir = iDir1 > iDir2 ? iDir1 : iDir2;
	if (iDot > iDir)
	{
		m_strFileName.assign(fileName, iDot);
	}
	else
	{
		m_strFileName = fileName;
	}


	WriteMesh();
	WriteAnim();
	WriteMatrix();
	//WriteMatrix2();

	return true;
}

bool CWriteFile::GetHeader(MBDHeader& MH, MBDDataheader& _mdh)
{
	//_mdh.m_keycounts;
	_mdh.m_keyframes = m_node.m_MaxKeyFramecount;
	_mdh.m_meshcount = m_node.m_vceMeshDatas.size();
	_mdh.m_matatrilcount = m_node.m_vecMaterial.size();
	_mdh.m_joints = m_node.m_vceSceneJoints.size();
	_mdh.m_matrixcount = m_node.m_vecMatrixName.size();

	_mdh.m_fps = m_node.m_fps;
	_mdh.m_cur_time = 0;

	for (int i = 0; i < _mdh.m_meshcount; ++i)
	{
		_mdh.m_vertices += m_node.m_vceMeshDatas[i].m_vertecount;
		_mdh.m_skeletonVertices += m_node.m_vceMeshDatas[i].m_skeletonvetexcount;

		_mdh.m_buffersize += m_node.m_vceMeshDatas[i].GetSizeInImport();
	}

	for (int i = 0; i < _mdh.m_matatrilcount; ++i)
	{
		_mdh.m_buffersize += m_node.m_vecMaterial[i].GetSizeInImport();
	}

	for (int i = 0; i < _mdh.m_joints; ++i)
	{
		_mdh.m_buffersize += m_node.m_vceSceneJoints[i].GetSizeInImport();
	}

	_mdh.m_buffersize += _mdh.m_matrixcount * (sizeof(double)+sizeof(mat4)+_max_name_len);
	_mdh.m_datasize = _mdh.m_buffersize + sizeof(MBDHeader)+sizeof(MBDDataheader);
	return true;
}

bool CWriteFile::WriteToBinary()
{
	std::string strFileName(m_strFileName);
	strFileName.append(".mole");
	MBDHeader MH ;
	MBDDataheader _mdh;
	GetHeader(MH, _mdh);
	std::ofstream outfile(strFileName, std::ofstream::binary);
	outfile.write((const char*)&MH, sizeof(MH));
	outfile.write((const char*)&_mdh, sizeof(_mdh));

	for (std::vector<MBDMaterial>::iterator itMaterial = m_node.m_vecMaterial.begin();
		itMaterial != m_node.m_vecMaterial.end();
		++itMaterial)
	{
		outfile.write((const char*)(itMaterial->m_name), sizeof(char)*_max_name_len);
		outfile.write((const char*)&(itMaterial->m_ambient), sizeof(vec3));
		outfile.write((const char*)&(itMaterial->m_diffuse), sizeof(vec3));
		outfile.write((const char*)&(itMaterial->m_specular), sizeof(vec3));
		outfile.write((const char*)&(itMaterial->m_emissive), sizeof(vec3));
		outfile.write((const char*)&(itMaterial->m_shininess), sizeof(float));
		outfile.write((const char*)&(itMaterial->m_transparency), sizeof(float));
		outfile.write((const char*)&(itMaterial->m_textcount), sizeof(u32));

		char pText[_max_file_len];
		for (int i = 0; i < itMaterial->m_textcount; ++i)
		{
			outfile.write((const char*)&(itMaterial->m_vecTextType.at(i)), sizeof(u32));
			memset(pText, 0, _max_file_len);
			strcpy_s((char*)pText, _max_file_len - 1, itMaterial->m_vecTetxure.at(0).c_str());
			outfile.write((const char*)pText, sizeof(char)*_max_file_len);
		}
	}


	for (std::vector<MBDMeshBag>::iterator it = m_node.m_vceMeshDatas.begin();
		it != m_node.m_vceMeshDatas.end();
		it++)
	{
		outfile.write((const char*)(it->m_meshname), sizeof(char)*_max_name_len);
		outfile.write((const char*)&(it->m_materialid), sizeof(u32));
		outfile.write((const char*)&(it->m_parentmeshid), sizeof(u32));

		outfile.write((const char*)&(it->m_translation), sizeof(vec3));
		outfile.write((const char*)&(it->m_rotation), sizeof(vec3));
		outfile.write((const char*)&(it->m_scale), sizeof(vec3));

		outfile.write((const char*)&(it->m_isanimated), sizeof(u32));
		outfile.write((const char*)&(it->m_meshchildcount), sizeof(u32));
		outfile.write((const char*)&(it->m_vertecount), sizeof(u32));
		outfile.write((const char*)&(it->m_skeletonvetexcount), sizeof(u32));
		outfile.write((const char*)&(it->m_indexcount), sizeof(u32));	

		if (0 == it->m_isanimated)
		{
			outfile.write((const char*)it->mVertexList.data(), sizeof(MBDVertex)* it->m_vertecount);
		}
		else
		{
			outfile.write((const char*)it->mSkelVertexList.data(), sizeof(MBDSkelVertex)* it->m_skeletonvetexcount);
		}

		if (it->m_indexcount > 0)
		{
			outfile.write((const char*)it->mIndex.data(), sizeof(u32)*it->mIndex.size());
		}
	}


	for (std::vector<MBDJoint>::iterator itJoint = m_node.m_vceSceneJoints.begin();
		itJoint != m_node.m_vceSceneJoints.end();
		itJoint++)
	{
		outfile.write((const char*)&(itJoint->m_id), sizeof(u32));
		outfile.write((const char*)&(itJoint->m_parentid), sizeof(int));
		outfile.write((const char*)itJoint->m_name, sizeof(char)*_max_name_len);
		outfile.write((const char*)&(itJoint->m_parent_name), sizeof(char)*_max_name_len);
		outfile.write((const char*)&(itJoint->m_rotation), sizeof(vec3));
		outfile.write((const char*)&(itJoint->m_translation), sizeof(vec3));
		outfile.write((const char*)&(itJoint->m_scale), sizeof(vec3));

		outfile.write((const char*)&(itJoint->m_pose_inverse), sizeof(vec4));
		outfile.write((const char*)&(itJoint->m_globalpose_inverse), sizeof(vec4));
		
		outfile.write((const char*)&(itJoint->ChildMeshCount), sizeof(u32));
		outfile.write((const char*)&(itJoint->keyTranslateCount), sizeof(u32));
		outfile.write((const char*)&(itJoint->keyRotationCount), sizeof(u32));

		if (itJoint->ChildMeshCount > 0)
		{
			outfile.write((const char*)itJoint->m_child_mesh.data(), sizeof(u32)* itJoint->ChildMeshCount);
		}

		if (itJoint->keyTranslateCount > 0)
		{
			outfile.write((const char*)itJoint->m_vecTranslate.data(), sizeof(MBDKeyFrame)* itJoint->keyTranslateCount);
		}
		if (itJoint->keyRotationCount > 0)
		{
			outfile.write((const char*)itJoint->m_vecRotation.data(), sizeof(MBDKeyFrame)* itJoint->keyRotationCount);
		}


	}


	if (_mdh.m_matrixcount > 0)
	{
		outfile.write((const char*)m_node.m_vecMatrixCount.data(), sizeof(u32)* m_node.m_vecMatrixCount.size());
		outfile.write((const char*)m_node.m_vecMatrixFramenum.data(), sizeof(u32)* m_node.m_vecMatrixFramenum.size());
	}

	char pName[_max_file_len];
	for (int iMatrix = 0; iMatrix < _mdh.m_matrixcount; ++iMatrix)
	{
		memset(pName, 0, _max_file_len);
		strcpy_s((char*)pName, _max_file_len - 1, m_node.m_vecMatrixName.at(iMatrix).c_str());
		outfile.write((const char*)pName, sizeof(char)*_max_file_len);

		int iSize = m_node.m_vecMatrixFramenum[iMatrix];
		for (int j = 0; j < iSize; ++j)
		{
			outfile.write((const char*)&(m_node.m_vecMatrixTime[iMatrix][j]), sizeof(double));
		}

		for (int j = 0; j < iSize; ++j)
		{
			outfile.write((const char*)&(m_node.m_vecmatrixMat4[iMatrix][j]), sizeof(mat4));
		}
	}

	outfile.close();

	return true;
}
