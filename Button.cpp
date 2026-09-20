#include "Button.h"

Button::Button(string text, int fontSize):
	text(move(text)),
	fontSize(fontSize)
{
	textLength = MeasureText(this->text.c_str(), fontSize) + 20;
	rectColor = BLACK;
	rect = { 0.0f, 0.0f, static_cast<float>(textLength), 32.0f };
	//icon = make_unique<AnimatedAsset>();

}

Button::~Button(){}

void Button::Update()
{
	ClickButton();

	/*if (icon)
		if (CheckCollisionPointRec(GetMousePosition(), rect))
			icon->Update();*/
}

void Button::ClickButton()
{
	
	if (!IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) return;
	
	auto mouse = GetMousePosition();
	if (CheckCollisionPointRec(mouse, rect))
	{
		std::cout << "Button clicked!\n";
		if (onClick) {
			onClick();           // This is the magic line
		}
	}
	
}

void Button::SetLeftOfRect(Rectangle textRect)
{
	rect = { textRect.x - textLength, textRect.y, (float)textLength, 32 };
}

void Button::SetPosition(int x, int y)
{
	rect = { static_cast<float>(x),static_cast<float>(y), static_cast<float>(textLength), 32.0f };
}

void Button::SetRectColor(Color newColor)
{
	rectColor = newColor;
}

Rectangle Button::GetRect()
{
	return rect;
}

void Button::Display()
{
	const auto lightYellow = Color{ 253, 249, 0, 50 };
	auto backgroundColor = (CheckCollisionPointRec(GetMousePosition(), rect)) ? lightYellow : DARKGRAY;
	
	DrawRectangleRec(rect, backgroundColor);

	DrawText(text.c_str(), rect.x + 10, rect.y + 10, 20, WHITE);
	DrawRectangleLinesEx(rect, 1, rectColor);

	/*if (icon)
	{
		icon->Draw();
		icon->DrawOnce();
	}*/
}

string Button::GetText()
{
	return text;
}

//void Button::SetIcon(unique_ptr<AnimatedAsset> icon, Rectangle iconDest)
//{
//	this->icon = std::move(icon);
//	
//	this->icon->SetDest(iconDest);
//	rect = iconDest;
//	
//}