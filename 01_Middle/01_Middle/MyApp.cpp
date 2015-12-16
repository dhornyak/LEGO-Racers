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

CMyApp::CMyApp(void)
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
		break;
	case SDLK_KP_1:
		break;
	case SDLK_KP_2:
		break;
	case SDLK_KP_3:
		if (activeCube->position.height > 1)
		{
			--activeCube->position.height;
		}
		break;
	case SDLK_KP_4:
		break;
	case SDLK_KP_5:
		activeCube->rotation = fmod(activeCube->rotation + 90.0f, 360.0f);
		break;
	case SDLK_KP_6:
		break;
	case SDLK_KP_7:
		break;
	case SDLK_KP_8:
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
	case SDLK_KP_ENTER:
		break;
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
	for (int r = 1; r <= 2; ++r)
	{
		for (int c = 1; c <= 8; ++c)
		{
			if (r == 2 && c == 1) continue;

			CubeSize thinCube(r, c, CubeHeight::THIN);
			CubeSize normalCube(r, c, CubeHeight::NORMAL);

			cubePrefabs[thinCube] = GeometryFactory::GetLegoCube(r, c, CubeHeight::THIN);
			cubePrefabs[thinCube]->initBuffers();

			cubePrefabs[normalCube] = GeometryFactory::GetLegoCube(r, c, CubeHeight::NORMAL);
			cubePrefabs[normalCube]->initBuffers();
		}
	}

	activeCube = std::make_shared<Cube>(Position(0, 0, 1), cubeColorTextures.begin(), 0.0f, cubePrefabs.begin());
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
