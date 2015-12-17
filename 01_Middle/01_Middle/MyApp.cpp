#include "MyApp.h"

#include "MyApp.h"
#include "GLUtils.hpp"

#include <GL/GLU.h>
#include <math.h>

#include "ObjParser_OGL3.h"
#include "GeometryFactory.h"
#include "CubeSize.h"
#include "Parameters.h"
#include <algorithm>

CMyApp::CMyApp(void) :
	defaultActiveCubePos(Position(0, 0, 10)), reflectorSize(CubeSize(1, 1, 6)), driverSize(CubeSize(3, 2, 10)), wheelSize(CubeSize(3, 1, 8))
{
	basePlainTextureID = TextureFromFile("LEGO_logo.jpg");
}

CMyApp::~CMyApp(void)
{
}

bool CMyApp::Init()
{
	// Init components.
	InitTextures();
	InitCubePrefabs();
	InitCubeZPuffer();

	floor = GeometryFactory::GetLegoCube(floorSize, floorSize, CubeHeight::THIN);
	floor->initBuffers();
	floorTextureID = cubeColorTextures[CubeColor::GRAY];

	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	glEnable(GL_CULL_FACE);		// kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST);	// mélységi teszt bekapcsolása (takarás)

								//
								// geometria letrehozasa
								//
	m_vb.AddAttribute(0, 3);
	m_vb.AddAttribute(1, 3);
	m_vb.AddAttribute(2, 2);

	m_vb.AddData(0, -15, 0, -15);
	m_vb.AddData(0, 15, 0, -15);
	m_vb.AddData(0, -15, 0, 15);
	m_vb.AddData(0, 15, 0, 15);

	m_vb.AddData(1, 0, 1, 0);
	m_vb.AddData(1, 0, 1, 0);
	m_vb.AddData(1, 0, 1, 0);
	m_vb.AddData(1, 0, 1, 0);

	m_vb.AddData(2, 0, 0);
	m_vb.AddData(2, 1, 0);
	m_vb.AddData(2, 0, 1);
	m_vb.AddData(2, 1, 1);

	m_vb.AddIndex(1, 0, 2);
	m_vb.AddIndex(1, 2, 3);

	m_vb.InitBuffers();

	//
	// shaderek betöltése
	//
	m_program.AttachShader(GL_VERTEX_SHADER, "dirLight.vert");
	m_program.AttachShader(GL_FRAGMENT_SHADER, "dirLight.frag");

	m_program.BindAttribLoc(0, "vs_in_pos");
	m_program.BindAttribLoc(1, "vs_in_normal");
	m_program.BindAttribLoc(2, "vs_in_tex0");

	if (!m_program.LinkProgram())
	{
		return false;
	}

	//
	// egyéb inicializálás
	//

	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);

	return true;
}

void CMyApp::Clean()
{
	glDeleteTextures(1, &basePlainTextureID);
}

void CMyApp::Update()
{
	static Uint32 last_time = SDL_GetTicks();
	float delta_time = (SDL_GetTicks() - last_time) / 1000.0f;

	m_camera.Update(delta_time);

	last_time = SDL_GetTicks();
}

void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawBasePlain();
	DrawFloor();
	DrawAllCubes();
	DrawCube(activeCube);
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent &key)
{
	m_camera.KeyboardDown(key);

	switch (key.keysym.sym)
	{
	case SDLK_KP_0:
		PutDownActiveCube();
		break;
	case SDLK_KP_1: // unused
		break;
	case SDLK_KP_2:
		++activeCube->position.row;
		break;
	case SDLK_KP_3:
		if (activeCube->position.height - 1 > cubeZPuffer[activeCube->position.row + halfFloorSize][activeCube->position.col + halfFloorSize])
		{
			--activeCube->position.height;
		}
		break;
	case SDLK_KP_4:
		--activeCube->position.col;
		break;
	case SDLK_KP_5:
		activeCube->rotation = fmod(activeCube->rotation + 90.0f, 360.0f);
		break;
	case SDLK_KP_6:
		++activeCube->position.col;
		break;
	case SDLK_KP_7: // unused
		break;
	case SDLK_KP_8:
		--activeCube->position.row;
		break;
	case SDLK_KP_9:
		++activeCube->position.height;
		break;
	case SDLK_KP_PLUS:
		++activeCube->mesh;
		if (activeCube->mesh == cubePrefabs.end())
		{
			activeCube->mesh = cubePrefabs.begin();
		}
		break;
	case SDLK_KP_MINUS:
		activeCube->mesh = (activeCube->mesh == cubePrefabs.begin()) ? std::prev(cubePrefabs.end()) : std::prev(activeCube->mesh);
		break;
	case SDLK_KP_MULTIPLY:
		++activeCube->color;
		if (activeCube->color == cubeColorTextures.end())
		{
			activeCube->color = cubeColorTextures.begin();
		}
		break;
	case SDLK_KP_ENTER: // unused
		break;
	case SDLK_BACKSPACE:
		if (cubes.size() > 0)
		{
			cubes.pop_back();
		}
	default:
		break;
	}
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent &key)
{
	m_camera.KeyboardUp(key);
}

void CMyApp::MouseMove(SDL_MouseMotionEvent &mouse)
{
	m_camera.MouseMove(mouse);
}

void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

	m_camera.Resize(_w, _h);
}

void CMyApp::DrawBasePlain()
{
	m_program.On();

	glm::mat4 matWorld = glm::mat4(1.0f);
	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
	glm::mat4 mvp = m_camera.GetViewProj() *matWorld;

	m_program.SetUniform("world", matWorld);
	m_program.SetUniform("worldIT", matWorldIT);
	m_program.SetUniform("MVP", mvp);
	m_program.SetUniform("eye_pos", m_camera.GetEye());

	m_program.SetTexture("texImage", 0, basePlainTextureID);

	// kapcsoljuk be a VAO-t (a VBO jön vele együtt)
	m_vb.On();

	m_vb.DrawIndexed(GL_TRIANGLES, 0, 6, 0);

	m_vb.Off();

	// shader kikapcsolasa
	m_program.Off();
}

void CMyApp::DrawFloor()
{
	m_program.On();

	glm::mat4 matWorld =
		glm::scale<float>(0.1f, 0.1f, 0.1f) *
		glm::translate<float>(floorSize / 2 * -GeometryFactory::cubeWidthUnit, 0.0f, floorSize / 2 * -GeometryFactory::cubeWidthUnit);

	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
	glm::mat4 mvp = m_camera.GetViewProj() *matWorld;

	m_program.SetUniform("world", matWorld);
	m_program.SetUniform("worldIT", matWorldIT);
	m_program.SetUniform("MVP", mvp);
	m_program.SetUniform("eye_pos", m_camera.GetEye());

	m_program.SetTexture("texImage", 0, floorTextureID);

	floor->draw();

	// shader kikapcsolasa
	m_program.Off();
}

void CMyApp::DrawCube(std::shared_ptr<Cube> cube)
{
	m_program.On();

	glm::mat4 matWorld =
		glm::scale<float>(0.1f, 0.1f, 0.1f) *
		glm::translate<float>(cube->position.col * GeometryFactory::cubeWidthUnit, cube->position.height * GeometryFactory::thinCubeHeightUnit, cube->position.row * GeometryFactory::cubeWidthUnit) *
		GetCubeRotationMatrix(cube);

	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
	glm::mat4 mvp = m_camera.GetViewProj() *matWorld;

	m_program.SetUniform("world", matWorld);
	m_program.SetUniform("worldIT", matWorldIT);
	m_program.SetUniform("MVP", mvp);
	m_program.SetUniform("eye_pos", m_camera.GetEye());

	m_program.SetTexture("texImage", 0, cube->color->second);

	cubePrefabs[cube->mesh->first]->draw();

	// shader kikapcsolasa
	m_program.Off();
}

void CMyApp::DrawAllCubes()
{
	std::for_each(cubes.begin(), cubes.end(), [this](auto cube)
	{
		DrawCube(cube);
	});
}

void CMyApp::InitCubePrefabs()
{
	// Add lego cubes.
	for (int r = 1; r <= 2; ++r)
	{
		for (int c = 1; c <= 8; ++c)
		{
			if (r == 2 && c == 1) continue;

			CubeSize thinCube(r, c, (int)CubeHeight::THIN);
			CubeSize normalCube(r, c, (int)CubeHeight::NORMAL);

			cubePrefabs[thinCube] = GeometryFactory::GetLegoCube(r, c, CubeHeight::THIN);
			cubePrefabs[thinCube]->initBuffers();

			cubePrefabs[normalCube] = GeometryFactory::GetLegoCube(r, c, CubeHeight::NORMAL);
			cubePrefabs[normalCube]->initBuffers();
		}
	}

	// Add special cubes.
	cubePrefabs[reflectorSize] = GeometryFactory::GetReflector();
	cubePrefabs[reflectorSize]->initBuffers();

	cubePrefabs[driverSize] = GeometryFactory::GetDriver();
	cubePrefabs[driverSize]->initBuffers();

	cubePrefabs[wheelSize] = GeometryFactory::GetWheel();
	cubePrefabs[wheelSize]->initBuffers();

	// activeCube = std::make_shared<Cube>(defaultActiveCubePos, cubeColorTextures.begin(), 0.0f, cubePrefabs.begin());
	activeCube = std::make_shared<Cube>(defaultActiveCubePos, cubeColorTextures.begin(), 0.0f, cubePrefabs.begin());
}

void CMyApp::InitTextures()
{
	cubeColorTextures[CubeColor::RED] = TextureFromFile("red.png");
	cubeColorTextures[CubeColor::GREEN] = TextureFromFile("green.png");
	cubeColorTextures[CubeColor::BLUE] = TextureFromFile("blue.png");
	cubeColorTextures[CubeColor::YELLOW] = TextureFromFile("yellow.png");
	cubeColorTextures[CubeColor::GRAY] = TextureFromFile("gray.png");
	cubeColorTextures[CubeColor::WHITE] = TextureFromFile("white.png");
	cubeColorTextures[CubeColor::BLACK] = TextureFromFile("black.png");
	cubeColorTextures[CubeColor::BROWN] = TextureFromFile("brown.png");
	cubeColorTextures[CubeColor::PEACH] = TextureFromFile("peach.png");
}

void CMyApp::DeleteTextures()
{
	std::for_each(cubeColorTextures.begin(), cubeColorTextures.end(), [](auto kvpair)
	{
		glDeleteTextures(1, &kvpair.second);
	});
}

void CMyApp::InitCubeZPuffer()
{
	for (int r = 0; r < floorSize; ++r)
	{
		for (int c = 0; c < floorSize; ++c)
		{
			cubeZPuffer[r][c] = 0;
		}
	}
}

glm::mat4 CMyApp::GetCubeRotationMatrix(std::shared_ptr<Cube> cube)
{
	auto rot_tr = 
		glm::rotate<float>(cube->rotation, 0.0f, 1.0f, 0.0f) *
		glm::translate<float>(cube->mesh->first.cols * -GeometryFactory::cubeWidthUnit / 2.0, 0.0f, cube->mesh->first.rows * -GeometryFactory::cubeWidthUnit / 2.0f);

	if (fmod(cube->rotation, 180.f) == 0.0f)
	{
		return
			glm::translate<float>(cube->mesh->first.cols * GeometryFactory::cubeWidthUnit / 2.0, 0.0f, cube->mesh->first.rows * GeometryFactory::cubeWidthUnit / 2.0f) *
			rot_tr;
	}

	return
		glm::translate<float>(cube->mesh->first.rows * GeometryFactory::cubeWidthUnit / 2.0, 0.0f, cube->mesh->first.cols * GeometryFactory::cubeWidthUnit / 2.0f) *
		rot_tr;
}

void CMyApp::PutDownActiveCube()
{
	int cubeRowNum, cubeColNum;

	// Determine row and column nums based on rotation of the active cube.
	if (fmod(activeCube->rotation, 180.0f) == 0.0)
	{
		cubeRowNum = activeCube->mesh->first.rows; cubeColNum = activeCube->mesh->first.cols;
	}
	else
	{
		cubeRowNum = activeCube->mesh->first.cols; cubeColNum = activeCube->mesh->first.rows;
	}

	// Check whether the active cube is above the base panel.
	if (!(activeCube->position.row + cubeRowNum <= halfFloorSize && activeCube->position.col + cubeColNum <= halfFloorSize))
	{
		return;
	}

	// Find maximal Z value in the puffer below the active cube.
	int zPufferRowInd, zPufferColInd;
	int maxHeight = cubeZPuffer[activeCube->position.row + halfFloorSize][activeCube->position.col + halfFloorSize];
	
	for (int r = activeCube->position.row; r < activeCube->position.row + cubeRowNum; ++r)
	{
		for (int c = activeCube->position.col; c < activeCube->position.col + cubeColNum; ++c)
		{
			zPufferRowInd = r + halfFloorSize; zPufferColInd = c + halfFloorSize;
			int currentZ = cubeZPuffer[zPufferRowInd][zPufferColInd];
			
			if (currentZ >= activeCube->position.height) return;

			if (currentZ > maxHeight) maxHeight = currentZ;
		}
	}

	// Update Z puffer with dimensions of the active cube.
	std::cout << "Putting down active cube to height " << maxHeight + 1 << std::endl;

	for (int r = activeCube->position.row; r < activeCube->position.row + cubeRowNum; ++r)
	{
		for (int c = activeCube->position.col; c < activeCube->position.col + cubeColNum; ++c)
		{
			zPufferRowInd = r + halfFloorSize; zPufferColInd = c + halfFloorSize;
			cubeZPuffer[zPufferRowInd][zPufferColInd] += (int)activeCube->mesh->first.height;
		}
	}

	// Store active cube and get a new one.
	auto newActiveCube = std::make_shared<Cube>(defaultActiveCubePos, activeCube->color, 0.0f, activeCube->mesh);
	activeCube->position.height = maxHeight + 1;
	cubes.push_back(activeCube);
	activeCube = newActiveCube;
}

/*void CMyApp::InitSpecialCubePrefabs()
{
	auto outerCylinder = GeometryFactory::GetCylinder(
		glm::vec3(0.0f, GeometryFactory::cubeWidthUnit, GeometryFactory::cubeWidthUnit),
		glm::vec3(1.0f, 0.0f, 0.0f),
		GeometryFactory::cubeWidthUnit, GeometryFactory::cubeWidthUnit);
	outerCylinder->merge(GeometryFactory::GetCircle(
		glm::vec3(0.0f, GeometryFactory::cubeWidthUnit, GeometryFactory::cubeWidthUnit),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		GeometryFactory::cubeWidthUnit).get());
	outerCylinder->merge(GeometryFactory::GetCircle(
		glm::vec3(GeometryFactory::cubeWidthUnit, GeometryFactory::cubeWidthUnit, GeometryFactory::cubeWidthUnit),
		glm::vec3(1.0f, 0.0f, 0.0f),
		GeometryFactory::cubeWidthUnit).get());

	auto innerCylinder = GeometryFactory::GetCylinder(
		glm::vec3(GeometryFactory::cubeWidthUnit, GeometryFactory::cubeWidthUnit, GeometryFactory::cubeWidthUnit),
		glm::vec3(1.0f, 0.0f, 0.0f),
		0.5f * GeometryFactory::cubeWidthUnit, 0.5f * GeometryFactory::cubeWidthUnit);
	innerCylinder->merge(GeometryFactory::GetCircle(
		glm::vec3(GeometryFactory::cubeWidthUnit, GeometryFactory::cubeWidthUnit, GeometryFactory::cubeWidthUnit),
		glm::vec3(1.0f, 0.0f, 0.0f),
		0.5f * GeometryFactory::cubeWidthUnit).get());
	
	outerCylinder->initBuffers();
	innerCylinder->initBuffers();

	specialCubePrefabs[CubeSize(1, 2, 2)] = outerCylinder;
	specialCubePrefabs[CubeSize(1, 1, 1)] = innerCylinder;
}*/
