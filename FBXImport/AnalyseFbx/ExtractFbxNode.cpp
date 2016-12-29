#include "ExtractFbxNode.h"
#include "..\..\PublicCommon\ToolsConfig.h"
#include <fstream>
#include <set>

#define Use_Index 1


struct sBlendData
{
	int jointID;
	int controlPointIndex;
	float blendingWeight;
};


CRootNode::CRootNode(FbxScene* secen, FbxManager* _fManager, int fps) :
m_pScene(secen),
m_fManager(_fManager),
m_fps(fps),
m_FirstProcess(true)
{
	
	Uint();
}

CRootNode::~CRootNode()
{
	m_pScene = nullptr;
}

bool CRootNode::Uint()
{
	m_MeshCount = 0;
	m_vceMeshDatas.clear();

	return true;
}

bool CRootNode::ProcessJointHierarchy(FbxNode * inputRoot)
{
	JUDEG_RETURN((nullptr == inputRoot), false)

	for (int childIndex = 0; childIndex < inputRoot->GetChildCount(); ++childIndex) 
	{
		FbxNode* currNode = inputRoot->GetChild(childIndex);
		RecursiveJointHierarchyTraversal(0, -1, currNode);
	}

	return true;
}

bool CRootNode::RecursiveJointHierarchyTraversal(int currIndex, int inNodeParentIndex, FbxNode * inNode, const char* pParentName)
{
	JUDEG_RETURN((nullptr == inNode), false)

	//printf("%d_%s\n", inNode->GetNodeAttribute()->GetAttributeType(), inNode->GetName());
	if (inNode->GetNodeAttribute() && 
		inNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton )
	{
		
		MBDJoint currJoint;
		memset(currJoint.m_name, 0, _max_name_len);
		memset(currJoint.m_parent_name, 0, _max_name_len);
		currJoint.m_parentid = inNodeParentIndex;
		currJoint.m_id = currIndex;

		memcpy_s(currJoint.m_name, _max_name_len, inNode->GetName(), _max_name_len - 1);
		if (nullptr != pParentName)
		{
			memcpy_s(currJoint.m_parent_name, _max_name_len, pParentName, _max_name_len - 1);
		}

		if (-1 == inNodeParentIndex)
		{//根节点用global
			FbxAMatrix bindposeTransform = inNode->EvaluateGlobalTransform();
			for (unsigned int x = 0; x < 3; x++)
			{
				currJoint.m_translation[x] = bindposeTransform.GetT()[x];
				currJoint.m_rotation[x] = bindposeTransform.GetR()[x];
				currJoint.m_scale[x] = bindposeTransform.GetS()[x];
			}
		}
		else
		{
			FbxVector4& lr = inNode->EvaluateLocalRotation();
			FbxVector4& ls = inNode->EvaluateLocalScaling();
			FbxVector4& lt = inNode->EvaluateLocalTranslation();

			for (unsigned int x = 0; x < 3; x++)
			{
				currJoint.m_translation[x] = lt[x];
				currJoint.m_rotation[x] = lr[x];
				currJoint.m_scale[x] = ls[x];
			}
		}
		m_vceSceneJoints.push_back(currJoint);
	}

	for (int i = 0; i < inNode->GetChildCount(); i++) 
	{
		RecursiveJointHierarchyTraversal(m_vceSceneJoints.size(), currIndex, inNode->GetChild(i), inNode->GetName());
	}

	return true;
}

FbxMesh* CRootNode::ToTriangleMesh(FbxMesh* inputMesh)
{
	if (!inputMesh->IsTriangleMesh())
	{
		FbxGeometryConverter converter(m_fManager);
		inputMesh = (FbxMesh*)converter.Triangulate(inputMesh, true);
	}

	return inputMesh;
}


bool CRootNode::CheckMaterialName(const char* materialName, MBDMeshBag& mMeshDatas)
{
	JUDEG_RETURN((nullptr == materialName), false)
	bool isMatching = false;
	for (int nameIndex = 0; nameIndex < m_vecMaterial.size(); nameIndex++)
	{
		if (std::strcmp(m_vecMaterial[nameIndex].m_name, materialName) == 0)
		{
			mMeshDatas.m_materialid = nameIndex;
			isMatching = true;
			return isMatching; /*A duplicate material was found.*/
		}
	}

	MBDMaterial _Material;
	memset(&_Material, 0, sizeof(_Material));
	if (m_FirstProcess == true)
	{
		strcpy_s(_Material.m_name, _max_name_len - 1, materialName);

		m_vecMaterial.push_back(_Material);
		mMeshDatas.m_materialid = 0;
		m_FirstProcess = false;

		return isMatching; /*No duplicate material was found.*/
	}

	strcpy_s(_Material.m_name, _max_name_len - 1, materialName);
	m_vecMaterial.push_back(_Material);
	mMeshDatas.m_materialid = m_vecMaterial.size() - 1;
	isMatching = false;
	return isMatching; /*No duplicate material was found.*/
}


bool CRootNode::ProcessMaterials(FbxMesh* inputMesh, MBDMeshBag& mMeshDatas)
{
	JUDEG_RETURN((nullptr == inputMesh), false)
	int materialCount = 0;
	if (inputMesh)
	{
		materialCount = inputMesh->GetNode()->GetMaterialCount();
		if (materialCount > 0)
		{
			for (int materialIndex = 0; materialIndex < materialCount; materialIndex++)
			{
				FbxSurfaceMaterial* material = inputMesh->GetNode()->GetMaterial(materialIndex);
				FbxDouble3 ambientColor, diffuseColor, specularColor, lEmissiveColor;

				/*The material for the current node is a surface phong.*/
				if (material->GetClassId().Is(FbxSurfacePhong::ClassId))
				{
					const char* materialName = material->GetName();
					bool materialFlag = CheckMaterialName(materialName, mMeshDatas);
					if (materialFlag == false)
					{
						/*Getting the material attributes that are typical for a surface phong.*/
						ambientColor = ((FbxSurfacePhong *)material)->Ambient;
						diffuseColor = ((FbxSurfacePhong *)material)->Diffuse;
						specularColor = ((FbxSurfacePhong *)material)->Specular;
						lEmissiveColor = ((FbxSurfacePhong *)material)->Emissive;

						m_vecMaterial[mMeshDatas.m_materialid].m_ambient[0] = ambientColor.mData[0];
						m_vecMaterial[mMeshDatas.m_materialid].m_ambient[1] = ambientColor.mData[1];
						m_vecMaterial[mMeshDatas.m_materialid].m_ambient[2] = ambientColor.mData[2];

						m_vecMaterial[mMeshDatas.m_materialid].m_diffuse[0] = diffuseColor.mData[0];
						m_vecMaterial[mMeshDatas.m_materialid].m_diffuse[1] = diffuseColor.mData[1];
						m_vecMaterial[mMeshDatas.m_materialid].m_diffuse[2] = diffuseColor.mData[2];

						m_vecMaterial[mMeshDatas.m_materialid].m_specular[0] = specularColor.mData[0];
						m_vecMaterial[mMeshDatas.m_materialid].m_specular[1] = specularColor.mData[1];
						m_vecMaterial[mMeshDatas.m_materialid].m_specular[2] = specularColor.mData[2];

						m_vecMaterial[mMeshDatas.m_materialid].m_emissive[0] = lEmissiveColor.mData[0];
						m_vecMaterial[mMeshDatas.m_materialid].m_emissive[1] = lEmissiveColor.mData[1];
						m_vecMaterial[mMeshDatas.m_materialid].m_emissive[2] = lEmissiveColor.mData[2];

						m_vecMaterial[mMeshDatas.m_materialid].m_transparency = (float)((FbxSurfacePhong *)material)->TransparencyFactor;
						m_vecMaterial[mMeshDatas.m_materialid].m_shininess = (float) ((FbxSurfacePhong*)material)->Shininess;
						ProcessTextures(material, mMeshDatas.m_materialid);
					}
				}
				else if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
				{
					const char* materialName = material->GetName();
					bool materialFlag = CheckMaterialName(materialName, mMeshDatas);
					if (materialFlag == false)
					{
						/*Getting the material attributes that are typical for a surface lambert.*/
						ambientColor = ((FbxSurfaceLambert *)material)->Ambient;
						diffuseColor = ((FbxSurfaceLambert *)material)->Diffuse;
						lEmissiveColor = ((FbxSurfaceLambert *)material)->Emissive;

						m_vecMaterial[mMeshDatas.m_materialid].m_ambient[0] = ambientColor.mData[0];
						m_vecMaterial[mMeshDatas.m_materialid].m_ambient[1] = ambientColor.mData[1];
						m_vecMaterial[mMeshDatas.m_materialid].m_ambient[2] = ambientColor.mData[2];

						m_vecMaterial[mMeshDatas.m_materialid].m_diffuse[0] = diffuseColor.mData[0];
						m_vecMaterial[mMeshDatas.m_materialid].m_diffuse[1] = diffuseColor.mData[1];
						m_vecMaterial[mMeshDatas.m_materialid].m_diffuse[2] = diffuseColor.mData[2];

						/*No specular attributes and shininess can't be found with a lambert material,
						so we simply set the values for the attributes to 0.*/
						m_vecMaterial[mMeshDatas.m_materialid].m_specular[0] = 0;
						m_vecMaterial[mMeshDatas.m_materialid].m_specular[1] = 0;
						m_vecMaterial[mMeshDatas.m_materialid].m_specular[2] = 0;
						m_vecMaterial[mMeshDatas.m_materialid].m_transparency = (float)((FbxSurfaceLambert *)material)->TransparencyFactor;
						ProcessTextures(material, mMeshDatas.m_materialid);
					}
				}
			}
		}
	}
	return true;
}

bool CRootNode::ProcessTextMaps(FbxProperty tProp, char** ppText, const int Len)
{
	int textureCount = tProp.GetSrcObjectCount<FbxTexture>();
	if (textureCount <= 0)
		return false;

	for (int textureIndex = 0; textureIndex < textureCount; textureIndex++)
	{
		FbxTexture* texture = tProp.GetSrcObject<FbxTexture>(textureIndex);
		FbxFileTexture* fileTexture = FbxCast<FbxFileTexture>(texture);
		memset(*ppText, 0, Len);
		strcpy_s(*ppText, Len - 1, fileTexture->GetFileName());
	}

	return true;
}


bool CRootNode::ProcessTextures(FbxSurfaceMaterial* pMaterial, int MaterialID)
{
	if (pMaterial)
	{
		//FbxProperty propNormal, propEmissive;
		//propNormal = material->FindProperty(FbxSurfaceMaterial::sNormalMap);
		//propEmissive = material->FindProperty(FbxSurfaceMaterial::sEmissive);
		//char* pNormalText = m_vecMaterial[mMeshDatas.m_materialid].m_texture;
		//char* pEmissiveText = m_vecMaterial[mMeshDatas.m_materialid].m_alphamap;
		//ProcessTextMaps(propNormal, &pNormalText, _max_file_len - 1);
		//ProcessTextMaps(propEmissive, &pEmissiveText, _max_file_len - 1);

		bool lDisplayHeader = true;
		int lTextureIndex;
		FbxProperty lProperty;
		FBXSDK_FOR_EACH_TEXTURE(lTextureIndex)
		{
			const char* pName = FbxLayerElement::sTextureChannelNames[lTextureIndex];
			lProperty = pMaterial->FindProperty(pName);
			const char* cText = FindAndDisplayTextureInfoByProperty(pName, lProperty, lDisplayHeader, MaterialID);
			if (nullptr != cText)
			{
				//char* pText = new char [_max_file_len];
				char pText[_max_file_len];
				memset(pText, 0, _max_file_len);
				strcpy_s((char*)pText, _max_file_len - 1, cText);
				std::string str(pText);
				//m_vecMaterial[mMeshDatas.m_materialid].m_ppTextList[m_vecMaterial[mMeshDatas.m_materialid].m_vecTextType.size()] = pText;
				m_vecMaterial[MaterialID].m_vecTetxure.push_back(str);
				m_vecMaterial[MaterialID].m_vecTextType.push_back(TextTureType(lTextureIndex));

				//m_vecMaterial[mMeshDatas.m_materialid].m_vecTetxure.push_back(Text);
			}
		}

		m_vecMaterial[MaterialID].m_textcount = m_vecMaterial[MaterialID].m_vecTextType.size();
	}

	return true;
}

const char* CRootNode::FindAndDisplayTextureInfoByProperty(const char* pName, FbxProperty pProperty, bool& pDisplayHeader, int pMaterialIndex)
{
	if (pProperty.IsValid())
	{
		int lTextureCount = pProperty.GetSrcObjectCount<FbxTexture>();
		for (int j = 0; j < lTextureCount; ++j)
		{
			//Here we have to check if it's layeredtextures, or just textures:
			FbxLayeredTexture *lLayeredTexture = pProperty.GetSrcObject<FbxLayeredTexture>(j);
			if (lLayeredTexture)
			{
				int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();
				for (int k = 0; k<lNbTextures; ++k)
				{
					FbxTexture* lTexture = lLayeredTexture->GetSrcObject<FbxTexture>(k);
					if (lTexture)
					{

						if (pDisplayHeader)
						{
							pDisplayHeader = false;
						}

						//NOTE the blend mode is ALWAYS on the LayeredTexture and NOT the one on the texture.
						//Why is that?  because one texture can be shared on different layered textures and might
						//have different blend modes.

						FbxLayeredTexture::EBlendMode lBlendMode;
						lLayeredTexture->GetTextureBlendMode(k, lBlendMode);

						FbxFileTexture *lFileTexture = FbxCast<FbxFileTexture>(lTexture);
						//if (lFileTexture)
						//{
						//	printf("\n--------\n%s\n", pName);
						//	printf("1Name:		%s\n", (char *)lTexture->GetName());
						//	printf("1File Name:  %s\n", (char *)lFileTexture->GetFileName());
						//}

						return  (char*)lFileTexture->GetFileName();
					}

				}
			}
			else
			{
				//no layered texture simply get on the property
				FbxTexture* lTexture = pProperty.GetSrcObject<FbxTexture>(j);
				if (lTexture)
				{
					//display connected Material header only at the first time
					if (pDisplayHeader){
						pDisplayHeader = false;
					}

					FbxFileTexture *lFileTexture = FbxCast<FbxFileTexture>(lTexture);
					//if (lFileTexture)
					//{
					//	printf("\n--------\n%s\n", pName);
					//	printf("2Name:		%s\n", (char *)lTexture->GetName());
					//	printf("2File Name:  %s\n", (char *)lFileTexture->GetFileName());
					//}
					return  (char*)lFileTexture->GetFileName();

				}
			}
		}
	}//end if pProperty

	return nullptr;
}

bool CRootNode::ProcessTransformations(FbxMesh* inputMesh, MBDMeshBag& mMeshDatas)
{
	FbxAMatrix transformMatrix = inputMesh->GetNode()->EvaluateGlobalTransform();
	FbxVector4 trans, scale, rotat;

	trans = transformMatrix.GetT();
	scale = transformMatrix.GetS();
	rotat = transformMatrix.GetR();

	mMeshDatas.m_translation[0] = trans.mData[0];
	mMeshDatas.m_translation[1] = trans.mData[1];
	mMeshDatas.m_translation[2] = trans.mData[2];

	mMeshDatas.m_scale[0] = scale.mData[0];
	mMeshDatas.m_scale[1] = scale.mData[1];
	mMeshDatas.m_scale[2] = scale.mData[2];

	mMeshDatas.m_rotation[0] = rotat.mData[0];
	mMeshDatas.m_rotation[1] = rotat.mData[1];
	mMeshDatas.m_rotation[2] = rotat.mData[2];

	//FbxTimeSpan lTs;
	//FbxTime     lStart, lEnd;
	//m_pScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTs);
	//lStart = lTs.GetStart();
	//lEnd = lTs.GetStop();

	//char lTimeString[256];
	//char lTimeEnd[256];
	//lStart.GetTimeString(lTimeString, FbxUShort(256));
	//lEnd.GetTimeString(lTimeEnd, FbxUShort(256));

	return true;
}

//////////////////////////
//----------------------------------------------------------------
bool CRootNode::ParseMatrix()
{
	//ProcessKeyFrame2();
	FbxNode* pNode = m_pScene->GetRootNode();
	const int nStackCount = m_pScene->GetSrcObjectCount<FbxAnimStack>();
	for (int i = 0; i < nStackCount; ++i)
	{
		FbxAnimStack* pStack = m_pScene->GetSrcObject<FbxAnimStack>(i);
		const char* szStackName = pStack->GetName();
		const int nLayerCount = pStack->GetMemberCount<FbxAnimLayer>();
		for (int j = 0; j < nLayerCount; ++j)
		{
			FbxAnimLayer* pLayer = pStack->GetMember<FbxAnimLayer>(j);
			ParseLayer(pNode, pLayer);
		}
	}

	int MatrixCount = m_vecMatrixName.size();
	for (int i = 0; i < MatrixCount; ++i)
	{
		m_vecMatrixFramenum.push_back(m_vecMatrixTime[i].size());
		m_vecMatrixCount.push_back(1);
	}
	return true;
}

//----------------------------------------------------------------
void CRootNode::ParseLayer(FbxNode* pNode, FbxAnimLayer* pLayer)
{
	ParseChannel(pNode, pLayer);
	const int nChildCount = pNode->GetChildCount();
	for (int k = 0; k < nChildCount; ++k)
	{
		ParseLayer(pNode->GetChild(k), pLayer);
	}
}
//----------------------------------------------------------------
void CRootNode::ParseChannel(FbxNode* pNode, FbxAnimLayer* pLayer)
{
	FbxAnimCurve* pCurve = pNode->LclTranslation.GetCurve(pLayer, FBXSDK_CURVENODE_COMPONENT_X);
	if (pCurve == 0)
	{
		return;
	}

	FbxTime kTime;
	char lTimeString[256];
	static int i = 0;
	//printf("%d,\n", i++);
	
	//const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	//const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	//const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);


	const int nKeyCount = pCurve->KeyGetCount();
	std::vector<mat4> vecmat;
	std::vector<double> vecTimel;
	for (int i = 0; i < nKeyCount; ++i)
	{
		kTime = pCurve->KeyGetTime(i);
		//const FbxAMatrix& kMatGlobal = pNode->EvaluateLocalTransform(kTime);
		FbxAMatrix kMatGlobal = pNode->EvaluateGlobalTransform(kTime);

		vecTimel.push_back(kTime.GetSecondDouble()/*.GetFrameCountPrecise()*/);
		vecmat.push_back(mat4(
			kMatGlobal[0][0], kMatGlobal[0][1], kMatGlobal[0][2], kMatGlobal[0][3],
			kMatGlobal[1][0], kMatGlobal[1][1], kMatGlobal[1][2], kMatGlobal[1][3],
			kMatGlobal[2][0], kMatGlobal[2][1], kMatGlobal[2][2], kMatGlobal[2][3],
			kMatGlobal[3][0], kMatGlobal[2][1], kMatGlobal[2][2], kMatGlobal[2][3]));

		//kTime.GetTimeString(lTimeString, FbxUShort(256));
		//printf("%s_%f,", lTimeString, dp);
	}

	m_vecMatrixTime.push_back(vecTimel);
	m_vecmatrixMat4.push_back(vecmat);
	m_vecMatrixName.push_back(std::string(pNode->GetName()));
}

///////////////////////////////////////


unsigned int CRootNode::FindJointIndexByName(const char * jointName)
{
	unsigned int r = _error_value;
	for (unsigned int i = 0; i < m_vceSceneJoints.size(); ++i)
	{
		if (std::strcmp(jointName, m_vceSceneJoints[i].m_name) == 0)
		{
			r =  m_vceSceneJoints[i].m_id;
			break;
		}
	}

	return r;
}

bool CRootNode::ConvertFbxMatrixToFloatArray(FbxAMatrix inputMatrix, float inputArray[16])
{
	//This function assumes row-major matrices.
	/**
	row column		n	r	c	n	 r c	n r c
	0	0	0			4	1	0	8  	 2 0	12  3 0
	1	0	1			5	1	1	9	 2 1	13  3 1
	2	0	2			6	1	2	10	 2 2	14  3 2
	3	0	3			7	1	3	11	 2 3	15 3 3
	**/
	unsigned int localCounter = 0;
	for (unsigned int g = 0; g < 4; ++g)
	{
		for (unsigned int h = 0; h < 4; ++h)
		{
			inputArray[localCounter] = inputMatrix.Get(g, h);
			localCounter++;
		}
	}

	return true;
}


bool CRootNode::ProcessJoints(FbxMesh * inputMesh, MBDMeshBag& mMeshDatas)
{
	unsigned int deformerCount = inputMesh->GetDeformerCount(FbxDeformer::eSkin);
	std::vector<std::vector <sBlendData> > bdList2;
	int prevSizes = 0;
	for (int pie = 0; pie < deformerCount; pie++)
	{
		FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(inputMesh->GetDeformer(pie, FbxDeformer::eSkin));
		if (!currSkin)
		{
			continue;
		}
			
		unsigned int dbSize = 0;
		const unsigned int clusterCount = currSkin->GetClusterCount();
		for (unsigned int clusterCounter = 0; clusterCounter < clusterCount; ++clusterCounter)
		{
			FbxCluster* currCluster = currSkin->GetCluster(clusterCounter);
			const unsigned int controlPointIndicesCount = currCluster->GetControlPointIndicesCount();
			dbSize = controlPointIndicesCount + prevSizes;
			prevSizes += controlPointIndicesCount;
		}

		if (dbSize > 0)
		{
			bdList2.resize(dbSize);
		}
		
		for (unsigned int clusterCounter = 0; clusterCounter < clusterCount; ++clusterCounter)//44
		{
			FbxCluster* currCluster = currSkin->GetCluster(clusterCounter);
			FbxNode* currJoint = currCluster->GetLink();
			unsigned int currJointIndex = FindJointIndexByName(currJoint->GetName());
			assert(_error_value != currJointIndex);
			const unsigned int controlPointIndicesCount = currCluster->GetControlPointIndicesCount();
			for (int i = 0; i < controlPointIndicesCount; i++)
			{
				sBlendData temp;
				temp.jointID = m_vceSceneJoints[currJointIndex].m_id;
				temp.controlPointIndex = currCluster->GetControlPointIndices()[i];
				temp.blendingWeight = currCluster->GetControlPointWeights()[i];
				bdList2[temp.controlPointIndex].push_back(temp);
			}
		}

	
		int SkelCount = mMeshDatas.mSkelVertexList.size();
		assert(SkelCount < bdList2.size());
		for (int indexCounter = 0; indexCounter < SkelCount; indexCounter++)
		{
			int count = bdList2[indexCounter].size();
			count = count < 4 ? count : 4;
			for (int i = 0; i < count; i++)
			{
				mMeshDatas.mSkelVertexList[indexCounter]._bone_indices[i] = bdList2[indexCounter][i].jointID;
				mMeshDatas.mSkelVertexList[indexCounter]._bone_weights[i] = bdList2[indexCounter][i].blendingWeight;
			}
		}


		ProcessInverse(currSkin);
		ProcessKeyFrame(currSkin, inputMesh, mMeshDatas);
	}

	MakeSumWeights(mMeshDatas);
	return true;
}

bool CRootNode::MeshCalculate(MBDMeshBag& mMeshDatas)
{
	mMeshDatas.m_indexcount = mMeshDatas.mIndex.size();
	mMeshDatas.m_skeletonvetexcount = mMeshDatas.mSkelVertexList.size();
	mMeshDatas.m_vertecount = mMeshDatas.mVertexList.size();

	m_MaxKeyFramecount = 0;
	for (std::vector<MBDJoint>::iterator itJoin = m_vceSceneJoints.begin();
		itJoin != m_vceSceneJoints.end();
		++itJoin)
	{
		itJoin->keyTranslateCount = itJoin->m_vecTranslate.size();
		itJoin->keyRotationCount = itJoin->m_vecRotation.size();

		if (itJoin->keyTranslateCount > m_MaxKeyFramecount)
		{
			m_MaxKeyFramecount = itJoin->keyTranslateCount;
		}

		if (itJoin->keyRotationCount > m_MaxKeyFramecount)
		{
			m_MaxKeyFramecount = itJoin->keyRotationCount;
		}
	}

	return true;
}

bool CRootNode::MakeSumWeights(MBDMeshBag& mMeshDatas)
{
	//Make the sum of the weights 1.
	for (int j = 0; j < mMeshDatas.mSkelVertexList.size(); j++)
	{
		float sumW = 0;
		for (int i = 0; i < 4; i++)
		{
			sumW += mMeshDatas.mSkelVertexList[j]._bone_weights[i];
		}
		for (int i = 0; i < 4; i++)
		{
			mMeshDatas.mSkelVertexList[j]._bone_weights[i] = mMeshDatas.mSkelVertexList[j]._bone_weights[i] / sumW;
		}
		float sumPost;
		sumPost = mMeshDatas.mSkelVertexList[j]._bone_weights[0] + mMeshDatas.mSkelVertexList[j]._bone_weights[1]
			+ mMeshDatas.mSkelVertexList[j]._bone_weights[2] + mMeshDatas.mSkelVertexList[j]._bone_weights[3];

		if (sumPost != 1.f) {
			float difference = 1.f - sumPost;
			for (int i = 0; i < 4; i++)
			{
				mMeshDatas.mSkelVertexList[j]._bone_weights[i] += difference;
				if (mMeshDatas.mSkelVertexList[j]._bone_weights[i] > 1.f) {
					difference = mMeshDatas.mSkelVertexList[j]._bone_weights[i] - 1.f;
				}
				else
				{
					break;
				}
			}
		}

	}
	return true;
}

bool CRootNode::ProcessInverse(FbxSkin* currSkin)
{
	if (nullptr == currSkin)
	{
		return false;
	}
	const unsigned int clusterCount = currSkin->GetClusterCount();
	for (unsigned int clusterCounter = 0; clusterCounter < clusterCount; ++clusterCounter)
	{
		FbxCluster* currCluster = currSkin->GetCluster(clusterCounter);
		FbxNode* currJoint = currCluster->GetLink();

		//The pos/rot/scale of the joint at bindtime
		FbxAMatrix bindposeTransform = currJoint->EvaluateGlobalTransform();
		unsigned int currJointIndex = FindJointIndexByName(currJoint->GetName());
		assert(_error_value != currJointIndex);

		FbxAMatrix tempBindMatrix;
		FbxAMatrix tempParentBindMatrix;
		FbxAMatrix tempGlobalBindPoseInverse;
		FbxAMatrix tempBindPoseInverse;
		currCluster->GetTransformMatrix(tempParentBindMatrix);
		currCluster->GetTransformLinkMatrix(tempBindMatrix);
		tempBindPoseInverse = tempBindMatrix.Inverse();
		tempGlobalBindPoseInverse = tempBindPoseInverse * tempParentBindMatrix;

		float invBindMatrix[16];
		float invGBindMatrix[16];

		ConvertFbxMatrixToFloatArray(tempGlobalBindPoseInverse, invGBindMatrix);
		ConvertFbxMatrixToFloatArray(tempParentBindMatrix, invBindMatrix);
		//convertFbxMatrixToFloatArray(tempInvParentBindMatrix, invParentBindMatrix);

		mat4 inverse(invBindMatrix[0], invBindMatrix[1], invBindMatrix[2], invBindMatrix[3],
			invBindMatrix[4], invBindMatrix[5], invBindMatrix[6], invBindMatrix[7],
			invBindMatrix[8], invBindMatrix[9], invBindMatrix[10], invBindMatrix[11],
			invBindMatrix[12], invBindMatrix[12], invBindMatrix[14], invBindMatrix[15]);
		mat4 gbinverse(invGBindMatrix[0], invGBindMatrix[1], invGBindMatrix[2], invGBindMatrix[3],
			invGBindMatrix[4], invGBindMatrix[5], invGBindMatrix[6], invGBindMatrix[7],
			invGBindMatrix[8], invGBindMatrix[9], invGBindMatrix[10], invGBindMatrix[11],
			invGBindMatrix[12], invGBindMatrix[12], invGBindMatrix[14], invGBindMatrix[15]);

		m_vceSceneJoints[currJointIndex].m_pose_inverse = inverse;
		m_vceSceneJoints[currJointIndex].m_globalpose_inverse = gbinverse;
	}
	return true;
}

bool CRootNode::ProcessKeyFrame2()
{
	FbxTimeSpan lTs;
	FbxTime     lStart, lEnd;
	m_pScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTs);
	lStart = lTs.GetStart();
	lEnd = lTs.GetStop();

	float keyTimeEnd = lEnd.GetSecondDouble();

	char lTimeString[256];
	char lTimeEnd[256];
	lStart.GetTimeString(lTimeString, FbxUShort(256));
	lEnd.GetTimeString(lTimeEnd, FbxUShort(256));
	return true;
}

bool CRootNode::ProcessKeyFrame(FbxSkin* currSkin, FbxMesh* inputMesh, MBDMeshBag& mMeshDatas)
{
	if (nullptr == currSkin)
	{
		return false;
	}
	

	FbxTimeSpan lTs;
	FbxTime     lStart, lEnd;
	m_pScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTs);
	lStart = lTs.GetStart();
	lEnd = lTs.GetStop();

	//float keyTimeBegin = lStart.GetSecondDouble();
	//float l  = (keyTimeBegin / 24) * m_fps;
	float keyTimeEnd = lEnd.GetSecondDouble();
	//float e = (keyTimeEnd / 24) * m_fps;


	char lTimeString[256];
	char lTimeEnd[256];
	lStart.GetTimeString(lTimeString, FbxUShort(256));
	lEnd.GetTimeString(lTimeEnd, FbxUShort(256));

	const unsigned int clusterCount = currSkin->GetClusterCount();
	for (unsigned int clusterCounter = 0; clusterCounter < clusterCount; ++clusterCounter)
	{
		FbxCluster* currCluster = currSkin->GetCluster(clusterCounter);
		FbxNode* currJoint = currCluster->GetLink();
		FbxAnimEvaluator* animationEvaluator = currJoint->GetAnimationEvaluator();
		unsigned int currJointIndex = FindJointIndexByName(currJoint->GetName());
		assert(_error_value != currJointIndex);

		//Start processing stacks holding animation layers.
		const unsigned int stackCount = m_pScene->GetSrcObjectCount<FbxAnimStack>();
		for (unsigned int stackCounter = 0; stackCounter < stackCount; ++stackCounter)
		{

			FbxAnimStack* currStack = m_pScene->GetSrcObject<FbxAnimStack>(stackCounter);
			int layerCount = currStack->GetMemberCount<FbxAnimLayer>();
			for (int ior = 0; ior < layerCount; ior++)
			{
				FbxAnimLayer* currLayer = currStack->GetMember<FbxAnimLayer>(ior);
				currLayer->Weight = 0;
				currLayer->Mute = false;
				currLayer->Solo = false;
			}

			std::string strName(currJoint->GetName());
			for (unsigned int layerCounter = 0; layerCounter < layerCount; layerCounter++)
			{
				FbxAnimLayer* currLayer = currStack->GetMember<FbxAnimLayer>(layerCounter);
				currLayer->Weight = 100;

				FbxAnimCurve* cu = nullptr;
				FbxAnimCurve* storeCurve[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

				storeCurve[0] = cu = currJoint->LclRotation.GetCurve(currLayer, FBXSDK_CURVENODE_COMPONENT_X);//RX
				storeCurve[1] = cu = currJoint->LclRotation.GetCurve(currLayer, FBXSDK_CURVENODE_COMPONENT_Y);
				storeCurve[2] = cu = currJoint->LclRotation.GetCurve(currLayer, FBXSDK_CURVENODE_COMPONENT_Z);//LclRotation

				storeCurve[3] = cu = currJoint->LclTranslation.GetCurve(currLayer, FBXSDK_CURVENODE_COMPONENT_X);//TX
				storeCurve[4] = cu = currJoint->LclTranslation.GetCurve(currLayer, FBXSDK_CURVENODE_COMPONENT_Y);
				storeCurve[5] = cu = currJoint->LclTranslation.GetCurve(currLayer, FBXSDK_CURVENODE_COMPONENT_Z);//LclTranslation
						
				unsigned int keyCount = 0;
				int currCurve = 0;
				for (int cur = 0; cur <= 5; cur++)
				{
					if (nullptr != storeCurve[cur] && keyCount < storeCurve[cur]->KeyGetCount())
					{
						keyCount = storeCurve[cur]->KeyGetCount();
						currCurve = cur;
					}
				}
				

				FbxDouble3 _d3r = currJoint->LclRotation.Get();
				FbxDouble3 _d3t = currJoint->LclTranslation.Get();
				FbxDouble3 _d3s = currJoint->LclScaling.Get();
				vec3 pz(currJoint->PreRotation.Get()[0], currJoint->PreRotation.Get()[1], currJoint->PreRotation.Get()[2]);
				

				//FbxVector4 _GeometricT = currJoint->GetGeometricTranslation(FbxNode::eDestinationPivot);
				//FbxVector4 _GeometricR = currJoint->GetGeometricRotation(FbxNode::eDestinationPivot);

				for (unsigned int keyCounter = 0; keyCounter < keyCount; keyCounter++)
				{
					MBDKeyFrame tempKey;

					FbxAnimCurveKey currKey = storeCurve[currCurve]->KeyGet(keyCounter);
					float keyTime = currKey.GetTime().GetSecondDouble();
					if (keyTimeEnd < keyTime)
					{
						break;
					}
					
					tempKey.m_keytime = (keyTime / 24) * m_fps;
					if (nullptr != storeCurve[0])
					{
						FbxVector4 tempRotation(1.0, 1.0, 1.0, 1.0);//GetNodeLocalRotation
						//FbxAMatrix fm = animationEvaluator->GetNodeGlobalTransform(currJoint, currKey.GetTime());
						tempRotation = animationEvaluator->GetNodeLocalRotation(currJoint, currKey.GetTime());
						//vec3 v3(tempRotation[0], tempRotation[1], tempRotation[2]);
						//vec3 r3(_d3r[0], _d3r[1], _d3r[2]);
						//v3 = glm::rotate(v3, pz[0], r3);

						for (unsigned int k = 0; k < 3; k++)
						{
							tempKey.m_keypos[k] = tempRotation[k] - _d3r[k];
						}

						if (-90.0 == pz[0])
						{
							float v2 = tempKey.m_keypos[2];
							tempKey.m_keypos[2] = tempKey.m_keypos[1];
							tempKey.m_keypos[1] = v2;

						}
						m_vceSceneJoints[currJointIndex].m_vecRotation.push_back(tempKey);
						
					}
					if (nullptr != storeCurve[3])
					{
						FbxVector4 tempTranslation(1.0, 1.0, 1.0, 1.0);
						//FbxAMatrix fm = animationEvaluator->GetNodeGlobalTransform(currJoint, currKey.GetTime());
						tempTranslation = animationEvaluator->GetNodeLocalTranslation(currJoint, currKey.GetTime());

						for (unsigned int k = 0; k < 3; k++)
						{
							tempKey.m_keypos[k] = tempTranslation[k] - _d3t[k];
						}
						m_vceSceneJoints[currJointIndex].m_vecTranslate.push_back(tempKey);
					}
				}


			
				////////////////////////
				currLayer->Weight = 0;
			}
		}
	}
	
	return true;
}


bool CRootNode::ProcessMesh(FbxMesh* inputMesh, MBDMeshBag& mMeshDatas)
{
	//mMeshDatas.m_isanimated = bool(inputMesh->GetDeformerCount(FbxDeformer::eSkin) > 0);
	//printf("%s_%d\n", inputMesh->GetName(), mMeshDatas.m_isanimated);
	//return true;

	inputMesh = ToTriangleMesh(inputMesh);

#if Use_Index
	CGetVerTextices::processIndices(inputMesh, mMeshDatas);
	CGetVerTextices::ProcessVertices(inputMesh, mMeshDatas);
	CGetVerTextices::ProcessNormal(inputMesh, mMeshDatas);
	CGetVerTextices::ProcessUVs(inputMesh, mMeshDatas);
#else

	CGetVerTextices::ProcessVerticesAll(inputMesh, mMeshDatas);
	CGetVerTextices::ProcessNormalsAll(inputMesh, mMeshDatas);	
	CGetVerTextices::ProcessUVAll(inputMesh, mMeshDatas);
#endif


	ProcessMaterials(inputMesh, mMeshDatas);
	ProcessTransformations(inputMesh, mMeshDatas);
	ProcessJoints(inputMesh, mMeshDatas);
	
	MeshCalculate(mMeshDatas);
	m_vceMeshDatas.push_back(mMeshDatas);
	m_MeshCount += 1;

	return true;
}


bool CRootNode::RecursiveChildTraversal(FbxNode* inputNode)
{
	JUDEG_RETURN((nullptr == inputNode), false)

	int parentIndex = m_vceMeshDatas.size() - 1;
	for (int child = 0; child < inputNode->GetChildCount(); child++)
	{
		FbxNode* currNode = inputNode->GetChild(child);
		if (currNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			MBDMeshBag mMeshDatas;
			memcpy_s(mMeshDatas.m_meshname, _max_name_len, currNode->GetMesh()->GetName(), _max_name_len - 1);
			mMeshDatas.m_materialid = m_MeshCount;
			mMeshDatas.m_parentmeshid = parentIndex;
			//m_vceMeshDatas[parentIndex].childMeshList.push_back(m_MeshCount);

			ProcessMesh((FbxMesh*)currNode->GetNodeAttribute(), mMeshDatas);
			RecursiveChildTraversal(currNode);
		}
	}

	return true;
}

bool CRootNode::ProcessNodeAttribute(FbxNode* pmRootNode)
{
	JUDEG_RETURN((nullptr == pmRootNode), false)

	int childCount = pmRootNode->GetChildCount();
	for (int childIndex = 0; childIndex < childCount; childIndex++)
	{
		/*Getting the child nodes in the scene and their node attribute types.*/
		FbxNode* childNode = pmRootNode->GetChild(childIndex);
		if (childNode->GetNodeAttribute() == nullptr)
		{
			continue;
		}
		/*No attributes, then skip!*/
		if (nullptr == childNode->GetNodeAttribute())
		{
			continue;
		}

		FbxNodeAttribute::EType attributeType = childNode->GetNodeAttribute()->GetAttributeType();

		switch (attributeType)
		{
		case FbxNodeAttribute::eMesh:
		{

										MBDMeshBag mMeshDatas;
										memcpy_s(mMeshDatas.m_meshname, _max_name_len, childNode->GetName(), _max_name_len - 1);
										mMeshDatas.m_materialid = m_MeshCount;
										//FbxMesh* currMesh = (FbxMesh*)childNode->GetNodeAttribute();
										CGetVerTextices::GetGeometric(childNode);
										ProcessMesh(childNode->GetMesh(), mMeshDatas);

										RecursiveChildTraversal(childNode);

		}
			break;
		case FbxNodeAttribute::eLight:
		{
										 //ProcessLight((FbxLight*)childNode->GetNodeAttribute());
		}
			break;
		case FbxNodeAttribute::eCamera:
		{
										 //ProcessCamera((FbxCamera*)childNode->GetNodeAttribute());
		}
			break;
		default:
			break;
		}

		ProcessNodeAttribute(childNode);
	}

	return true;
}


bool CRootNode::ProcessMeshNode(FbxNode* pmRootNode)
{
	JUDEG_RETURN((nullptr == pmRootNode), false)

	int meshCounter2 = 0;
	//Second pass to get the mesh-bbox-children into the vector
	for (int childIndex = 0; childIndex < pmRootNode->GetChildCount(); childIndex++)
	{
		FbxNode* childNode = pmRootNode->GetChild(childIndex);
		if (nullptr == childNode->GetNodeAttribute())
		{
			continue;
		}

		FbxNodeAttribute::EType attributeType = childNode->GetNodeAttribute()->GetAttributeType();
		if (attributeType != FbxNodeAttribute::eMesh)
		{
			continue;
		}

		FbxMesh* currMesh = (FbxMesh*)childNode->GetNodeAttribute();
		unsigned int deformerCount = currMesh->GetDeformerCount(FbxDeformer::eSkin);
		for (unsigned int deformerCounter = 0; deformerCounter < deformerCount; ++deformerCounter)
		{
			FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(currMesh->GetDeformer(0, FbxDeformer::eSkin));
			if (!currSkin)
			{
				continue;
			}

			const unsigned int clusterCount = currSkin->GetClusterCount();
			for (unsigned int clusterCounter = 0; clusterCounter < clusterCount; ++clusterCounter)
			{
				FbxCluster* currCluster = currSkin->GetCluster(clusterCounter);
				FbxNode* currJoint = currCluster->GetLink();
				const int currJointID = FindJointIndexByName(currJoint->GetName());

				const unsigned int jointChildCount = currJoint->GetChildCount();
				for (int i = 0; i < jointChildCount; i++)
				{
					if (nullptr == currJoint->GetChild(i)->GetNodeAttribute())
					{
						continue;
					}
					if (currJoint->GetChild(i)->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
					{
						FbxNode* meshChild = currJoint->GetChild(i);

						MBDMeshBag   importMeshData;
						memcpy_s(importMeshData.m_meshname, _max_name_len, childNode->GetMesh()->GetName(), _max_name_len - 1);
						for (int ting = 0; ting < m_vceSceneJoints.size(); ++ting)
						{
							if (m_vceSceneJoints[ting].m_id == currJointID)
							{
								m_vceSceneJoints[ting].m_child_mesh.push_back(importMeshData.m_materialid);
								m_vceSceneJoints[ting].ChildMeshCount = m_vceSceneJoints[ting].m_child_mesh.size();
							}
						}

						
						FbxMesh* currMesh = (FbxMesh*)meshChild->GetNodeAttribute();
						ProcessMesh(currMesh, importMeshData);
						m_MeshCount += 1;

						RecursiveChildTraversal(meshChild);
					}
				}
			}
		}
		meshCounter2++;
	}

	return true;
}
