#include "GateKeeper.h"


GateKeeper::GateKeeper(const string& enterTownName,
	fs::path dataPath,
	string town,
	shared_ptr<TextureManager> textureManager,
	float id,
	int position,
	string type,
	float x,
	float y,
	Utilities::Direction direction,
	bool drag,
	string stance,
	vector<string> dialogue,
	string name,
	int color
) :
	Entity(dataPath,
		town,
		textureManager,
		id,
		position,
		type,
		x,
		y,
		direction,
		drag,
		stance),
	enterTownName(enterTownName)
{
	enterTownButton = std::make_unique<Button>("enter " + this->enterTownName, FONT_SIZE);
	enterTownButton->SetPosition(rect.x, rect.y - 32.0f);
}

GateKeeper::~GateKeeper()
{
	//std::cout << "DESTRUCTOR CALLED:" << std::endl;
	//std::cout << "destination: " << enterTownName << std::endl
	//	<< "id: " << id << std::endl
	//	<< " position: " << position << std::endl
	//	<< " x: " << x << std::endl
	//	<< " y: " << y << std::endl
	//	<< " type: " << type << std::endl
	//	<< " flip: " << direction << std::endl
	//	<< " stance: " << stance << std::endl
	//	<< "name" << name << std::endl
	//	<< std::endl;
	SaveCoordinates();
	save = false;
}

void GateKeeper::DrawButton()
{
	if (hasCollidedWithDante)
		enterTownButton->Display();
}

bool& GateKeeper::HasCollidedWithDante()
{
	enterTownButton->SetPosition(rect.x, rect.y - 32.0f);

	return hasCollidedWithDante;
}

string GateKeeper::ClickButton()
{
	return enterTownName;
}

//void GateKeeper::SaveCoordinates()
//{
//	string unitName = to_string(this->position) + this->type + to_string(this->id);
//	CreateDirectories(unitName);
//	std::fstream coordinates;
//	cout << "SAVING UNIT COORDINATES PATH" << coordinatesPath << endl;
//	coordinates.open(coordinatesPath, std::ios::out);
//	if (coordinates.is_open())
//	{
//		coordinates << enterTownName << std::endl;
//		coordinates << id << std::endl;
//		coordinates << position << std::endl;
//		coordinates << x << std::endl;
//		coordinates << y << std::endl;
//		coordinates << type << std::endl;
//		coordinates << direction << std::endl;
//		coordinates << stance << std::endl;
//		coordinates << name << std::endl;
//		coordinates << color << std::endl;
//		coordinates.close();
//	}
//	else
//	{
//		cout << "Problem with file:" << endl;
//		cout << coordinatesPath << endl;
//	}
//}

Rectangle GateKeeper::ButtonRect()
{
	return enterTownButton->GetRect();
}

void GateKeeper::SetNextTown(const string& nextTownName)
{
	enterTownName = nextTownName;
	SetButton(enterTownName);
}

void GateKeeper::SetButton(const string& nextTownName)
{
	enterTownButton = std::make_unique<Button>("enter " + this->enterTownName, FONT_SIZE);
	enterTownButton->SetPosition(rect.x, rect.y - 32.0f);
}