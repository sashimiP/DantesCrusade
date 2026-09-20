#pragma once
#include <filesystem>
#include <string>
#include <vector>
#include<iostream>
#include<fstream>
namespace fs = std::filesystem;
using std::string, std::cout, std::vector;

#include "raylib.h"
#include "TextureManager.h"
class Tiles
{
public:
	Tiles() = default;
	Tiles(float, float, std::shared_ptr<TextureManager>, float, fs::path);
	~Tiles();
	void Update(float);  // Handles movement logic
	void Display();
	void MoveRight(float, int);
	void MoveLeft(float, int);
	void SaveCoordinates();
	string LoadCoordinates() const;
	void SetTexture(const string& newType);
	float GetX();
	float GetY();


private:
	float x;
	float y;
	float globalSpeed;            // Base scroll speed
	fs::path coordinatesPath;
	string type;
	Texture2D* texture;
	std::shared_ptr<TextureManager> textureManager;
	
};

