// swcpmc.cpp : Defines the entry point for the application.
//

#include "swcpmc.h"
#include <array>
#include <sstream>

using namespace std;
using byte = unsigned char;
string _pathToMesh;
string _pathToOutput;
byte bin[];


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

class Color {
public:
	byte _r;
	byte _g;
	byte _b;
	byte _a;
	Color(byte r, byte g, byte b, byte a) {
		_r = r;
		_g = g;
		_b = b;
		_a = a;
	}
};

//string split
vector<string> split(string const& str, char const& delim) {
	stringstream ss(str);
	string s;
	vector<string> out;
	while (std::getline(ss, s, delim)) {
		out.push_back(s);
	}
	return out;

}




int main(int argc, char **argv)
{
	if (argc != 3) {
		cout << "invalid number of arguements, please run this tool from the command line" << endl;
		return 0;
	}
	else
	{
		_pathToMesh = argv[1];
		_pathToOutput = argv[2];
		cout << "Reading from " << _pathToMesh << ", Writing to " << _pathToOutput << endl;
	}

	// main bit of code

}



void objToMesh(string data[]) {
	string error = "";
	try {
		uint16_t vertexCount = 0;
		Color c = Color(255, 125, 0, 255);
		int vertexPosCount = 0;
		vector<Normal> normals;
		vector<int> subMeshVertices;
		vector<SubMesh> submeshes;
		int currentSubmesh = -1;
		subMeshVertices.push_back(0);
		bool needSubmesh = true;

		for (int i = 0; i < sizeof(data); i++) {
			try {
				error = data[i].substr(0, data[i].find_first_of(' '));
			}
			catch (exception) {}

			if (data[i].rfind('V', 0) == 0 && needSubmesh) {
				SubMesh s;
				s._r = c._r;
				s._g = c._g;
				s._b = c._b;
				s._a = c._a;
				currentSubmesh++;
				submeshes.push_back(s);
				needSubmesh = false;


			}

			// object 'o'
			if (data[i].rfind('o', 0) == 0) {
				if (data[i].find('/') != string::npos) {

					vector<string> cv = split(split(split(data[i], ' ')[1], '/')[0], '-');
					c._r = stoi(cv[0]);
					c._g = stoi(cv[1]);
					c._b = stoi(cv[2]);
					c._a = stoi(cv[3]);
				}
				else {
					c._r = 255;
					c._g = 125;
					c._b = 0;
					c._a = 255;
				}

				if (needSubmesh) {
					SubMesh s;
					s._r = c._r;
					s._g = c._g;
					s._b = c._b;
					s._a = c._a;
					currentSubmesh++;
					submeshes.push_back(s);
					needSubmesh = false;
				}
				else {
					submeshes[currentSubmesh]._r = c._r;
					submeshes[currentSubmesh]._g = c._g;
					submeshes[currentSubmesh]._b = c._b;
					submeshes[currentSubmesh]._a = c._a;
				}
			}

			// vertex 'v'
			else if (data[i].rfind('v', 0) == 0 && data[i].rfind("vt", 0) != 0 && data[i].rfind("vn", 0) != 0) {
				vector<string> v = split(data[i], ' ');
				float vx = stof(v[i]);
				float vy = stof(v[2]);
				float vz = stof(v[3]);
				Vertex vtx = Vertex(vx, vy, vz, c._r, c._g, c._b, c._a);
				submeshes[currentSubmesh].addVertex(vtx);
				submeshes[currentSubmesh].setcullingMax(vx, vy, vz);
				submeshes[currentSubmesh].setCullingMin(vx, vy, vz);
				vertexCount++;
				subMeshVertices[currentSubmesh + (int)1]++;
			}

			// vertex normals

			else if (data[i].rfind("vn", 0) == 0) {
				vector<string> n = split(data[i], ' ');
				float nx = stof(n[1]);
				float ny = stof(n[2]);
				float nz = stof(n[3]);
				normals.push_back(Normal(nx, ny, nz));
			}

			// material usemtl
			else if (data[i].rfind("usemtl", 0) == 0) {
				uint16_t shaderID = 0;
				if (data[i].find('/') != string::npos) {
					shaderID = stoi(split(split(data[i], ' ')[1], '/')[0]);
				}
				submeshes[currentSubmesh].setShader(shaderID);
			}

			// face f
			else if (data[i].rfind('f', 0) == 0) {
				vector<string> t = split(data[i], ' ');
				int nidxv1 = stoi(split(t[1], '/')[2]) - 1;
				int nidxv2 = stoi(split(t[2], '/')[2]) - 1;
				int nidxv3 = stoi(split(t[3], '/')[2]) - 1;
				uint16_t v1 = (uint16_t)stoi(split(t[1], '/')[0]) - 1;
				uint16_t v2 = (uint16_t)stoi(split(t[2], '/')[0]) - 1;
				uint16_t v3 = (uint16_t)stoi(split(t[3], '/')[0]) - 1;
				Triangle trg = Triangle(v1, v2, v3);
				int toSubtract = 0;
				for (int j = 0; j < subMeshVertices.size() - 1; j++) {
					toSubtract += subMeshVertices[j];
				}
				submeshes[currentSubmesh]._vertices[v1 - (uint16_t)toSubtract].setNormal(normals[nidxv1]);
				submeshes[currentSubmesh]._vertices[v2 - (uint16_t)toSubtract].setNormal(normals[nidxv2]);
				submeshes[currentSubmesh]._vertices[v3 - (uint16_t)toSubtract].setNormal(normals[nidxv3]);

				submeshes[currentSubmesh].addTriangle(trg);
				needSubmesh = true;
				
			}

			// hehe, thats it for that bit, now to write it all


		}
		vector<byte> mesh;
		mesh.push_back(0x6D);
	}
	catch (exception) {

	}
}
