#include "TownBackground.h"
TownBackground::TownBackground(std::shared_ptr<TextureManager> textureManager, float globalSpeed, fs::path coordinatesPath) :
	Background(textureManager, 50, coordinatesPath)
{

}

//void TownBackground::MoveRight(float dt)
//{
//	for (auto& layer : layers) {
//		layer.x -= globalSpeed * layer.speedFactor * dt;
//
//	}
//}
//
//void TownBackground::MoveLeft(float dt)
//{
//	
//	for (auto& layer : layers) {
//		layer.x += globalSpeed * layer.speedFactor * dt;
//	}
//	
//}

void TownBackground::Update(float dt, float speed)
{
	for (auto& layer : layers) {
		layer.x = std::fmod(layer.x, (float)layer.texture.width);
		if (layer.x > 0) {
			layer.x -= layer.texture.width;
		}
	}
}

void TownBackground::Display() {
	for (const auto& layer : layers) {
		// Always draw two textures side by side
		Vector2 pos1{ layer.x, 0 };
		Vector2 pos2{ layer.x + (float)layer.texture.width, 0 };

		DrawTextureEx(layer.texture, pos1, 0.0f, 1.0f, WHITE);
		DrawTextureEx(layer.texture, pos2, 0.0f, 1.0f, WHITE);
	}
}