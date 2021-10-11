#include "pch.h"
#include "Game.h"
#include "SVGParser.h"
#include <fstream>
#include "Level.h"
#include "LevelInitializer.h"
#include "StaticInitializer.h"
#include <iostream>

Game::Game(const Window& window, float scale)
	: m_Window{ window }
	, m_Camera(window.width, window.height, scale)
	, m_Player(m_SoundManager, m_TextureManager)
	, m_Transition(m_Player)
	, m_Hud(scale, m_SoundManager, m_TextureManager)
	, m_Title(scale, m_SoundManager, m_TextureManager)
{
	Initialize();
}

Game::~Game()
{
	Cleanup();
}

void Game::Initialize()
{
	StaticInitializer::InitStatics(m_TextureManager, m_SoundManager);
	Map* map = LevelInitializer::InitLevels(m_Levels, m_TextureManager, m_SoundManager);
	m_Player.SetMap(map);
	SetCurrentLevel();
	m_Player.InitDeathWarp(map);
	m_Title.PlayMusic();
}

void Game::Cleanup()
{
	for (Level* level : m_Levels)
	{
		delete level;
	}
}

void Game::Update(float elapsedSec)
{
	// Check keyboard state
	//const Uint8 *pStates = SDL_GetKeyboardState( nullptr );
	//if ( pStates[SDL_SCANCODE_RIGHT] )
	//{
	//	std::cout << "Right arrow key is down\n";
	//}
	//if ( pStates[SDL_SCANCODE_LEFT] && pStates[SDL_SCANCODE_UP])
	//{
	//	std::cout << "Left and up arrow keys are down\n";
	//}
	if (m_Title.Update(elapsedSec))
	{
		return;
	}
	if (m_Transition.Update(elapsedSec, m_CurrentLevel, m_Camera, m_SoundManager, m_ProjectileManager, m_PickUpManager))
	{
		return;
	}
	Map::UpdateFrame(elapsedSec);
	if (m_Hud.Update(elapsedSec))
	{
		m_CurrentLevel->PlayLoop();
		return;
	}
	if (m_Player.Update(elapsedSec, m_ProjectileManager, m_Transition, m_PickUpManager))
	{
		return;
	}
	m_ProjectileManager.Update(elapsedSec, m_Player, m_ProjectileManager);
	m_PickUpManager.Update(elapsedSec, m_Player);
	m_CurrentLevel->Update(elapsedSec, m_Player, m_ProjectileManager);
	m_Camera.Transform(m_Player.GetHitBox(), m_Player.GetMap(), m_CurrentLevel->IsFree());
}

void Game::Draw() const
{
	ClearBackground();
	if (m_Title.Draw())
	{
		return;
	}
	glPushMatrix();
	Map* map = m_Player.GetMap();
	Rectf cameraTransform = m_Camera.CameraTransform();
	//std::cout << std::to_string(cameraTransform.left) + ", " + std::to_string(cameraTransform.bottom) + ", " + std::to_string(cameraTransform.width) + ", " + std::to_string(cameraTransform.height) + "\n";
	if (map)
	{
		map->DrawLevelBackGround(cameraTransform);
	}
	m_PickUpManager.Draw();
	m_Player.DrawGround();
	m_CurrentLevel->DrawGround(cameraTransform);
	m_ProjectileManager.DrawGround();
	if (map)
	{
		map->DrawLevelTop(cameraTransform);
	}
	m_Player.DrawUp();
	m_CurrentLevel->DrawUp(cameraTransform);
	m_ProjectileManager.DrawUp();
	if (map)
	{
		map->DrawLevelUp(cameraTransform);
	}
	glPopMatrix();
	m_Transition.Draw(m_Window.width, m_Window.height);
	m_Hud.Draw(m_Player);
}

void Game::ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
	if (m_Title.KeyStroke(m_CurrentLevel))
	{
		return;
	}
	switch (e.keysym.sym)
	{
	case SDLK_i:
		ShowControls();
		break;
	case SDLK_j:
		m_Hud.Menu();
		break;
	case SDLK_f:
		Kill();
		break;
	}
}

void Game::ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
{
	//std::cout << "KEYUP event: " << e.keysym.sym << std::endl;
	//switch ( e.keysym.sym )
	//{
	//case SDLK_LEFT:
	//	//std::cout << "Left arrow key released\n";
	//	break;
	//case SDLK_RIGHT:
	//	//std::cout << "`Right arrow key released\n";
	//	break;
	//case SDLK_1:
	//case SDLK_KP_1:
	//	//std::cout << "Key 1 released\n";
	//	break;
	//}
}

void Game::ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
}

void Game::ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
{
	//std::cout << "MOUSEBUTTONDOWN event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
}

void Game::ProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
{
	//std::cout << "MOUSEBUTTONUP event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
}

void Game::ClearBackground() const
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Game::ShowControls()
{
	std::cout << std::endl;
	std::cout << "Movement: W, A, S, D\n";
	std::cout << "L: Interact\n";
	std::cout << "K: Swing Sword\n";
	std::cout << "J: Menu\n";
	std::cout << "F: Reset\n";
}

void Game::SetCurrentLevel()
{
	m_CurrentLevel = m_Player.GetMap()->GetFloor()->GetLevel();
	m_CurrentLevel->SetCurrentFloor(m_Player.GetMap()->GetFloor());
}

void Game::Kill()
{
	m_Player.Hit(Direction::south, 1000, m_ProjectileManager);
}
