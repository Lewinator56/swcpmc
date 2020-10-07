// swcpmc.cpp : Defines the entry point for the application.
//

#include "swcpmc.h"

using namespace std;
using byte = unsigned char;

class Triangle {
public:
	uint16_t _v1;
	uint16_t _v2;
	uint16_t _v3;

	Triangle(uint16_t v1, uint16_t v2, uint16_t v3) {
		_v1 = v1;
		_v2 = v2;
		_v3 = v3;
	}
};

class Normal {
public:
	float _x;
	float _y;
	float _z;
	Normal() {

	}
	Normal(float x, float y, float z) {
		_x = x;
		_y = y;
		_z = z;
	}
};

class Vertex {
public:
	float _px;
	float _py;
	float _pz;
	byte _r;
	byte _g;
	byte _b;
	byte _a;
	Normal _n;
	Vertex(float px, float py, float pz, byte r, byte g, byte b, byte a, Normal n) {
		_px = px;
		_py = py;
		_pz = pz;
		_r = r;
		_g = g;
		_b = b;
		_a = a;
		_n = n;
	}
	Vertex(float px, float py, float pz, byte r, byte g, byte b, byte a) {
		_px = px;
		_py = py;
		_pz = pz;
		_r = r;
		_g = g;
		_b = b;
		_a = a;
	}
	void setNormal(Normal n) {
		_n = n;
	}
};

class SubMesh {
public:
	vector<Vertex> _vertices;
	uint16_t _shader;
	float _cullingMax[3];
	float _cullingMin[3];
	vector<Triangle> _triangles;
	byte _r;
	byte _g;
	byte _b;
	byte _a;

	void addVertex(Vertex v) {
		_vertices.push_back(v);
	}
	void setCullingMin(float x, float y, float z) {
		_cullingMin[0] = x < _cullingMin[0] ? x : _cullingMin[0];
		_cullingMin[1] = y < _cullingMin[1] ? y : _cullingMin[1];
		_cullingMin[2] = z < _cullingMin[2] ? z : _cullingMin[2];
	}
	void setcullingMax(float x, float y, float z) {
		_cullingMax[0] = x > _cullingMax[0] ? x : _cullingMax[0];
		_cullingMax[1] = y > _cullingMax[1] ? y : _cullingMax[1];
		_cullingMax[2] = z > _cullingMax[2] ? z : _cullingMax[2];
	}
	void setShader(uint16_t shaderID) {
		_shader = shaderID;
	}
	void addTriangle(Triangle t) {
		_triangles.push_back(t);
	}


};



int main()
{
	Vertex v = Vertex(10.0, 10.0, 10.0, 5, 5, 5, 5);
}
