#include "Portrait.h"
bool Portrait::Show = true;

Portrait::Portrait(const std::string& type,const Texture& texture,const Rectangle& rect) :
	type(type), texture(texture), rect(rect), ID(texture.id)
{
};

void Portrait::Display() const
{
	if (Show)
	{
		const auto lightYellow = Color{ 253, 249, 0, 50 };
		
		DrawTexture(texture, rect.x, rect.y, WHITE);
		DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, BLUE);
		if (CheckCollisionPointRec(GetMousePosition(), rect)) DrawRectangleRec(rect, lightYellow);
	}
}

int Portrait::GetID()
{
	return ID;
}

std::string Portrait::GetType() const
{
	//cout << type << endl;
	return type;
}

Rectangle Portrait::GetRect() const
{
	return rect;
}

Texture Portrait::GetTexture() const
{
	return texture;
}
