#pragma once
#include "stdafx.h"
#include "Matrix.h"
#include <osg/Matrix>
#include <osg/GLBeginEndAdapter>
#include <OpenThreads\Thread>
#include <assert.h>
class texImp;
class heightField;
enum STATUS
{
	VS_DISABLE = 0x00,
	VS_ACTIVE = 0x01,
	VS_CULLED = 0x02,
	VS_STOPED = 0x04,
	VS_UNREACH = 0x08
};
enum NEIGHBOR
{
	NV_L = 0,
	NV_U,
	NV_D,
	NV_R

};
typedef struct _NODE
{
	int _x, _y;
	int _lod;
	_NODE()
	{
		_x = 0;
		_y = 0;
		_lod = -1;
	}
	_NODE(int x, int y, int l)
	{
		_x = x;
		_y = y;
		_lod = l;
	}
	void setLOD(int l)
	{
		_lod = l;
	}
	int getLOD()
	{
		return _lod;
	}
} LNODE;


template <class T>
struct _VECTOR
{
	mutable T _x, _y, _z;
	_VECTOR()
	{
		_x = 0;
		_y = 0;
		_z = 0;


	}
	_VECTOR(T x, T y, T z)
	{

		_x = x;
		_y = y;
		_z = z;
	}
	T operator*(const _VECTOR& n) const
	{

		return _x*n._x + _y*n._y + _z*n._z;
	}
	_VECTOR operator-(const _VECTOR& n)const
	{

		T nx = _x - n._x;
		T ny = _y - n._y;
		T nz = _z - n._z;
		return _VECTOR(nx, ny, nz);
	}
	_VECTOR operator+(const _VECTOR& n)const
	{

		T nx = _x + n._x;
		T ny = _y + n._y;
		T nz = _z + n._z;
		return _VECTOR(nx, ny, nz);
	}
	_VECTOR operator/(float s)const
	{
		assert(s > 0.01);
		_x = _x / s;
		_y = _y / s;
		_z = _z / s;
		return *this;

	}
	//Cross Dot
	_VECTOR getCross(const _VECTOR& n)const
	{
		T nx = _y*n._z - _z*n._y;
		T ny = _z*n._x - _x*n._z;
		T nz = _x*n._y - _y*n._x;
		return _VECTOR(nx, ny, nz);
	}
	void normalize()
	{
		float len = sqrt(double(_x*_x + _y*_y + _z*_z));

		if (len > 0.01)
		{
			_x = _x / len;
			_y = _y / len;
			_z = _z / len;

		}
		else
		{
			_x = 0;
			_y = 1;
			_z = 0;
		}

	}
	T length()
	{
		float fL = sqrt(double(_x*_x + _y*_y + _z*_z));
		return *((T*)(&fL));

	}
};
//Alias for vector,specially used for normal.


typedef _VECTOR<float> VECTOR;


typedef struct _PatchSize
{
	_PatchSize()
	{
		_x = 0;
		_y = 0;
	}
	void set(int x, int y)
	{
		_x = x;
		_y = y;
	}
	int _x, _y;

} PatchSize;

typedef struct _Range
{

	int _width, _height;
	int _centerX, _centerY;
	int _index_i, _index_j;
	int _N;
	bool operator==(const _Range& other)
	{
		if (_width != other._width)
			return false;
		if (_height != other._height)
			return false;
		if (_centerX != other._centerX)
			return false;
		if (_centerY != other._centerY)
			return false;

		return true;
	}
} Range;


class VAO
{

public:
	VAO();
	~VAO();
public:
	void initVertex(const float* vertexs);
	void initVertex(const BYTE* heightMap, int offset_col, int offset_row, Range rlocal, Range rglobal, int offset_x, int offset_y, int offset_z = -100, bool isFlip = true);
	void initVertex(const BYTE* heightMap, int row_offset, int col_offset, int w, int h, int offset_x, int offset_y, int offset_z, bool isFlip = true);
	void updateIndex(const UINT* indx, int size);
	void draw(int startIndex, int num);
private:
	GLuint* m_index;
	GLuint m_ibo, m_vbo;
	GLuint m_vao;
	std::vector<GLfloat> m_vecVertex;
	std::vector<GLint> m_vecIndex;
};
class LODTile
{
public:
	LODTile();

	void init(BYTE* heightMat, const Range globalRange, const Range localRange);
	void init(heightField* input, int i, int j, int N);
	void updateCameraInfo(osg::Vec3d& eye);
	void updateCameraInfo(osg::Vec3d&	eye, osg::GLBeginEndAdapter& gl, osg::State* stat);
	int   GetHeight(int x, int y) const;
	float GetAveHeight(float x, float y) const;
	void BFSRender() const;
	void BFSRenderPrimitive() const;
	void DrawIndexedPrimitive() const;
	Range getLocalRange();

private:
	void initParams();
	inline void GLVERTEX_MESH(int x, int y) const;
	inline void GLVERTEX_TEX(int x, int y) const;
	inline void GLVERTEX(int x, int y) const;
	inline void local2Global(int& x, int& y, int& z) const;


	void DrawNode(int cx, int cy, int dx, int dy) const;
	void DrawPrim(int cx, int cy) const;

	BOOL NodeCanDivid(int cx, int cy, int dx, int dy) const;
	void DividNode(int cx, int cy, int dx, int dy) const;
	void DisableNode(int cx, int cy, int dx, int dy) const;

	void CheckNeighbor(int cx, int cy, int dx, int dy) const;
	unsigned char CanActive(int x, int y, int pitchSizeX, int pitchSizeY) const;
	VECTOR getNormal(int x, int y, int dx, int dy) const;

	void CalculateDHMatrix();

	void DrawPrim_TEXTURE(int x, int y) const;
	void DrawPrim_FRAME(int x, int y) const;

	void DrawNode_TEXTURE(int x, int y, int dx, int dy) const;
	void DrawNode_FRAME(int x, int y, int dx, int dy) const;
private:
	CMatrix<BYTE> m_HMMatrix;
	CMatrix<BYTE>    m_LodMatrix;
	CMatrix<float> m_DHMatrix;
	float m_fc;
	float m_fC;
	Range m_rlocalPara;
	Range m_rglobalPara;
	mutable float m_ViewX, m_ViewY, m_ViewZ;
	PatchSize   m_delta[30];
	mutable int   m_neighbor[4];
	float         m_fScale;
#if 0
	mutable osg::GLBeginEndAdapter m_gl;
	mutable osg::State* m_stat;
#endif
	std::unique_ptr<texImp> m_texture;
	mutable VAO m_vertexBuf;
	mutable std::vector<UINT> m_indexBuf;
};

