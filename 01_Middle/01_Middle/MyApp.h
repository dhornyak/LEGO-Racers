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
#include "TrackSection.h"

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
	void UpdateCubeZPuffer(int topLeftRow, int topLeftCol, int rowNum, int colNum, int valueToAdd);
	void PrintCubeZPuffer();

	glm::mat4 GetCubeRotationMatrix(std::shared_ptr<Cube> cube);
	void PutDownActiveCube();

	// Special cube identifiers.
	CubeSize reflectorSize, driverSize, wheelSize, chassisSize;
	const int chassisHeight = 5;

	// Initial vehicle parts.
	std::shared_ptr<Mesh> wheelMesh = nullptr;
	std::shared_ptr<Mesh> chassisMesh = nullptr;
	void InitInitialiVehicleParts();
	void DrawInitialVehicleParts();

	// Scene informations.
	Scene currentScene;

	// Track components.
	Track track;
	std::vector<std::shared_ptr<Mesh>> trackSectionMeshes;
	
	GLuint asphaltTextureID;

	void AssembleTrack();
	void DrawTrack();

	// Racing components.
	float speed;
	bool raceStarted;
	glm::vec3 carPosition;
	float driveDirection;
};

