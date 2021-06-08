#pragma once
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <algorithm>
using namespace std;


struct Vertex
{
	GLfloat x, y, z;
};

struct Texture
{
	GLfloat x, y;
};

struct Normal
{
	GLfloat x, y, z;
};

struct Material
{
	GLfloat ambient[4] = {0.2, 0.2, 0.2, 1.0};
	GLfloat diffuse[4] = {0.8, 0.8, 0.8, 1.0};
	GLfloat specular[4] = {0.0, 0.0, 0.0, 1.0};
	GLfloat shininess = 65.0;
};

struct Face
{
	GLint a, b, c;		// 顶点下标
	GLint ta, tb, tc;	// 顶点纹理下标
	GLint na, nb, nc;	// 顶点法线下标
	string mtl_name;	// 材质名
};



class OBJImporter
{
public:
	OBJImporter(string path)
		: path(path)
	{
		ifstream ifs;
		ifs.open(path, ifstream::in);
		if (!ifs.is_open()) {
			cerr << "Can't open file.\n";
			return;
		}
		parseStream(ifs);
		ifs.close();
		smoothNormals();
	}
	~OBJImporter() = default;

	string path;
	vector<Vertex> vertexList;
	vector<Texture> textureList;
	vector<Normal> normalList;
	vector<Face> faceList;
	unordered_map<string, Material> materialTable;

private:
	void parseStream(std::ifstream& ifs) {
		string lineBuffer;
		string currentMaterialName;
		while (getline(ifs, lineBuffer).good())
		{
			if (lineBuffer.empty()) continue;
			switch (lineBuffer[0])
			{
			case '#':	// comment
				break;
			case 'm':	// mtllib
				readMaterial(lineBuffer);
				break;
			case 'u':	// usemtl
				useMaterial(lineBuffer, currentMaterialName);
				break;
			case 'v':	// v, vt, vn
				readVertex(lineBuffer);
				break;
			case 'f':	// face
				readFace(lineBuffer, currentMaterialName);
				break;
			default:	// 未支持 group
				break;
			}
		}
	}
	void readMaterial(string& lineBuffer);
	void useMaterial(string& lineBuffer, string& currentMaterialName);
	void readVertex(string& lineBuffer);
	void readFace(string& lineBuffer, string& currentMaterialName);
	void smoothNormals();
};
