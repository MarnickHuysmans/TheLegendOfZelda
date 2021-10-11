#pragma once
#include "Camera.h"
#include "Map.h"
#include "Player.h"
#include "ProjectileManager.h"
#include "PickUpManager.h"
#include "TextureManager.h"
#include "Transition.h"
#include "SoundManager.h"
#include "HUD.h"
#include "Title.h"

class Game final
{
public:
	explicit Game(const Window& window, float scale);
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	~Game();

	void Update(float elapsedSec);
	void Draw() const;

	// Event handling
	void ProcessKeyDownEvent(const SDL_KeyboardEvent& e);
	void ProcessKeyUpEvent(const SDL_KeyboardEvent& e);
	void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e);
	void ProcessMouseDownEvent(const SDL_MouseButtonEvent& e);
	void ProcessMouseUpEvent(const SDL_MouseButtonEvent& e);

private:
	// DATA MEMBERS
	const Window m_Window;
	TextureManager m_TextureManager;
	SoundManager m_SoundManager;
	Camera m_Camera;
	Player m_Player;
	Level* m_CurrentLevel;
	std::vector<Level*> m_Levels;
	ProjectileManager m_ProjectileManager;
	PickUpManager m_PickUpManager;
	Transition m_Transition;
	HUD m_Hud;
	Title m_Title;

	// FUNCTIONS
	void Initialize();
	void Cleanup();
	void ClearBackground() const;
	void ShowControls();
	void SetCurrentLevel();
	void Kill();
};
