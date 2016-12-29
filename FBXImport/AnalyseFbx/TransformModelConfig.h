#ifndef _TRANSFORMMODELCONFIG_CONFIG_H
#define _TRANSFORMMODELCONFIG_CONFIG_H

#include "GLInclude.h"
#include <vector>


const float _pi = 3.14159265359;
const float _delta = 0.0001;
#define EQUAL(A,B) (abs((A)-(B)) < _delta) ? true:false

const int _max_name_len = 128;
const int _max_file_len = 256;
const int _error_value = -1;

//const char* cMashExt = ".mobd";
//const char* cAnimatedExt = ".Ani";
//const char* cMatrixExt = ".mad";
#define MASH_EXT		(".mobd")
#define ANIMATED_EXT	(".animated")
#define Matrix_Ext		(".matrix")

namespace ar_engine
{
typedef signed char   s8;
typedef unsigned char u8;
typedef char          c8;
typedef unsigned short int u16;
typedef signed   short int s16;
typedef unsigned  int u32;
typedef signed    int s32;
typedef unsigned long long u64;
typedef signed long long s64;
typedef bool   b2;
typedef float  f32;
typedef double f64;

typedef glm::vec4 vec4;
typedef glm::vec3 vec3;
typedef glm::vec2 vec2;
typedef glm::mat4 Matrix;
typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::quat Quatation;
typedef glm::quat quat;

#define  _PUSH_SIZE  4

#pragma pack(push)
#pragma pack(_PUSH_SIZE)
	struct MBDHeader
	{
		char m_id[8];//默认MBD01
		int  m_version;
		MBDHeader() : m_version(2017)
		{
			memset(m_id, 0, 8);
			memcpy(m_id, "MBD01", 5);
		}
	};
#pragma pack(pop)

#pragma pack(push)
#pragma pack(_PUSH_SIZE)
	struct MBDDataheader
	{
		MBDDataheader()
		{
			memset(this, 0, sizeof(MBDDataheader));
		}

		u32 m_datasize;
		u32 m_buffersize;

		u32 m_meshcount;
		u32 m_matatrilcount; 
		u32 m_matrixcount;
		u32 m_joints;

		u32 m_keycounts;//?
		u32 m_keyframes;

		u32 m_isAnimated;

		u32 m_vertices;
		u32 m_skeletonVertices;

		f32 m_fps;                 // 4 bytes
		f32 m_cur_time;            // 4 bytes
		
	};
#pragma pack(pop)


#pragma pack(push)
#pragma pack(_PUSH_SIZE)
	class MBDVertex
	{
	public:
		MBDVertex() : _position(0.0f, 0.0f, 0.0f),
			_normal(0.0f, 0.0f, 0.0f),
			_texcoord(0.0f, 0.0f)
		{}

		MBDVertex(const vec3& p, const vec3& n, const vec2& t) :
			_position(p), _normal(n), _texcoord(t)
		{}

		vec3 _position;
		vec3 _normal;
		vec2 _texcoord;
	};

#pragma pack(pop)

#pragma pack(push)
#pragma pack(_PUSH_SIZE)
	class MBDSkelVertex
	{
	public:
		vec3 _position;
		vec3 _normal;
		vec2 _texcoord;

		vec4 _bone_indices; /** bone indices that affect this vertex ( for animated mesh only)*/
		vec4 _bone_weights; /** bone weights that affect this vertex for animated mes only)*/

		/** constructor*/
		MBDSkelVertex() :
			_position(vec3(0.0)), _normal(vec3(0.0)), _texcoord(vec2(0.0)),
			_bone_indices(vec4(-1)), _bone_weights(vec4(0.0))
		{}

		MBDSkelVertex(const vec3& pos_, const vec3& normal_, const vec2& texCoord_,
			const vec4& boneIndices_, const vec4 & boneWeights_)
			:
			_position(pos_), _normal(normal_), _texcoord(texCoord_),
			_bone_indices(boneIndices_), _bone_weights(boneWeights_)
		{}

		MBDSkelVertex(const MBDSkelVertex & v)
		{
			_position = v._position;
			_normal = v._normal;
			_texcoord = v._texcoord;
			_bone_indices = v._bone_indices;
			_bone_weights = v._bone_weights;
		}

		MBDSkelVertex & operator=(MBDSkelVertex & v)
		{
			_position = v._position;
			_normal = v._normal;
			_texcoord = v._texcoord;
			_bone_indices = v._bone_indices;
			_bone_weights = v._bone_weights;
			return *this;
		}
	};
#pragma pack(pop)

#pragma pack(push)
#pragma pack(_PUSH_SIZE)
	struct MBDKeyFrame
	{
		MBDKeyFrame() :m_keytime(0.0), m_keypos(0.0, 0.0, 0.0)
		{}
		float m_keytime;
		vec3  m_keypos;
	};
#pragma pack(pop)

#pragma pack(push)
#pragma pack(_PUSH_SIZE)
	struct MBDJoint
	{
		int  m_parentid;
		u32  m_id;
		u32 ChildMeshCount;
		u32 keyTranslateCount;
		u32 keyRotationCount;

		char m_name[_max_name_len];
		char m_parent_name[_max_name_len];

		vec3 m_rotation;// local animation matrices
		vec3 m_translation;
		vec3 m_scale;

		mat4 m_pose_inverse;//local animation matrices
		mat4 m_globalpose_inverse;// local animation matrices
		
		std::vector<u32> m_child_mesh;
		std::vector<MBDKeyFrame> m_vecTranslate;
		std::vector<MBDKeyFrame> m_vecRotation;
		int GetSizeInImport()
		{
			int rt = ChildMeshCount* sizeof(32) +
				(keyTranslateCount + keyRotationCount)* sizeof(MBDKeyFrame)+
				sizeof(int) + 4 * sizeof(u32) + 2 * sizeof(vec3) + 2 * sizeof(mat4);
			return rt;
		}

		MBDJoint()
		{
			Release();
		}

		~MBDJoint()
		{
			Release();
		}

		void Release()
		{
			ChildMeshCount = 0;
			keyTranslateCount = 0;
			keyRotationCount = 0;
			m_child_mesh.clear();
			m_vecTranslate.clear();
			m_vecRotation.clear();
		}
	};
#pragma pack(pop)


#pragma pack(push)
#pragma pack(_PUSH_SIZE)
	struct MBDMatrix
	{
	    u32 m_count;//动作数量,
		u32 m_framenum;//动作帧数,
		char  m_name[_max_name_len];//动作名称[NAME1],[NAME2],[NAME3]
	    //double* m_time;//N个double时间,T1,T2,T3
	    //mat4* m_mat;//N的矩阵//[R,T,S,0],[],[]
		std::vector<double> m_vecTime;
		std::vector<mat4> m_vecMat;

		int GetSizeInImport()//
		{
			int rt = sizeof(u32)* 2 + 
				_max_name_len +
				(sizeof(mat4) + sizeof(double)) * m_framenum;
			return rt;
		}

		MBDMatrix() :
			m_count(1),
			m_framenum(0)
		{
			memset(m_name, 0, _max_name_len* sizeof(char));
			Release();
		}

		~MBDMatrix()
		{
			Release();
		}

		void Release()
		{
			m_vecTime.clear();
			m_vecMat.clear();
		}
	};
#pragma pack(pop)

#pragma pack(push)
#pragma pack(_PUSH_SIZE)
	struct MBDMesh
	{
		char m_meshname[_max_name_len];
		u32 m_materialid;
		u32 m_parentmeshid;
		u32 m_parentjointid;

		vec3 m_translation;//此三个vec3构成补间动画的matrix
		vec3 m_rotation;
		vec3 m_scale;

		u32 m_isanimated;

		u32 m_meshchildcount;
		u32 m_vertecount;
		u32 m_skeletonvetexcount;
		u32 m_indexcount;

		MBDMesh():
			m_materialid(0),
			m_parentmeshid(0),
			m_parentjointid(0),
			m_isanimated(0),
			m_meshchildcount(0),
			m_skeletonvetexcount(0),
			m_indexcount(0)
		{}
	};
#pragma pack(pop)

#pragma pack(push)
#pragma pack(_PUSH_SIZE)
	struct MBDMeshBag : public MBDMesh
	{

		std::vector<u32> mIndex;
		std::vector<MBDVertex> mVertexList;
		std::vector<MBDSkelVertex> mSkelVertexList;

		void Release()
		{
			mIndex.clear();
			mVertexList.clear();
			mSkelVertexList.clear();
		}

		MBDMeshBag() :MBDMesh()
		{
			Release();
		}

		~MBDMeshBag()
		{
			Release();
		}

		unsigned int GetSizeInImport()
		{
			unsigned int rt = sizeof(MBDMesh)+
				sizeof(u32)* mIndex.size();
			rt += m_isanimated ? sizeof(MBDSkelVertex)* mSkelVertexList.size() : sizeof(MBDVertex)* mVertexList.size();
			return rt;
		}
	};
#pragma pack(pop)


	enum TextTureType
	{
		eTextureDiffuse,
		eTextureDiffuseFactor,
		eTextureEmissive,
		eTextureEmissiveFactor,
		eTextureAmbient,
		eTextureAmbientFactor,
		eTextureSpecular,
		eTextureSpecularFactor,
		eTextureShininess,
		eTextureNormalMap,
		eTextureBump,
		eTextureTransparency,
		eTextureTransparencyFactor,
		eTextureReflection,
		eTextureReflectionFactor,
		eTextureDisplacement,
		eTextureDisplacementVector,
	};


#pragma pack(push)
#pragma pack(_PUSH_SIZE)
	struct MBDMaterialBase
	{
		char  m_name[_max_name_len];
		vec3  m_ambient;
		vec3  m_diffuse;
		vec3  m_specular;
		vec3  m_emissive;
		float m_shininess;	// 0.0f - 128.0f
		float m_transparency;// 0.0f - 1.0f
	};

#pragma pack(pop)

#pragma pack(push)
#pragma pack(_PUSH_SIZE)
	struct MBDMaterial :public MBDMaterialBase
	{
		//char  m_texture[_max_file_len];// texture.bmp
		//char  m_alphamap[_max_file_len];// alpha.bmp
		u32 m_textcount;
		std::vector<TextTureType> m_vecTextType;
		std::vector<std::string> m_vecTetxure;
		//char** m_ppTextList;//m_textcount*_max_file_len

		MBDMaterial():m_textcount(0)
		{
			Release();
		}

		~MBDMaterial()
		{
			Release();
		}

		void Release()
		{
			m_vecTextType.clear();
			m_vecTetxure.clear();
		}

		u32 GetSizeInImport()
		{
			u32 rt = sizeof(MBDMaterialBase) +
				sizeof(u32) +
				sizeof(TextTureType)* m_vecTextType.size();
			rt += m_textcount * _max_file_len;

			return rt;
		}
	};
#pragma pack(pop)
	
};


#endif