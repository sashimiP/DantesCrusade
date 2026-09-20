#pragma once
#include <string>  
#include <vector>  
#include <map>  
#include <iostream>  
#include <array>
#include "Utilities.h"  

#include "raylib.h"  

class Menu
{
public:
	Menu() = default;
	Menu(std::vector<std::string> options, int fontSize = FONT_SIZE);
	~Menu();
	void AddMenuOption(std::string);
	void CreateGrid(float x, float y);
	void CreateOptionsCells();
	void DrawMenu();
	void DrawMenu(int);
	std::string GetOption();
	float GetCellWidth();
	float GetCellHeight();
	Rectangle GetCell(int pos);
	void SetFrameRectColor(Color color);
	

private:


	std::vector<Rectangle> cells;
	std::vector<std::string> options;
	std::vector<std::pair<std::string, Rectangle>> optionCells;
	//int screenWidth;
	//int screenHeight;
	int fontSize;
	float cellWidth{ 0.f };
	float cellHeight{ 32.0f };
	Color rectFrameColor{ BLUE };
	void CreateGrid();
	
	
};

