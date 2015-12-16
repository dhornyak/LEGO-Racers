#pragma once

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL.h>
#include <SDL_opengl.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

#include "gCamera.h"
#include "gShaderProgram.h"
#include "gVertexBuffer.h"
#include "Mesh_OGL3.h"
#include "ActiveCube.h"
#include "CubeSize.h"

#include <memory>
#include <map>

class CMyApp
{
public:
	CMyApp(void);
	~CMyApp(void);

	bool Init();
	void Clean();

	void Update();
	void Render();

	void KeyboardDown(SDL_KeyboardEvent&);
	void KeyboardUp(SDL_KeyboardEvent&);
	void MouseMove(SDL_MouseMotionEvent&);
	void MouseDown(SDL_MouseButtonEvent&) { };
	void MouseUp(SDL_MouseButtonEvent&) { };
	void MouseWheel(SDL_MouseWheelEvent&) { };
	void Resize(int, int);

protected:
	gCamera			m_camera;
	gShaderProgram	m_program;
	gVertexBuffer	m_vb;

	static const int floorSize = 26;
	static const int halfFloorSize = floorSize / 2;

	std::shared_ptr<Mesh> floor = nullptr;
	GLuint floorTextureID;
	GLuint basePlainTextureID;
	void DrawBasePlain();
	void DrawFloor();

	std::shared_ptr<Cube> activeCube;
	Position defaultActiveCubePos;

	std::vector<std::shared_ptr<Cube>> cubes;
	void DrawCube(std::shared_ptr<Cube> cube);
	void DrawAllCubes();

	std::map<CubeSize, std::shared_ptr<Mesh>> cubePrefabs;
	void InitCubePrefabs();

	std::map<CubeColor, GLuint> cubeColorTextures;
	void InitTextures();
	void DeleteTextures();

	int cubeZPuffer[floorSize][floorSize];
	void InitCubeZPuffer();

	glm::mat4 GetCubeRotationMatrix(std::shared_ptr<Cube> cube);
	void PutDownActiveCube();
};

