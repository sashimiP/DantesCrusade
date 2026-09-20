#include "Menu.h"

Menu::Menu(std::vector<std::string> options, int fontSize):
	options(std::move(options))
{
	//std::cout << "MENU CONSTRUCTOR CALLED WITH:\n";
	//std::cout << "SCREEN WIDTH: " << SCR_WIDTH << std::endl;
	//std::cout << "SCREEN HEIGHT: " << SCR_HEIGHT << std::endl;
	
	this->cells.reserve(this->options.size());
	this->fontSize = fontSize;
	
	CreateGrid();
	CreateOptionsCells();
}

Menu::~Menu()
{
	//std::cout << "CALLING MENU DESTRUCTOR WITH:\n";
	for (auto& option : options)
		std::cout << option << std::endl;
}

void Menu::AddMenuOption(std::string option)
{

	options.insert(options.end()-1, std::move(option));
	CreateGrid();
	CreateOptionsCells();
}

void Menu::CreateGrid()
{
	cells.clear();
	for (auto& option : options)
	{
		cellWidth = std::max(MeasureText(option.c_str(), fontSize) + 10, static_cast<int>(cellWidth));
	}
	float x{ 0 };
	float y{ 0 + cellHeight };
	for (int i{ 0 }; i < options.size(); ++i) {
		auto rect = Rectangle{ x, y, cellWidth, cellHeight };
		cells.push_back(rect);


		if ((rect.y + rect.height * 2) < SCR_HEIGHT - 64 * 2)
		{
			x = x;
			y += rect.height;
		}
		else
		{
			//std::cout << "reached Bottom" << std::endl;
			x += rect.width;
			y = cellHeight;
		}
	}
}

void Menu::CreateGrid(float x, float y)
{
	cells.clear();
	for (auto& option : options)
	{
		cellWidth = std::max(MeasureText(option.c_str(), fontSize) + 10, static_cast<int>(cellWidth));
	}

	for (int i{ 0 }; i < options.size(); ++i) {
		auto rect = Rectangle{ x, y, cellWidth, cellHeight };
		cells.push_back(rect);


		if ((rect.y + rect.height * 2) < SCR_HEIGHT - 64 * 2)
		{
			x = x;
			y += rect.height;
		}
		else
		{
			//std::cout << "reached Bottom" << std::endl;
			x += rect.width;
			y = cellHeight;
		}
	}
}


void Menu::CreateOptionsCells() {
	optionCells.clear();
	for (int i{}; i < options.size(); ++i)
		optionCells.emplace_back(options[i], cells[i]);
}

void Menu::DrawMenu()
{
	const auto lightYellow = Color{ 253, 249, 0, 50 };
	for (const auto& cell : optionCells) {
		auto backgroundColor = (CheckCollisionPointRec(GetMousePosition(), cell.second)) ? lightYellow : DARKGRAY;

		DrawRectangleRec(cell.second, backgroundColor);
		DrawRectangleLines(cell.second.x, cell.second.y, cell.second.width, cell.second.height, rectFrameColor);
		DrawText(cell.first.c_str(), cell.second.x + 5, cell.second.y + 5, fontSize, RAYWHITE);
	}
}

void Menu::DrawMenu(int lastIndex)
{
	const auto lightYellow = Color{ 253, 249, 0, 50 };
	for (int i{}; i < lastIndex; i++)
	{
		
		auto backgroundColor = (CheckCollisionPointRec(GetMousePosition(), optionCells[i].second)) ? lightYellow : DARKGRAY;

		DrawRectangleRec(optionCells[i].second, backgroundColor);
		DrawRectangleLinesEx(optionCells[i].second, 1.0f, BLUE);
		DrawText(optionCells[i].first.c_str(), optionCells[i].second.x + 5, optionCells[i].second.y + 5, fontSize, RAYWHITE);
		
	}
}

std::string Menu::GetOption() 
{
	for (const auto& [option, rect] : optionCells) {
		if (CheckCollisionPointRec(GetMousePosition(), rect))
		{
			std::cout << option << std::endl;
			return option;
		}
	}
	return "";
}

float Menu::GetCellWidth()
{
	return cellWidth;
}

float Menu::GetCellHeight()
{
	return cellHeight;
}

Rectangle Menu::GetCell(int pos)
{
	return optionCells[pos].second;
}

void  Menu::SetFrameRectColor(Color color)
{
	rectFrameColor = color;
}