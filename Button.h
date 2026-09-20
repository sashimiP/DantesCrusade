#pragma once
#include <functional>
#include <string>
#include <iostream>
#include "raylib.h"

#include"AnimatedAsset.h"

using std::string, std::move, std::unique_ptr;

class Button
{
public:
	Button() = default;
	Button(string text, int fontSize);
	~Button();
	void Update();
	// The action we want to run when clicked
	std::function<void()> onClick;
	void ClickButton();
	void SetPosition(int x, int y);
	void SetLeftOfRect(Rectangle);
	void SetRectColor(Color);
	Rectangle GetRect();
	string GetText();
	void Display();
	
	//void SetIcon(unique_ptr<AnimatedAsset> icon, Rectangle iconDest);
	bool drawFlag{ false };
private:

	string text;
	int fontSize;
	int textLength;
	Rectangle rect;
	Color rectColor;

	//unique_ptr<AnimatedAsset> icon;
};

