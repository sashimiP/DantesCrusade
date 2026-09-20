#include "Tiles.h"

Tiles::Tiles(float x, float y, std::shared_ptr<TextureManager> textureManager, float globalSpeed, fs::path coordinatesPath) :
	x(x),
	y(y),
	textureManager(textureManager),
	globalSpeed(globalSpeed),
	coordinatesPath(coordinatesPath / "coordinates.txt")
{
	type = LoadCoordinates();
	texture = textureManager->Tiles(type);
}

Tiles::~Tiles()
{
	std::cout << "TILES DESTRUCTOR CALLED" << std::endl;
	SaveCoordinates();
}

void Tiles::SaveCoordinates()
{
	std::ofstream coordinates(coordinatesPath); // RAII: no need for manual close()
	if (!coordinates) {
		std::cerr << "Failed to save coordinates to " << coordinatesPath << std::endl;
		return;
	}
	coordinates << type << "\n";
	std::cout << "TILES SAVED WITH:\ntype: " << type << std::endl;
}

string Tiles::LoadCoordinates() const 
{
	std::ifstream coordinates(coordinatesPath); // RAII: no need for manual close()
	std::string line;
	if (!coordinates) {
		std::cerr << "Failed to load coordinates from " << coordinatesPath << std::endl;
		return ""; // Default value
	}
	if (!std::getline(coordinates, line)) {
		std::cerr << "File is empty or corrupted: " << coordinatesPath << std::endl;
		return "";
	}
	//std::cout << "TILES BACKGROUND TYPE: " << line << std::endl;
	return line;
}

void Tiles::MoveRight(float dt, int speed)
{
	x -= globalSpeed * speed * dt;

	
}

void Tiles::MoveLeft(float dt, int speed)
{

	x += globalSpeed * speed * dt;

}

// This will be using during marching mode!
void Tiles::Update(float dt)
{
	//x -= globalSpeed * 50 * dt;

	//// Wrap x within [0, tileWidth) to avoid precision drift
	//if (x <= -96) {
	//	x += 96;
	//}
	//else if (x >= 96) {
	//	x -= 96;
	//}
}

void Tiles::Display()
{
	if (!texture) return;
	int tileWidth = 96;
	int screenWidth = GetScreenWidth();
	int tilesNeeded = screenWidth / tileWidth + 3; // +3 to ensure coverage

	int startX = static_cast<int>(x) % tileWidth - tileWidth;

	for (int i = 0; i < tilesNeeded; i++)
	{
		DrawTexture(*texture, startX + i * tileWidth, y, RAYWHITE);
	}
}

void Tiles::SetTexture(const string& newType)
{
	type = newType;
	texture = textureManager->Tiles(newType);
}

float Tiles::GetX()
{
	return x;
}

float Tiles::GetY()
{
	return y;
}
