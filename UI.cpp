#include "UI.h"

UI::UI(shared_ptr<TextureManager> textureManager, const vector<string>&armyTypes):
	textureManager(textureManager),
	armyTypes(armyTypes)
{
	cellsBackgroundColor = Color{ 0, 0, 0, 200 };

	squareFrame = textureManager->Asset("square_frame");
	decoratedFrame = textureManager->Asset("decorated_frame");

	armyIconWidth = BASIC_PORTRAIT_WIDTH;
	armyIconHeight = BASIC_PORTRAIT_HEIGHT;
	
	
	squareFrameSourceRect = Rectangle{ 0.0f, 0.0f, static_cast<float>(squareFrame->width), static_cast<float>(squareFrame->height) };

	/*armyDestRect = Rectangle{ static_cast<float>(SCR_WIDTH - (armyIconWidth * 2)), static_cast<float>(DIALOGUE_PORTRAIT_HEIGHT), static_cast<float>(armyIconWidth), static_cast<float>(armyIconHeight) };*/
	origin = { 0.0f, 0.0f };

	topDecoratedFrameSourceRect = Rectangle{ 0.0f, 0.0f, static_cast<float>(decoratedFrame->width), static_cast<float>(decoratedFrame->height) };
	bottomDecoratedFrameSourceRect = Rectangle{ 0.0f, 0.0f, static_cast<float>(decoratedFrame->width), -static_cast<float>(decoratedFrame->height) };
	topDecoratedFrameWidth = BASIC_PORTRAIT_WIDTH * 2;
	topDecoratedFrameHeight = DIALOGUE_PORTRAIT_HEIGHT;
	topDecorateFrameRect = Rectangle{
		static_cast<float>(SCR_WIDTH - topDecoratedFrameWidth),
		0.0f,
		static_cast<float>(topDecoratedFrameWidth),
		static_cast<float>(topDecoratedFrameHeight)
	};

	bottomDecoratedFrameWidth = BASIC_PORTRAIT_WIDTH * 2;
	bottomDecoratedFrameHeight = SCR_HEIGHT - (BASIC_PORTRAIT_HEIGHT * 6 + DIALOGUE_PORTRAIT_HEIGHT);
	bottomDecorateFrameRect = Rectangle{
		static_cast<float>(SCR_WIDTH - bottomDecoratedFrameWidth),
		static_cast<float>((BASIC_PORTRAIT_HEIGHT * 6) + DIALOGUE_PORTRAIT_HEIGHT),
		static_cast<float>(bottomDecoratedFrameWidth),
		static_cast<float>(bottomDecoratedFrameHeight)
	};

	stats = {
	{ "basic_attack",   &Dante::attack },
	{ "bronze_armor",    &Dante::armor },
	{ "holiness", &Dante::holiness },
	{ "food",     &Dante::food },
	{ "gold",     &Dante::gold }
	};

	const auto& defaultStatsTexture = textureManager->Asset("holiness");
	statsWidth = defaultStatsTexture->width;
	statsHeight = defaultStatsTexture->height;

	statsSourceRect = Rectangle(0.0f, 0.0f, statsWidth, statsHeight);

}

void UI::CreateGrid(vector<Portrait>& Portraits, Direction direction)
{
	armyPortraitCells.clear();
	const float width{ static_cast<float>(armyIconWidth) };
	const float height{ static_cast<float>(armyIconHeight) };
	float x = (direction == east) ? static_cast<float>(SCR_WIDTH - (armyIconWidth*2)): 0.0f;
	float y{ static_cast<float>(DIALOGUE_PORTRAIT_HEIGHT) };
	
	armyPortraitCells.reserve(12);

	for (int i{}; i < 12;i++)
	{
		Rectangle rect{ x, y, width, height };
		armyPortraitCells.push_back(rect);

		// Update position for next cell
		y += height;

		// Check if we need to move to next column
		if (y + height > SCR_HEIGHT - 128.0f) {  // 128 = 64*2 margin
			y = static_cast<float>(DIALOGUE_PORTRAIT_HEIGHT);
			x += width;
		}
	}
}

void UI::PrepArmyPortraits()
{
	armyPortraits.clear();
	int i = 0;
	for (const auto& [type, count] : countArmyTypes) {
		auto texture = textureManager->Portrait(type);
		armyPortraits.emplace_back(type, *texture, armyPortraitCells[i++]);
	}
}

void UI::PrepStatsDestRects(Direction direction)
{
	statsDestRect.clear();
	float x = (direction == east) ? static_cast<float>(SCR_WIDTH + 32.0f - bottomDecoratedFrameWidth) : 32.0f;
	float y = static_cast<float>((BASIC_PORTRAIT_HEIGHT * 6) + DIALOGUE_PORTRAIT_HEIGHT + 16.0f);

	statsDestRect.reserve(stats.size());

	for (int i{}; i < stats.size(); i++)
	{
		statsDestRect.emplace_back( x,y,static_cast<float>(statsWidth), static_cast<float>(statsHeight) );

		y += statsHeight + 5.0f;
	}
}

void UI::PrepStatsPortraits()
{
	statsPortraits.clear();
	statsPortraits.reserve(stats.size());
	int i{};
	for (const auto& [stat, value] : stats)
	{
		statsPortraits.emplace_back(stat, *textureManager->Asset(stat), statsDestRect[i++]);
	}
}

void UI::CountTypes()
{
	countArmyTypes.clear();
	for (const auto& type : armyTypes) {
		++countArmyTypes[type];  // increment frequency
	}
}

void UI::SetUI(Direction direction)
{

	topDecorateFrameRect = Rectangle{
		(direction == east)? static_cast<float>(SCR_WIDTH - topDecoratedFrameWidth) : 0.0f,
		0.0f,
		static_cast<float>(topDecoratedFrameWidth),
		static_cast<float>(topDecoratedFrameHeight)
	};

	bottomDecorateFrameRect = Rectangle{
		(direction == east) ? static_cast<float>(SCR_WIDTH - bottomDecoratedFrameWidth) : 0.0f,
		static_cast<float>((BASIC_PORTRAIT_HEIGHT * 6) + DIALOGUE_PORTRAIT_HEIGHT),
		static_cast<float>(bottomDecoratedFrameWidth),
		static_cast<float>(bottomDecoratedFrameHeight)
	};

	CountTypes();
	CreateGrid(armyPortraits, direction);
	PrepArmyPortraits();
	PrepStatsDestRects(direction);
	PrepStatsPortraits();
}

void UI::SetStats(string& stat, float newStat)
{
	PrepStatsPortraits();
}

void UI::BeginDrawing(bool draw)
{
	this->draw = draw;
}

void UI::Display()
{
	if (!draw) return;
	for (auto& portrait : armyPortraits)
	{
		DrawRectangleRec(portrait.GetRect(), cellsBackgroundColor);
		DrawTexturePro(portrait.GetTexture(), squareFrameSourceRect, portrait.GetRect(), origin, 0.0f, RAYWHITE);
	}

	for (int i{}; i < armyPortraitCells.size(); i++)
	{
		if(i>=armyPortraits.size())
			DrawRectangleRec(armyPortraitCells[i], cellsBackgroundColor);
		DrawTexturePro(*squareFrame, squareFrameSourceRect, armyPortraitCells[i], origin, 0.0f, RAYWHITE);
	}

	if (!engagedInDialogue)
	{
		DrawRectangleRec(topDecorateFrameRect, cellsBackgroundColor);
		DrawTexturePro(*decoratedFrame, topDecoratedFrameSourceRect, topDecorateFrameRect, origin, 0.0f, RAYWHITE);
	}

	DrawRectangleRec(bottomDecorateFrameRect, cellsBackgroundColor);
	DrawTexturePro(*decoratedFrame, bottomDecoratedFrameSourceRect, bottomDecorateFrameRect, origin, 0.0f, RAYWHITE);

	int i = 0;
	for (const auto& [type, count] : countArmyTypes)
	{
		string c = std::to_string(count);
		DrawText(c.c_str(), armyPortraitCells[i].x, armyPortraitCells[i++].y, 16, RAYWHITE);
	}

	for (const auto& stat : statsPortraits)
	{
		DrawTexturePro(stat.GetTexture(), statsSourceRect, stat.GetRect(), origin, 0.0f, RAYWHITE);
		string c = to_string((int)*stats[stat.GetType()]);
		DrawText(c.c_str(), stat.GetRect().x + 18.0f, stat.GetRect().y, 18, RAYWHITE);
	}
}

void UI::AddArmyType(string type)
{
	countArmyTypes[type] += 1;
	auto it = std::find(armyTypes.begin(), armyTypes.end(), type);

	if (it == armyTypes.end())
	{
		PrepArmyPortraits();
	}
	armyTypes.push_back(type);
}
