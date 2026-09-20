#pragma once
#include <string>
#include <vector>
#include<iostream>
#include "raylib.h"
#include "Entity.h"

class Portrait
{
public:
	Portrait() = default;
	Portrait(const std::string&, const Texture&, const Rectangle&);
	virtual ~Portrait() {};
	virtual void Display() const;

	int GetID();
	std::string GetType() const;
	Rectangle GetRect() const;
	Texture GetTexture() const;
	static bool Show;
	//bool Clicked;
	
	
protected:
	int ID;
	std::string type;
	Rectangle rect;
	Texture texture;	
	
};

