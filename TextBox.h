#pragma once
#include "raylib.h"
#include<iostream>
#include<string>
using std::string;


class TextBox
{
public:
	TextBox() = default;
	TextBox(string text, int fontSize, const Rectangle& rect, const Color& textColor);
	void MoveRight(int globalSpeed, float dt);
	void MoveLeft(int globalSpeed, float dt);
	void UpdateRect(const Rectangle&);
	void Display();
	string GetText() const;

private:
	string text;
	int fontSize;
	Rectangle rect;
	Color textColor;
	Color boxColor;
	int movementSpeed{ 50 };

	//bool showDialogue;
	//bool addDialogue;
	//bool textBoxActive;
};

