#include "TextBox.h"

TextBox::TextBox(string text, int fontSize, const Rectangle& rect, const Color& textColor):
	text(std::move(text)),
	fontSize(fontSize),
	rect(rect),
	textColor(textColor)
{
	/*showDialogue =  false;
	addDialogue = false;
	textBoxActive = false;*/
	boxColor = DARKGRAY;
	boxColor.a = 200;
}

void TextBox::MoveRight(int globalSpeed, float dt)
{
	rect.x -= movementSpeed * globalSpeed * dt;
}

void TextBox::MoveLeft(int globalSpeed, float dt)
{
	rect.x += movementSpeed * globalSpeed * dt;
}

void TextBox::UpdateRect(const Rectangle& rect)
{
	this->rect.x = rect.x;
}

void TextBox::Display()
{	
	DrawRectangleRec(rect, boxColor);
	DrawText(text.c_str(), rect.x+5, rect.y+10, fontSize, textColor);
	DrawRectangleLinesEx(rect, 1, YELLOW);
	
}

string TextBox::GetText() const
{
	return text;
}