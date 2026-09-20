#include <string>
#include <vector>

#include "raylib.h"
#include"Utilities.h"
#include "Entity.h"
#include "Editor.h"

int main()
{
	//SetConfigFlags(FLAG_VSYNC_HINT);
	//SetConfigFlags(FLAG_FULLSCREEN_MODE);
	InitWindow(SCR_WIDTH, SCR_HEIGHT, "Dante's Crusade");
	//ToggleFullscreen();
	SetTargetFPS(60);
	InitAudioDevice(); // Alway initialise before loading sounds and/or music streams!!!
	Image icon = LoadImage("resources/icon.png");
	SetWindowIcon(icon);
	Editor EDITOR(ANIMATION_SPEED);

	while (!WindowShouldClose() && !EDITOR.ExitGame())
	{
		auto mousePos = GetMousePosition();
		float dt = GetFrameTime();

		// Handle keyboard input
		EDITOR.KeyPressed(GetKeyPressed());
		EDITOR.KeyReleased();
		EDITOR.KeyDown(dt);

		// Handle mouse input
		EDITOR.MouseButtonReleased();
		EDITOR.MouseButtonPressed();

		// Updated
		EDITOR.Update(dt);

		// Collisions
		EDITOR.Collisions();		

		BeginDrawing();
		ClearBackground(DARKGRAY);

		EDITOR.Display(dt);
		
		EDITOR.PlaySoundTracks();
		EndDrawing();
	}

	// Clean up	
	CloseAudioDevice();       // Close the audio device
	CloseWindow();            // Close the window

	return 0;
}