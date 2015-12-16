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

	m_vb.AddData(0, -25, 0, -25);
	m_vb.AddData(0, 25, 0, -25);
	m_vb.AddData(0, -25, 0, 25);
	m_vb.AddData(0, 25, 0, 25);

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
	switch (key.keysym.sym)
	{
	case SDLK_KP_0:
		break;
	case SDLK_KP_1:
		break;
	case SDLK_KP_2:
		break;
	case SDLK_KP_3:
		break;
	case SDLK_KP_4:
		break;
	case SDLK_KP_5:
		break;
	case SDLK_KP_6:
		break;
	case SDLK_KP_7:
		break;
	case SDLK_KP_8:
		break;
	case SDLK_KP_9:
		break;
	case SDLK_KP_PLUS:
		break;
	case SDLK_KP_MINUS:
		break;
	case SDLK_KP_MULTIPLY:
		break;
	case SDLK_KP_ENTER:
		break;
	default:
		break;
	}
}

void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);

	m_camera.Resize(_w, _h);
}

void CMyApp::DrawBasePlain()
{
}

void CMyApp::DrawFloor()
{
}

void CMyApp::DrawCube(std::shared_ptr<Cube> cube)
{
}

void CMyApp::DrawAllCubes()
{
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

	activeCube = std::make_shared<Cube>(Position(0, 0, 1), cubeColorTextures.begin()->first, 0.0f, cubePrefabs.begin());
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
