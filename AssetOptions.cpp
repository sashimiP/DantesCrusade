#include "AssetOptions.h"



AssetOptions::AssetOptions(const std::string& unitType, const Rectangle& unitRect, const vector<string>& options) :
	filepath(fs::path("resources") / "units" / unitType),
	unitRect(unitRect),
	options(options)
{
	size_t capacity = this->options.size() + Utilities::getFileCount(filepath)+2;
	this->options.reserve(capacity);
	cells.reserve(capacity);
	optionCells.reserve(capacity);
	
	//std::cout << "AssetOptions constructor called with Entity " << std::endl;
	for (const auto& option : options)
		cellWidth = std::max(MeasureText(option.c_str(), fontSize)+10, cellWidth);

	FindAvailableOptions();
	CreateGrid();
	CreateAssetOptions();
}

AssetOptions::~AssetOptions() {
	//std::cout << "AssetOptions destructor called" << std::endl;
}

void AssetOptions::CreateAssetOptions()
{
	for (size_t i = 0; i < options.size(); ++i)
	{
		optionCells.emplace(options[i], cells[i]);
	}

}

void AssetOptions::FindAvailableOptions()
{
	if (!fs::exists(filepath)) return;

	// Iterate directory
	for (const auto& entry : fs::directory_iterator(filepath)) {
		std::string option = entry.path().filename().string();
		cellWidth = std::max(MeasureText(option.c_str(), fontSize)+10, cellWidth);
		if (option == "walk")
		{
			options.emplace_back(std::move("setRoute"));
			options.emplace_back(std::move("patrol"));
		};
		options.emplace_back(std::move(option));

	}
}

void AssetOptions::CreateGrid()
{
	float width = cellWidth;
	float height = 32;
	float x{ unitRect.x - width };
	float y{ unitRect.y - height};
	bool rightSide = false;
	for (int i{ 0 }; i < cells.capacity(); ++i) {
		auto rect = Rectangle{ x, y, width, height };
		cells.push_back(rect);
		
		if ((rect.y + rect.height*2) < unitRect.height + unitRect.y) {
			//x = x;
			y += rect.height;
		}
		else if(!rightSide) {
			rightSide = true;
			x = unitRect.x + unitRect.width;
			y = unitRect.y - height;
		}
		else if (rightSide)
		{
			x = x + width;
			y = unitRect.y - height;
		}
	}
}

void AssetOptions::drawOptionCells() {

	const auto lightYellow = Color{ 253, 249, 0, 50 };	
	for (const auto& cell : optionCells) {

		if (CheckCollisionPointRec(GetMousePosition(), cell.second))
		{
			DrawRectangle(
				cell.second.x,
				cell.second.y,
				cell.second.width,
				cell.second.height,
				lightYellow);
		}
		else
		{
			DrawRectangle(
				cell.second.x,
				cell.second.y,
				cell.second.width,
				cell.second.height,
				DARKGRAY);
		}

		DrawRectangleLines(cell.second.x, cell.second.y, cell.second.width, cell.second.height, BLUE);
		DrawText(cell.first.c_str(), cell.second.x + 5, cell.second.y + 5, fontSize, RAYWHITE);
	}

}
void AssetOptions::updateGrid(const Rectangle& newUnitRect)
{
	unitRect = newUnitRect;
	cells.clear();
	CreateGrid();

	optionCells.clear();
	for (size_t i = 0; i < options.size() && i < cells.size(); ++i) {
		optionCells.emplace(options[i], cells[i]);
	}
}

string AssetOptions::getOption() const {
	Vector2 mousePos = GetMousePosition();
	for (const auto& [name, rect] : optionCells) {
		if (CheckCollisionPointRec(mousePos, rect)) {
			return name;
		}
	}
	return "";
}

void AssetOptions::SetAssetOptions(const vector<string>& options)
{
	this->options.clear();
	cells.clear();
	this->options = options;
	size_t capacity = this->options.size() + Utilities::getFileCount(filepath)+2;
	this->options.reserve(capacity);
	cells.reserve(capacity);
	optionCells.reserve(capacity);

	//std::cout << "AssetOptions constructor called with Entity " << std::endl;
	for (const auto& option : this->options)
		cellWidth = std::max(MeasureText(option.c_str(), fontSize) + 10, cellWidth);

	FindAvailableOptions();
	CreateGrid();
	CreateAssetOptions();
}

bool AssetOptions::hasOption(const std::string& option) const
{
	return std::find(options.begin(), options.end(), option) != options.end();
}



