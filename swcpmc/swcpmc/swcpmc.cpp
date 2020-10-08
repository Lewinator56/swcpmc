// swcpmc.cpp : Defines the entry point for the application.
//

#include "swcpmc.h"
#include <array>
#include <sstream>
#include <fstream>

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
template <typename T>
vector<byte> getBytes(T in) {
	byte* bytePointer = reinterpret_cast<byte*>(&in);
	return vector<byte>(bytePointer, bytePointer + sizeof(T));
}


void objToMesh(vector<string> data) {
	string error;
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
		cout << data.size() << endl;
		for (int i = 0; i < data.size(); i++) {
			try {
				error = data[i].substr(0, data[i].find_first_of(" "));
			}
			catch (exception e) {
				cout << "exception" << endl;
			}

			if (data[i].rfind('V', 0) == 0 && needSubmesh == true) {
				SubMesh s;
				s._r = c._r;
				s._g = c._g;
				s._b = c._b;
				s._a = c._a;
				currentSubmesh++;
				submeshes.push_back(s);
				subMeshVertices.push_back(0);
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

				if (needSubmesh == true) {
					SubMesh s;
					s._r = c._r;
					s._g = c._g;
					s._b = c._b;
					s._a = c._a;
					currentSubmesh++;
					submeshes.push_back(s);
					subMeshVertices.push_back(0);
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
				float vx = stof(v[1]);
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
		vector<byte> mesh = {};
		// write header
		vector<byte> toappend = { 0x6D, 0x65, 0x73, 0x68, 0x07, 0x00, 0x01, 0x00 };
		mesh.insert(mesh.end(), toappend.begin(), toappend.end());
		// write vertex cound
		toappend = getBytes(vertexCount);
		mesh.insert(mesh.end(), toappend.begin(), toappend.end());

		toappend = { 0x13, 0x00, 0x00, 0x00 };
		mesh.insert(mesh.end(), toappend.begin(), toappend.end());

		uint32_t tc = 0;
		error = "vtxDefWrie";
		for each (SubMesh sm in submeshes)
		{

			for each (Vertex v in sm._vertices) {
				vector<byte> vtxDef;
				toappend = getBytes(v._px);
				vtxDef.insert(vtxDef.end(), toappend.begin(), toappend.end());
				toappend = getBytes(v._py);
				vtxDef.insert(vtxDef.end(), toappend.begin(), toappend.end());
				toappend = getBytes(v._pz);
				vtxDef.insert(vtxDef.end(), toappend.begin(), toappend.end());

				vtxDef.push_back(sm._r);
				vtxDef.push_back(sm._g);
				vtxDef.push_back(sm._b);
				vtxDef.push_back(sm._a);

				toappend = getBytes(v._n._x);
				vtxDef.insert(vtxDef.end(), toappend.begin(), toappend.end());
				toappend = getBytes(v._n._y);
				vtxDef.insert(vtxDef.end(), toappend.begin(), toappend.end());
				toappend = getBytes(v._n._z);
				vtxDef.insert(vtxDef.end(), toappend.begin(), toappend.end());

				mesh.insert(mesh.end(), vtxDef.begin(), vtxDef.end());

			}
			tc += (uint32_t) sm._triangles.size();
		}

		toappend = getBytes(tc * 3);
		mesh.insert(mesh.end(), toappend.begin(), toappend.end());

		error = "faceWrite";
		for each (SubMesh sm in submeshes) {
			for each (Triangle t in sm._triangles) {
				vector<byte> tDef = {};
				toappend = getBytes((uint16_t)t._v1);
				tDef.insert(tDef.end(), toappend.begin(), toappend.end());
				toappend = getBytes((uint16_t)t._v3);
				tDef.insert(tDef.end(), toappend.begin(), toappend.end());
				toappend = getBytes((uint16_t)t._v2);
				cout << (uint16_t)t._v2 << endl;
				cout << toappend.size() << endl;
				
				tDef.insert(tDef.end(), toappend.begin(), toappend.end());
				
				mesh.insert(mesh.end(), tDef.begin(), tDef.end());

			}
		}
		toappend = getBytes((uint16_t)submeshes.size());
		mesh.insert(mesh.end(), toappend.begin(), toappend.end());
		uint32_t subMeshPosition = 0;
		error = "subWrite";

		for each (SubMesh sm in submeshes) {
			toappend = getBytes(subMeshPosition);
			mesh.insert(mesh.end(), toappend.begin(), toappend.end());
			subMeshPosition += (uint32_t)sm._triangles.size() * 3;
			toappend = getBytes((uint32_t)sm._triangles.size() * 3);
			mesh.insert(mesh.end(), toappend.begin(), toappend.end());
			toappend = { 0x00, 0x00 };
			mesh.insert(mesh.end(), toappend.begin(), toappend.end());
			toappend = getBytes(sm._shader);
			mesh.insert(mesh.end(), toappend.begin(), toappend.end());
			for each (float f in sm._cullingMin) {
				toappend = getBytes(f);
				mesh.insert(mesh.end(), toappend.begin(), toappend.end());
			}
			for each (float f in sm._cullingMax) {
				toappend = getBytes(f);
				mesh.insert(mesh.end(), toappend.begin(), toappend.end());
			}
			toappend = { 0x00, 0x00, 0x03, 0x00, 0x49, 0x44, 0x33, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F };
			mesh.insert(mesh.end(), toappend.begin(), toappend.end());
		}
		toappend = { 0x00, 0x00 };
		mesh.insert(mesh.end(), toappend.begin(), toappend.end());;


		// Stupid workaround to cater for microsoft's idiocy, and inability to follow unix standards
		ofstream fs;
		fs.open(_pathToOutput, ios_base::binary | ios_base::out );
		fs.write((char*)&mesh[0], mesh.size());
		
		fs.close();
	}
	catch (exception e) {
		cout << "Could not convert obejct, error code: " << error << endl;
	}
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
		ifstream ifs;
		string line;
		vector<string> bin;
		ifs.open(_pathToMesh);
		while (getline(ifs, line)) {
			bin.push_back(line);
		}
		objToMesh(bin);
		
	}

	
	

	// main bit of code

}




