#include "GetVerTextices.h"
#include "..\..\PublicCommon\ToolsConfig.h"
#include <set>

CGetVerTextices::CGetVerTextices()
{
	
}

CGetVerTextices::~CGetVerTextices()
{}

void CGetVerTextices::UpdateUV(FbxVector2 v2Put, vec2& v2Get)
{
	v2Get[0] = v2Put.mData[0];
	v2Get[1] = 1 - v2Put.mData[1];
}

void CGetVerTextices::UpdateNormal(FbxVector4 v4Put, vec3& v3Get)
{
	/*
	//glm::mat4 orientation;
	//vec3 _normal;
	//glm::rotate(_normal, v4Put.mData[3]), angle, vec3(0, 1, 0));
	//v3Get[0] = v4Put.mData[0];
	//v3Get[1] = v4Put.mData[1];
	//v3Get[2] = v4Put.mData[2];


	const float angle = m_d4r.mData[0];
	glm::vec4 v4(v4Put.mData[0], v4Put.mData[1], v4Put.mData[2], v4Put.mData[3]);
	glm::vec3 v3(v4Put.mData[0], v4Put.mData[1], v4Put.mData[2]);// , v4Put.mData[3]);
	glm::vec3 r3(m_d4r.mData[0], m_d4r.mData[1], m_d4r.mData[2]);// , m_d4r.mData[3]);
	glm::vec3 s3(m_d4s.mData[0], m_d4s.mData[1], m_d4s.mData[2]);// , m_d4s.mData[3]);
	glm::vec3 t3(m_d4t.mData[0], m_d4t.mData[1], m_d4t.mData[2]);// , m_d4t.mData[3]);
	//glm::angleAxis(v, 90.0f);
	//glm::rotate(v, 90.0f);
	//v =  glm::rotate(v, 90.0f, glm::vec3(1.0, 1.0, 1.0));
	// v3 = glm::rotate(v3, 90.0f, s3);
	glm::vec3 v3_x(v3);
	glm::vec3 v3_y(v3);
	glm::vec3 v3_z(v3);

	v3_x = glm::rotateX(v3_x, float(m_d4r.mData[0]));
	v3_y = glm::rotateY(v3_y, float(m_d4r.mData[1]));
	v3_z = glm::rotateZ(v3_z, float(m_d4r.mData[2]));


	////glm::vec3 Result;// = glm::rotateX(v, angle);
	//const double Cos = cos(angle*_pi /180);
	//const double Sin = sin(angle*_pi / 180);

	//Result.y = v.y * Cos - v.z * Sin;
	//Result.z = v.y * Sin + v.z * Cos;
	*/
	v3Get[0] = v4Put.mData[0];
	v3Get[1] = v4Put.mData[1];
	v3Get[2] = v4Put.mData[2];

	//v3Get[0] = v4Put.mData[0];
	//v3Get[1] = v4Put.mData[2];
	//v3Get[2] = -v4Put.mData[1];
}

bool CGetVerTextices::GetGeometric(FbxNode* iNode)
{
	if (nullptr != iNode)
	{
		std::string strName = iNode->GetName();
		//FbxAnimEvaluator* animationEvaluator = iNode->GetAnimationEvaluator();
		m_d4r = iNode->LclRotation.Get();
		m_d4t = iNode->LclTranslation.Get();
		m_d4s = iNode->LclScaling.Get();

		//glm::rotate;
		m_GeometricT = iNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		//printf("GetGeometric_%d_%s_%f,%f,%f\n", iNode->GetNodeAttribute()->GetAttributeType(), iNode->GetName(), m_GeometricT[0], m_GeometricT[1], m_GeometricT[2]);
		m_GeometricR = iNode->GetGeometricRotation(FbxNode::eSourcePivot);
		m_GeometricS = iNode->GetGeometricScaling(FbxNode::eSourcePivot);
	}
	return true;
}


bool CGetVerTextices::ProcessNormalsAll(FbxMesh* inputMesh, MBDMeshBag& mMeshDatas)
{
	JUDEG_RETURN((nullptr == inputMesh), false)

		/*Get the normal element of the mesh.*/
		FbxGeometryElementNormal* normalElement = inputMesh->GetElementNormal();
	JUDEG_RETURN((nullptr == normalElement), false)

		/*Obtain normal of each vertex.*/
	if (normalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		/*Obtain the normals of each vertex, because the mapping mode of the normal element is by control point.*/
		for (int vertexIndex = 0; vertexIndex < inputMesh->GetControlPointsCount(); vertexIndex++)
		{
			int normalIndex = 0;

			/*If reference mode is direct, it means that the normal index is the same as a vertex index.*/
			if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
			{
				normalIndex = vertexIndex;
			}

			/*If the reference mode is Index-to-Direct, it means that the normals are obtained by the Index-to-Direct.*/
			if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
			{
				normalIndex = normalElement->GetIndexArray().GetAt(normalIndex);
			}

			/*Normals of each vertex is obtained.*/
			FbxVector4 normals = normalElement->GetDirectArray().GetAt(normalIndex);
			if (mMeshDatas.m_isanimated)
			{
				UpdateNormal(normals, mMeshDatas.mSkelVertexList.at(vertexIndex)._normal);
			}
			else
			{
				UpdateNormal(normals, mMeshDatas.mVertexList.at(vertexIndex)._normal);
			}

		}
	}
	/*Get the normals by obtaining polygon-vertex.*/
	else if (normalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		int indexPolygonVertex = 0;
		int iPolygoncount = inputMesh->GetPolygonCount();
		/*Obtain normals of each polygon, because the mapping mode of normal element is by Polygon-Vertex.*/
		for (int polygonIndex = 0; polygonIndex < iPolygoncount; polygonIndex++)
		{
			/*Get the polygon size, to know how many vertices in current polygon.*/
			int polygonSize = inputMesh->GetPolygonSize(polygonIndex);
			assert(polygonSize == 3);
			for (int i = 0; i < polygonSize; i++) //Obtain each vertex of the current polygon.
			{
				int normalIndex = 0;
				if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					normalIndex = indexPolygonVertex;
				}
				if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
				{
					normalIndex = normalElement->GetIndexArray().GetAt(indexPolygonVertex);
				}

				FbxVector4 normals = normalElement->GetDirectArray().GetAt(normalIndex); //Obtain normals of each polygon-vertex
				if (mMeshDatas.m_isanimated)
				{
					UpdateNormal(normals, mMeshDatas.mSkelVertexList.at(indexPolygonVertex)._normal);
				}
				else
				{
					UpdateNormal(normals, mMeshDatas.mVertexList.at(indexPolygonVertex)._normal);
				}

				indexPolygonVertex++;
			}
		}
	}

	return true;
}


bool CGetVerTextices::ProcessUVAll(FbxMesh* pMesh, MBDMeshBag& mMeshDatas)
{
	int i, j, lPolygonCount = pMesh->GetPolygonCount();
	FbxVector4* lControlPoints = pMesh->GetControlPoints();

	int vertexId = 0;
	for (i = 0; i < lPolygonCount; i++)
	{
		int lPolygonSize = pMesh->GetPolygonSize(i);
		for (j = 0; j < lPolygonSize; j++)
		{
			int lControlPointIndex = pMesh->GetPolygonVertex(i, j);
			for (int l = 0; l < pMesh->GetElementUVCount(); ++l)
			{
				FbxVector2 v2;
				FbxGeometryElementUV* leUV = pMesh->GetElementUV(l);
				switch (leUV->GetMappingMode())
				{
				default:
					break;
				case FbxGeometryElement::eByControlPoint:
					switch (leUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						v2 = leUV->GetDirectArray().GetAt(lControlPointIndex);
						if (mMeshDatas.m_isanimated)
						{
							UpdateUV(v2, mMeshDatas.mSkelVertexList.at(vertexId)._texcoord);
						}
						else
						{
							UpdateUV(v2, mMeshDatas.mVertexList.at(vertexId)._texcoord);
						}
						//Display2DVector(header, leUV->GetDirectArray().GetAt(lControlPointIndex));
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
															   int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
															   v2 = leUV->GetDirectArray().GetAt(id);
															   if (mMeshDatas.m_isanimated)
															   {
																   UpdateUV(v2, mMeshDatas.mSkelVertexList.at(vertexId)._texcoord);
															   }
															   else
															   {
																   UpdateUV(v2, mMeshDatas.mVertexList.at(vertexId)._texcoord);
															   }
															   //Display2DVector(header, leUV->GetDirectArray().GetAt(id));
					}
						break;
					default:
						break; // other reference modes not shown here!
					}
					break;

				case FbxGeometryElement::eByPolygonVertex:
				{
															 int lTextureUVIndex = pMesh->GetTextureUVIndex(i, j);
															 switch (leUV->GetReferenceMode())
															 {
															 case FbxGeometryElement::eDirect:
															 case FbxGeometryElement::eIndexToDirect:
															 {
																										v2 = leUV->GetDirectArray().GetAt(lTextureUVIndex);
																										if (mMeshDatas.m_isanimated)
																										{
																											UpdateUV(v2, mMeshDatas.mSkelVertexList.at(vertexId)._texcoord);
																										}
																										else
																										{
																											UpdateUV(v2, mMeshDatas.mVertexList.at(vertexId)._texcoord);

																										}
															 }
																 break;
															 default:
																 break; // other reference modes not shown here!
															 }
				}
					break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
					break;
				}
			}


			vertexId++;
		} // for polygonSize
	} // for polygonCount
	return true;
}


bool CGetVerTextices::ProcessVerticesAll(FbxMesh* inputMesh, MBDMeshBag& mMeshDatas)
{
	JUDEG_RETURN((nullptr == inputMesh), false)
		//inputMesh->SetControlPointCount(3);
		FbxVector4* vertices = inputMesh->GetControlPoints();
	mMeshDatas.m_isanimated = bool(inputMesh->GetDeformerCount(FbxDeformer::eSkin) > 0);
	int polyCount = inputMesh->GetPolygonCount();
	for (int i = 0; i < polyCount; i++)
	{
		int numPolygonVertices = inputMesh->GetPolygonSize(i);
		assert(numPolygonVertices == 3);
		for (int j = 0; j < numPolygonVertices; j++)
		{
			int polygonVertex = inputMesh->GetPolygonVertex(i, j);
			//FbxVector4 normals;
			//ReadNormal(inputMesh, polygonVertex, vertexCounter, &normals);
			if (mMeshDatas.m_isanimated)
			{
				PushVertices(vertices[polygonVertex], mMeshDatas.mSkelVertexList);
				//UpdateNormal(normals, mMeshDatas.mSkelVertexList.at(vertexCounter)._normal);
			}
			else
			{
				PushVertices(vertices[polygonVertex], mMeshDatas.mVertexList);
				//UpdateNormal(normals, mMeshDatas.mVertexList.at(vertexCounter)._normal);
			}

			mMeshDatas.mIndex.push_back(polygonVertex);
		}
	}
	return true;
}


void CGetVerTextices::ReadNormal(FbxMesh* pMesh, int ctrlPointIndex, int vertexCounter, FbxVector4* pNormal)
{
	if (pMesh->GetElementNormalCount() < 1)
	{
		return;
	}

	FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal(0);
	switch (leNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
												switch (leNormal->GetReferenceMode())
												{
												case FbxGeometryElement::eDirect:
												{
																					*pNormal = leNormal->GetDirectArray().GetAt(ctrlPointIndex);
												}
													break;

												case FbxGeometryElement::eIndexToDirect:
												{
																						   int id = leNormal->GetIndexArray().GetAt(ctrlPointIndex);
																						   *pNormal = leNormal->GetDirectArray().GetAt(id);
												}
													break;

												default:
													break;
												}
	}
		break;

	case FbxGeometryElement::eByPolygonVertex:
	{
												 switch (leNormal->GetReferenceMode())
												 {
												 case FbxGeometryElement::eDirect:
												 {
																					 *pNormal = leNormal->GetDirectArray().GetAt(vertexCounter);
												 }
													 break;

												 case FbxGeometryElement::eIndexToDirect:
												 {
																							int id = leNormal->GetIndexArray().GetAt(vertexCounter);
																							*pNormal = leNormal->GetDirectArray().GetAt(id);
												 }
													 break;

												 default:
													 break;
												 }
	}
		break;
	}
}

///////////////////////////////////////

bool CGetVerTextices::processIndices(FbxMesh* inputmesh, MBDMeshBag& mMeshDatas)
{
	const int* pin = inputmesh->GetPolygonVertices();
	if (nullptr == pin)
	{
		return false;
	}

	int  counts = inputmesh->GetPolygonVertexCount();
	if (counts > 0)
	{
		mMeshDatas.mIndex.resize(counts);
		memcpy_s(mMeshDatas.mIndex.data(), counts*sizeof(int), pin, counts*sizeof(int));
	}


	//unsigned int uPolyCont = inputmesh->GetPolygonCount();
	//for (unsigned int polyCounter = 0; polyCounter < uPolyCont; polyCounter++)
	//{
	//	const unsigned int polySize = inputmesh->GetPolygonSize(polyCounter);
	//	assert(polySize == 3 && "The size of polygon nr: %d is not 3.", polyCounter);
	//	for (unsigned int polyCorner = 0; polyCorner < 3; polyCorner++)
	//	{
	//		mMeshDatas.mIndex.push_back(inputmesh->GetPolygonVertex(polyCounter, polyCorner));
	//	}
	//}
	return true;
}


bool CGetVerTextices::ProcessVertices(FbxMesh* inputMesh, MBDMeshBag& mMeshDatas)
{
	JUDEG_RETURN((nullptr == inputMesh), false)
	
	mMeshDatas.m_isanimated = bool(inputMesh->GetDeformerCount(FbxDeformer::eSkin) > 0);
	FbxVector4* vertices = inputMesh->GetControlPoints();
	const unsigned int controlPointCount = inputMesh->GetControlPointsCount();
	if (mMeshDatas.m_isanimated)
	{
		for (u32 cpCounter = 0; cpCounter < controlPointCount; ++cpCounter)
		{
			PushVertices(vertices[cpCounter], mMeshDatas.mSkelVertexList);
		}
	}
	else
	{
		for (u32 cpCounter = 0; cpCounter < controlPointCount; ++cpCounter)
		{
			PushVertices(vertices[cpCounter], mMeshDatas.mVertexList);
		}
	}
	//#endif
	return true;
}


bool CGetVerTextices::ProcessNormal(FbxMesh* inputMesh, MBDMeshBag& mMeshDatas)
{
	JUDEG_RETURN((nullptr == inputMesh), false)

	FbxGeometryElementNormal* pnormalelement = inputMesh->GetElementNormal();
	if (pnormalelement &&
		pnormalelement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
		pnormalelement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
	{
		return false;
	}

	FbxArray<FbxVector4> normals;
	normals.Clear();
	inputMesh->GetPolygonVertexNormals(normals);
	//FbxVector4 v0 = normals[0], v1 = normals[1], v2 = normals[2], v3 = normals[3];

	//const int* pin = inputMesh->GetPolygonVertices();
	//int ip0 = pin[0], ip1 = pin[1], ip2 = pin[2], ip3 = pin[3], ip4 = pin[4];
	//int cU = pnormalelement->GetIndexArray().GetCount();
	//int dU = pnormalelement->GetDirectArray().GetCount();
	int normal_counts = normals.Size();
	std::set<int> setIndex;
	setIndex.clear();
	for (int i = 0; i < normal_counts; i++)
	{
		int index = mMeshDatas.mIndex[i];//pin[i]
		if (setIndex.end() == setIndex.find(index))
		{
			setIndex.insert(index);
			//printf("%d ", index);
		}
		else
		{
			continue;
		}

		FbxVector4 vnormal(normals[i]);
		if (0 != mMeshDatas.m_isanimated)
		{
			UpdateNormal(vnormal, mMeshDatas.mSkelVertexList.at(index)._normal);
		}
		else
		{
			UpdateNormal(vnormal, mMeshDatas.mVertexList.at(index)._normal);
		}
	}

	
	return true;
}

bool CGetVerTextices::ProcessUVs(FbxMesh* inputMesh, MBDMeshBag& mMeshDatas)
{
	JUDEG_RETURN((nullptr == inputMesh), false)

	std::set<int>setUv;
	FbxStringList UVSetNameList;
	inputMesh->GetUVSetNames(UVSetNameList);
	for (int setIndex = 0; setIndex < UVSetNameList.GetCount(); setIndex++)
	{
		const char* UVSetName = UVSetNameList.GetStringAt(setIndex);
		const FbxGeometryElementUV* UVElement = inputMesh->GetElementUV(UVSetName);
		if (!UVElement ||
			(UVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
			UVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint))
		{
			continue;
		}


		FbxArray<FbxVector2> uvs;
		uvs.Clear();
		inputMesh->GetPolygonVertexUVs(UVSetName, uvs);
		//FbxVector2 v0 = uvs[0], v1 = uvs[1], v2 = uvs[2], v3 = uvs[3];

		//const int * pin = inputMesh->GetPolygonVertices();
		//int ip0 = pin[0], ip1 = pin[1], ip2 = pin[2], ip3 = pin[3], ip4 = pin[4];
		//int cU = UVElement->GetIndexArray().GetCount();
		//int dU = UVElement->GetDirectArray().GetCount();
		int uvcounts = uvs.Size();
		
		setUv.clear();
		for (int i = 0; i < uvcounts; i++)
		{
			int index = mMeshDatas.mIndex[i];//pin[i]
			if (setUv.end() == setUv.find(index))
			{
				setUv.insert(index);
				//printf("%d ", index);
			}
			else
			{
				continue;
			}

			FbxVector2 uv(uvs[i]);
			if (0 != mMeshDatas.m_isanimated)
			{
				UpdateUV(uv, mMeshDatas.mSkelVertexList.at(index)._texcoord);
			}
			else
			{
				UpdateUV(uv, mMeshDatas.mVertexList.at(index)._texcoord);
			}
		}
	}

	return true;
}
