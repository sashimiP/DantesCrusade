#include "Dialogue.h"

Dialogue::Dialogue(const fs::path& path,
	const vector<string>& dialogueStr,
	const Rectangle& rect,
	TextureManager* textureManager,
	const string& unitType):

	path(path),
	textureManager(textureManager),
	unitType(unitType)

{
	cout << "CALLING DIALOGUE CONSTRUCTOR" << endl;
	SetTextRect(rect);
	SetDialogue(dialogueStr);
}

Dialogue::~Dialogue()
{
	cout << "CALLING DIALOGUE DESTRUCTOR" << endl;
}

void Dialogue::AddDialogue(Rectangle rect)
{
	if (dialogue.empty())
		dialogue.emplace_back(TextBox("!", 20, textRect, RAYWHITE));

	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
	{

		Vector2 mouse = GetMousePosition();
		textBoxActive = CheckCollisionPointRec(mouse, textRect);

		// Add button clicked
		if (CheckCollisionPointRec(mouse, addButtonRect) && !text.empty())
		{
			addDialogue = false;
			textBoxActive = false;
			cursor.showCursor = false;
			if (dialogue.empty())
				dialogue.emplace_back(TextBox("!", fontSize, textRect, RAYWHITE));
			dialogue.emplace_back(TextBox(text, fontSize, textRect, RAYWHITE));
			if (portrait == nullptr) portrait = textureManager->Portrait(unitType);
			text = "";
			textRect = Rectangle{ rect.x, rect.y, 20, 40 };
			return;
		}
	}

	// Handle character input
	if (textBoxActive)
	{
		int key = GetCharPressed();
		while (key > 0) {
			if ((key >= 32) && (key <= 125) && (text.length() < maxChars)) {
				text += (char)key;
				int textWidth = MeasureText(text.c_str(), 20);
				textRect.width = 20 + textWidth;
				//std::string lastCharStr(1, text.back()); // or text[text.size() - 1]
				cursor.x += MeasureText("a", 20) + 2;
				cursor.width += MeasureText("a", 20) + 2;
			}
			key = GetCharPressed();
		}

		// Handle backspace
		if (IsKeyPressed(KEY_BACKSPACE) && !text.empty()) {
			auto deletedKey = text[text.size() - 1];
			text.pop_back();
			textRect.width = 20 + MeasureText(text.c_str(), 20);
			if (deletedKey != '\n')
			{
				cursor.x -= MeasureText("a", 20) + 2;
				cursor.width -= MeasureText("a", 20) + 2;
			}
			else if (deletedKey == '\n')
			{
				textRect.height -= 20 + 4;
				cursor.x = textRect.x + 6 + MeasureText(text.c_str(), 20);
				cursor.width = textRect.x + 6 + MeasureText(text.c_str(), 20);
				cursor.y -= 20 + 4;
				cursor.height = cursor.y + 30;
			}
		}

		//Cursor blinking logic
		framesCounter++;

		if (cursor.showCursor && framesCounter > cursor.blinkRateOn) {
			cursor.showCursor = false;
			framesCounter = 0;
		}
		else if (!cursor.showCursor && framesCounter > cursor.blinkRateOff) {
			cursor.showCursor = true;
			framesCounter = 0;
		}
		if (IsKeyPressed(KEY_ENTER))
		{
			text += '\n';
			textRect.width = 20 + MeasureText(text.c_str(), 20);
			textRect.height += 20 + 4;
			cursor.y += 20 + 4;
			cursor.x = textRect.x + 6;
			cursor.width = textRect.x + 6;
			cursor.height = cursor.y + 30;
		}
	}
}

void Dialogue::SetPath(fs::path path)
{
	this->path = path;
}

void Dialogue::SetTextRect(Rectangle rect)
{
	textRect = std::move(rect);

	cursor.x = textRect.x + 6;
	cursor.y = textRect.y + 5;
	cursor.width = textRect.x + 6;
	cursor.height = textRect.y + 35;

	addButtonRect = { textRect.x - 60, textRect.y, 60, 40 };
}

void Dialogue::SetDialogue(const vector<string>& dialogueStr)
{
	if (dialogueStr.empty()) return;

	dialogue.reserve(dialogueStr.size());
	portrait = textureManager->Portrait(unitType);
	for (auto& line : dialogueStr)
	{
		auto cellWidth = MeasureText(line.c_str(), fontSize) + 10;
		dialogue.emplace_back(TextBox(line, fontSize, Rectangle(textRect.x, textRect.y, cellWidth, 40), RAYWHITE));
	}
}



void Dialogue::SetPortrait(Texture2D* portrait)
{
	this->portrait = portrait;
}