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
#include <stdlib.h>

CMyApp::CMyApp(void) :
	defaultActiveCubePos(Position(0, 0, 10)), reflectorSize(CubeSize(1, 1, 6)), driverSize(CubeSize(3, 2, 10)), wheelSize(CubeSize(3, 1, 8)), chassisSize(CubeSize(10, 6, (int)CubeHeight::THIN)), speed(10.0f)
{
	basePlainTextureID = TextureFromFile("LEGO_logo.jpg");
	asphaltTextureID = TextureFromFile("asphalt.jpg");
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
	InitInitialiVehicleParts();
	PrintCubeZPuffer();

	currentScene = Scene::EDITING;

	// Create track.
	AssembleTrack();
	raceStarted = false;

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

	if (currentScene == Scene::RACING && raceStarted)
	{
		// Update camera and car position/direction.
		carPosition = track.GetPosition(speed);
		driveDirection = track.GetDriveDirection();
	}
}

void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (currentScene)
	{
	case Scene::EDITING:
		DrawBasePlain();
		DrawFloor();
		DrawInitialVehicleParts();
		DrawAllCubes();
		DrawCube(activeCube);
		break;
	case Scene::RACING:
		DrawInitialVehicleParts();
		// DrawAllCubes();
		DrawTrack();
		break;
	case Scene::FINISH:
		break;
	default:
		break;
	}
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
	case SDLK_KP_ENTER:
		switch (currentScene)
		{
		case Scene::EDITING: currentScene = Scene::RACING;
			break;
		case Scene::RACING: currentScene = Scene::EDITING;
			break;
		case Scene::FINISH: currentScene = Scene::EDITING;
			break;
		default:
			break;
		}
		break;
	case SDLK_BACKSPACE:
		if (cubes.size() > 0)
		{
			cubes.pop_back();
		}
	case SDLK_UP:
		if (currentScene == Scene::RACING)
		{
			if (!raceStarted)
			{
				raceStarted = true;
				track.InitTrack();
			}
			else
			{
				speed += 2.0f;
				std::cout << "speed: " << speed << std::endl;
			}
		}
		break;
	case SDLK_DOWN:
		if (currentScene == Scene::RACING && raceStarted)
		{
			speed -= 2.0f;
			if (speed < 0.0f) speed = 0.0f;
			std::cout << "speed: " << speed << std::endl;
		}
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

void CMyApp::UpdateCubeZPuffer(int topLeftRow, int topLeftCol, int rowNum, int colNum, int valueToAdd)
{
	for (int r = topLeftRow; r < topLeftRow + rowNum; ++r)
	{
		for (int c = topLeftCol; c < topLeftCol + colNum; ++c)
		{
			cubeZPuffer[r][c] += valueToAdd;
		}
	}
}

void CMyApp::PrintCubeZPuffer()
{
	system("cls");

	for (int r = 0; r < floorSize; ++r)
	{
		for (int c = 0; c < floorSize; ++c)
		{
			if (cubeZPuffer[r][c] >= 0) std::cout << " " << cubeZPuffer[r][c] << " ";
			else std::cout << "-" << abs(cubeZPuffer[r][c]) << " ";
		}
		std::cout << std::endl;
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
			
			if (currentZ >= activeCube->position.height || currentZ < 0) return;

			if (currentZ > maxHeight) maxHeight = currentZ;
		}
	}

	// Update Z puffer with dimensions of the active cube.
	// std::cout << "Putting down active cube to height " << maxHeight + 1 << std::endl;
	int valueToAdd = (int)activeCube->mesh->first.height;

	if (activeCube->mesh->first == wheelSize || activeCube->mesh->first == driverSize || activeCube->mesh->first == reflectorSize)
	{
		valueToAdd = -(maxHeight + 1);
	}

	UpdateCubeZPuffer(activeCube->position.row + halfFloorSize, activeCube->position.col + halfFloorSize, cubeRowNum, cubeColNum, valueToAdd);

	// Store active cube and get a new one.
	auto newActiveCube = std::make_shared<Cube>(defaultActiveCubePos, activeCube->color, 0.0f, activeCube->mesh);
	activeCube->position.height = maxHeight + 1;
	cubes.push_back(activeCube);
	activeCube = newActiveCube;

	// Print Z-puffer to console.
	PrintCubeZPuffer();
}

void CMyApp::InitInitialiVehicleParts()
{
	chassisMesh = GeometryFactory::GetLegoCube(chassisSize.rows, chassisSize.cols, (CubeHeight)chassisSize.height);
	chassisMesh->initBuffers();

	wheelMesh = GeometryFactory::GetWheel();
	wheelMesh->initBuffers();

	// Init Z-puffer for these elements.
	// chassis
	UpdateCubeZPuffer(halfFloorSize - chassisSize.rows / 2, halfFloorSize - chassisSize.cols / 2, chassisSize.rows, chassisSize.cols, chassisHeight);

	// wheels
	UpdateCubeZPuffer(halfFloorSize - chassisSize.rows / 2, halfFloorSize + chassisSize.cols / 2, wheelSize.rows, wheelSize.cols, -1);
	UpdateCubeZPuffer(halfFloorSize - chassisSize.rows / 2, halfFloorSize - chassisSize.cols / 2 - 1, wheelSize.rows, wheelSize.cols, -1);

	UpdateCubeZPuffer(halfFloorSize + chassisSize.rows / 2 - wheelSize.rows, halfFloorSize + chassisSize.cols / 2, wheelSize.rows, wheelSize.cols, -1);
	UpdateCubeZPuffer(halfFloorSize + chassisSize.rows / 2 - wheelSize.rows, halfFloorSize - chassisSize.cols / 2 - 1, wheelSize.rows, wheelSize.cols, -1);
}

void CMyApp::DrawInitialVehicleParts()
{
	// Draw chassis.
	m_program.On();

	glm::mat4 matWorld = glm::scale<float>(0.1f, 0.1f, 0.1f);
		
	if ((currentScene == Scene::RACING || currentScene == Scene::FINISH) && track.IsInitialized())
	{
		matWorld *= glm::translate<float>(carPosition) *
					glm::rotate<float>(driveDirection, 0, 1, 0);
	}

	matWorld *= glm::translate<float>(chassisSize.cols / 2.0f * -GeometryFactory::cubeWidthUnit, chassisHeight * GeometryFactory::thinCubeHeightUnit, chassisSize.rows / 2.0f * -GeometryFactory::cubeWidthUnit);

	glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
	glm::mat4 mvp = m_camera.GetViewProj() *matWorld;

	m_program.SetUniform("world", matWorld);
	m_program.SetUniform("worldIT", matWorldIT);
	m_program.SetUniform("MVP", mvp);
	m_program.SetUniform("eye_pos", m_camera.GetEye());

	m_program.SetTexture("texImage", 0, cubeColorTextures.find(CubeColor::BROWN)->second);

	chassisMesh->draw();

	// shader kikapcsolasa
	m_program.Off();

	// Draw four wheels.
	std::vector<std::pair<float, glm::vec3>> rot_trans = 
	{
		std::pair<float, glm::vec3>(0.0f, glm::vec3(3.0f * GeometryFactory::cubeWidthUnit, GeometryFactory::thinCubeHeightUnit, -5.0f * GeometryFactory::cubeWidthUnit)),
		std::pair<float, glm::vec3>(0.0f, glm::vec3(3.0f * GeometryFactory::cubeWidthUnit, GeometryFactory::thinCubeHeightUnit, 2.0f * GeometryFactory::cubeWidthUnit)),
		std::pair<float, glm::vec3>(180.0f, glm::vec3(-3.0f * GeometryFactory::cubeWidthUnit, GeometryFactory::thinCubeHeightUnit, -2.0f * GeometryFactory::cubeWidthUnit)),
		std::pair<float, glm::vec3>(180.0f, glm::vec3(-3.0f * GeometryFactory::cubeWidthUnit, GeometryFactory::thinCubeHeightUnit, 5.0f * GeometryFactory::cubeWidthUnit))
	};

	std::for_each(rot_trans.begin(), rot_trans.end(), [this](auto pair)
	{
		m_program.On();

		glm::mat4 matWorld =
			glm::scale<float>(0.1f, 0.1f, 0.1f);

		if ((currentScene == Scene::RACING || currentScene == Scene::FINISH) && track.IsInitialized())
		{
			matWorld *= glm::translate<float>(carPosition) *
						glm::rotate<float>(driveDirection, 0, 1, 0);
		}

		matWorld *= glm::translate<float>(pair.second) *
					glm::rotate<float>(pair.first, 0, 1, 0);

		glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
		glm::mat4 mvp = m_camera.GetViewProj() *matWorld;

		m_program.SetUniform("world", matWorld);
		m_program.SetUniform("worldIT", matWorldIT);
		m_program.SetUniform("MVP", mvp);
		m_program.SetUniform("eye_pos", m_camera.GetEye());

		m_program.SetTexture("texImage", 0, cubeColorTextures.find(CubeColor::BLACK)->second);

		wheelMesh->draw();

		// shader kikapcsolasa
		m_program.Off();
	});
}

void CMyApp::AssembleTrack()
{
	/*auto line1 = std::make_shared<Line>(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f * 5.0f * TrackSection::trackHalfWidth, 0.0f, 0.0f),
		Line::Orientation::HORIZONTAL, TrackSection::Direction::PLUS);
	track.sections.push_back(line1);
	trackSectionMeshes.push_back(line1->GetMesh());*/

	auto line2 = std::make_shared<Line>(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -2.0f * 1.0f * TrackSection::trackHalfWidth),
		Line::Orientation::VERTICAL, TrackSection::Direction::MINUS);
	track.sections.push_back(line2);
	trackSectionMeshes.push_back(line2->GetMesh());

	/*auto line3 = std::make_shared<Line>(
		glm::vec3(2.0f * 5.0f * TrackSection::trackHalfWidth, 0.0f, -2.0f * 4.0f * TrackSection::trackHalfWidth),
		glm::vec3(2.0f * 1.0f * TrackSection::trackHalfWidth, 0.0f, -2.0f * 4.0f * TrackSection::trackHalfWidth),
		Line::Orientation::HORIZONTAL, TrackSection::Direction::MINUS);
	track.sections.push_back(line3);
	trackSectionMeshes.push_back(line3->GetMesh());

	auto line4 = std::make_shared<Line>(
		glm::vec3(2.0f * 1.0f * TrackSection::trackHalfWidth, 0.0f, -2.0f * 4.0f * TrackSection::trackHalfWidth),
		glm::vec3(2.0f * 1.0f * TrackSection::trackHalfWidth, 0.0f, -2.0f * 1.0f * TrackSection::trackHalfWidth),
		Line::Orientation::VERTICAL, TrackSection::Direction::PLUS);
	track.sections.push_back(line4);
	trackSectionMeshes.push_back(line4->GetMesh());*/

	auto corner1 = std::make_shared<Corner>(
		glm::vec3(-TrackSection::trackHalfWidth, 0.0f, -2.0f * 1.0f * TrackSection::trackHalfWidth),
		TrackSection::trackHalfWidth, 1, TrackSection::Direction::PLUS);
	track.sections.push_back(corner1);
	trackSectionMeshes.push_back(corner1->GetMesh());

	/*auto corner2 = std::make_shared<Corner>(
		glm::vec3(0.0f, 0.0f, 0.0f),
		1.5f * TrackSection::trackHalfWidth, 2, TrackSection::Direction::PLUS);
	track.sections.push_back(corner2);
	trackSectionMeshes.push_back(corner2->GetMesh());

	auto corner3 = std::make_shared<Corner>(
		glm::vec3(0.0f, 0.0f, 0.0f),
		1.5f * TrackSection::trackHalfWidth, 3, TrackSection::Direction::PLUS);
	track.sections.push_back(corner3);
	trackSectionMeshes.push_back(corner3->GetMesh());

	auto corner4 = std::make_shared<Corner>(
		glm::vec3(0.0f, 0.0f, 0.0f),
		1.5f * TrackSection::trackHalfWidth, 4, TrackSection::Direction::PLUS);
	track.sections.push_back(corner4);
	trackSectionMeshes.push_back(corner4->GetMesh());*/

	auto corner5 = std::make_shared<Corner>(
		glm::vec3(-TrackSection::trackHalfWidth, 0.0f, -4.0f * TrackSection::trackHalfWidth),
		TrackSection::trackHalfWidth, 3, TrackSection::Direction::MINUS);
	track.sections.push_back(corner5);
	trackSectionMeshes.push_back(corner5->GetMesh());

	auto line5 = std::make_shared<Line>(
		glm::vec3(-2.0f * TrackSection::trackHalfWidth, 0.0f, -4.0f * TrackSection::trackHalfWidth),
		glm::vec3(-2.0f * TrackSection::trackHalfWidth, 0.0f, -8.0f * TrackSection::trackHalfWidth),
		Line::Orientation::VERTICAL, TrackSection::Direction::MINUS);
	track.sections.push_back(line5);
	trackSectionMeshes.push_back(line5->GetMesh());

	auto corner6 = std::make_shared<Corner>(
		glm::vec3(-TrackSection::trackHalfWidth, 0.0f, -8.0f * TrackSection::trackHalfWidth),
		TrackSection::trackHalfWidth, 2, TrackSection::Direction::MINUS);
	track.sections.push_back(corner6);
	trackSectionMeshes.push_back(corner6->GetMesh());

	auto line6 = std::make_shared<Line>(
		glm::vec3(-TrackSection::trackHalfWidth, 0.0f, -9.0f * TrackSection::trackHalfWidth),
		glm::vec3(7.0f * TrackSection::trackHalfWidth, 0.0f, -9.0f * TrackSection::trackHalfWidth),
		Line::Orientation::HORIZONTAL, TrackSection::Direction::PLUS);
	track.sections.push_back(line6);
	trackSectionMeshes.push_back(line6->GetMesh());

	// track.sections.push_back(std::make_shared<Line>(glm::vec3(6.0f, 0.0f, 0.0f), glm::vec3(12.0f, 0.0f, 0.0f), Line::Orientation::HORIZONTAL, TrackSection::Direction::PLUS));

	// track.sections.push_back(std::make_shared<Line>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(6.0f, 0.0f, 0.0f), Line::Orientation::HORIZONTAL, TrackSection::Direction::PLUS));
	// track.sections.push_back(std::make_shared<Line>(glm::vec3(6.0f, 0.0f, 0.0f), glm::vec3(6.0f, 0.0f, 6.0f), Line::Orientation::VERTICAL, TrackSection::Direction::PLUS));
	// track.sections.push_back(std::make_shared<Line>(glm::vec3(6.0f, 0.0f, 6.0f), glm::vec3(-6.0f, 0.0f, 6.0f), Line::Orientation::HORIZONTAL, TrackSection::Direction::MINUS));
	// track.sections.push_back(std::make_shared<Line>(glm::vec3(-6.0f, 0.0f, 6.0f), glm::vec3(-6.0f, 0.0f, 0.0f), Line::Orientation::VERTICAL, TrackSection::Direction::MINUS));

	// track.sections.push_back(std::make_shared<Line>(glm::vec3(0.0f, 0.0f, -6.0f), glm::vec3(-6.0f, 0.0f, -6.0f), Line::Orientation::HORIZONTAL, TrackSection::Direction::MINUS));
	// track.sections.push_back(std::make_shared<Line>(glm::vec3(-6.0f, 0.0f, -6.0f), glm::vec3(-6.0f, 0.0f, 6.0f), Line::Orientation::VERTICAL, TrackSection::Direction::PLUS));
	// track.sections.push_back(std::make_shared<Line>(glm::vec3(-6.0f, 0.0f, 6.0f), glm::vec3(6.0f, 0.0f, 6.0f), Line::Orientation::HORIZONTAL, TrackSection::Direction::PLUS));
	// track.sections.push_back(std::make_shared<Line>(glm::vec3(6.0f, 0.0f, 6.0f), glm::vec3(6.0f, 0.0f, -6.0f), Line::Orientation::VERTICAL, TrackSection::Direction::MINUS));

	// track.sections.push_back(std::make_shared<Corner>(glm::vec3(0.0f, 0.0f, 0.0f), 5.0f, 1, TrackSection::Direction::PLUS));
	// track.sections.push_back(std::make_shared<Corner>(glm::vec3(0.0f, 0.0f, 0.0f), 5.0f, 2, TrackSection::Direction::PLUS));
	// track.sections.push_back(std::make_shared<Corner>(glm::vec3(0.0f, 0.0f, 0.0f), 5.0f, 3, TrackSection::Direction::PLUS));
	// track.sections.push_back(std::make_shared<Corner>(glm::vec3(0.0f, 0.0f, 0.0f), 5.0f, 4, TrackSection::Direction::PLUS));

	// track.sections.push_back(std::make_shared<Corner>(glm::vec3(0.0f, 0.0f, 0.0f), 5.0f, 4, TrackSection::Direction::MINUS));
	// track.sections.push_back(std::make_shared<Corner>(glm::vec3(0.0f, 0.0f, 0.0f), 5.0f, 3, TrackSection::Direction::MINUS));
	// track.sections.push_back(std::make_shared<Corner>(glm::vec3(0.0f, 0.0f, 0.0f), 5.0f, 2, TrackSection::Direction::MINUS));
	// track.sections.push_back(std::make_shared<Corner>(glm::vec3(0.0f, 0.0f, 0.0f), 5.0f, 1, TrackSection::Direction::MINUS));

	std::for_each(trackSectionMeshes.begin(), trackSectionMeshes.end(), [](auto mesh)
	{
		mesh->initBuffers();
	});
}

void CMyApp::DrawTrack()
{
	for (int i = 0; i < track.sections.size(); ++i)
	{
		m_program.On();

		glm::mat4 matWorld =
			glm::scale<float>(0.1f, 0.1f, 0.1f) *
			glm::translate<float>(track.sections[i]->TranslateMeshTo()) *
			glm::rotate<float>(track.sections[i]->GetRotationAroundY(), 0, 1, 0);

		glm::mat4 matWorldIT = glm::transpose(glm::inverse(matWorld));
		glm::mat4 mvp = m_camera.GetViewProj() *matWorld;

		m_program.SetUniform("world", matWorld);
		m_program.SetUniform("worldIT", matWorldIT);
		m_program.SetUniform("MVP", mvp);
		m_program.SetUniform("eye_pos", m_camera.GetEye());

		m_program.SetTexture("texImage", 0, asphaltTextureID);

		trackSectionMeshes[i]->draw();

		// shader kikapcsolasa
		m_program.Off();
	}
}
