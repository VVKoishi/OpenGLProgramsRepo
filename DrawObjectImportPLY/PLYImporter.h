#pragma once
#include <GL/glut.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>


struct Vertex
{
	GLfloat x, y, z, r, g, b;
};

struct Face
{
	GLint a, b, c;
};

class PLYImporter
{
public:
	PLYImporter(const char* filename) {
		std::ifstream ifs;
		ifs.open(filename, std::ifstream::in);
		if (!ifs.is_open()) {
			std::cout << "open wrong" << std::endl;
			return;
		}
		parseStream(ifs);
		ifs.close();
	}
	~PLYImporter() {
		delete[] vertexPointer;
		delete[] facePointer;
	}
	Vertex* GetVertexPointer() {
		return vertexPointer;
	}
	Face* GetFacePointer()
	{
		return facePointer;
	}
	GLint GetVertexNum() {
		return vertexNum;
	}
	GLint GetFaceNum() {
		return faceNum;
	}
private:
	Vertex* vertexPointer;	// 指向顶点表
	Face* facePointer;		// 指向面片表
	GLint vertexNum = 0;
	GLint faceNum = 0;

	void parseStream(std::ifstream& ifs) {
		char lineBuffer[256];
		// parse header
		while (ifs.getline(lineBuffer, 256).good())
		{
			int start = 0;
			int end = 0;
			if (getToken(lineBuffer, 256, start, end))
			{
				if (matchToken(lineBuffer, start, end, "element") 
					&& getToken(lineBuffer, 256, start, end))
				{
					// match element vertex xxx
					if (matchToken(lineBuffer, start, end, "vertex") 
						&& getToken(lineBuffer, 256, start, end))
					{
						// set xxx
						vertexNum = tokenToInt(lineBuffer, start, end);

						// 分配空间
						vertexPointer = new Vertex[vertexNum];
					}

					// match element face yyy
					if (matchToken(lineBuffer, start, end, "face") 
						&& getToken(lineBuffer, 256, start, end))
					{
						// set yyy
						faceNum = tokenToInt(lineBuffer, start, end);

						// 分配空间
						facePointer = new Face[faceNum];
					}
				}

				if (matchToken(lineBuffer, start, end, "end_header"))
				{
					break;
				}
			}
		}
		// parse body
		int vertexCount = 0;
		int faceCount = 0;
		while (ifs.getline(lineBuffer, 256).good())
		{
			int start = 0;
			int end = 0;
			if (vertexCount < vertexNum)
			{
				getToken(lineBuffer, 256, start, end);
				vertexPointer[vertexCount].x = tokenToFloat(lineBuffer, start, end);
				getToken(lineBuffer, 256, start, end);
				vertexPointer[vertexCount].y = tokenToFloat(lineBuffer, start, end);
				getToken(lineBuffer, 256, start, end);
				vertexPointer[vertexCount].z = tokenToFloat(lineBuffer, start, end);
				//getToken(lineBuffer, 256, start, end);
				getToken(lineBuffer, 256, start, end);
				//vertexPointer[vertexCount].r = tokenToFloat(lineBuffer, start, end);
				vertexPointer[vertexCount].r = 1;
				vertexPointer[vertexCount].g = vertexPointer[vertexCount].r;
				vertexPointer[vertexCount].b = vertexPointer[vertexCount].r;
				vertexCount++;
			} 
			else if (faceCount < faceNum)
			{
				getToken(lineBuffer, 256, start, end);	// 去掉开头的3
				getToken(lineBuffer, 256, start, end);
				facePointer[faceCount].a = tokenToInt(lineBuffer, start, end);
				getToken(lineBuffer, 256, start, end);
				facePointer[faceCount].b = tokenToInt(lineBuffer, start, end);
				getToken(lineBuffer, 256, start, end);
				facePointer[faceCount].c = tokenToInt(lineBuffer, start, end);
				faceCount++;
			}
		}
	}
	bool getToken(const char* lineBuffer, int length, int& start, int& end) {
		if (lineBuffer[end] == '\0') return false;
		for (int i = end; lineBuffer[i] <= length; i++)
		{
			if (lineBuffer[i] != ' ')
			{
				if (lineBuffer[i] == '\0' || lineBuffer[i] == EOF)
				{
					return false;
				}
				else
				{
					start = i;
					break;
				}
			}
		}
		for (int i = start; lineBuffer[i] <= length; i++)
		{
			if (lineBuffer[i] == ' ' || lineBuffer[i] == '\0' || lineBuffer[i] == EOF)
			{
				end = i;
				break;
			}
		}
		return true;
	}
	bool matchToken(const char* lineBuffer, int start, int end, const char* keyword) {
		return memcmp(lineBuffer + start, keyword, end - start) == 0;
	}
	int tokenToInt(const char* lineBuffer, int start, int end) {
		char token[32];
		memcpy(token, lineBuffer + start, end - start);
		return atoi(token);
	}
	double tokenToFloat(const char* lineBuffer, int start, int end) {
		char token[32];
		memcpy(token, lineBuffer + start, end - start);
		return atof(token);
	}
};