#include "MyApp.h"

CMyApp::CMyApp(void)
{
}

CMyApp::~CMyApp(void)
{
}

bool CMyApp::Init()
{
	return false;
}

void CMyApp::Clean()
{
}

void CMyApp::Update()
{
}

void CMyApp::Render()
{
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent &)
{
}

void CMyApp::Resize(int, int)
{
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
}

void CMyApp::InitTextures()
{
}

void CMyApp::DeleteTextures()
{
}
