#pragma once
#include <string>
#include <vector>
#include<map>
#include<unordered_map>
#include <iostream>
#include<filesystem>
#include <algorithm>
#include <regex>
#include<fstream>
#include <random> 
namespace fs = std::filesystem;
#include "raylib.h"
#include "TextureManager.h"
#include"AssetOptions.h"
#include "Utilities.h"
#include "TextBox.h"
#include"Button.h"

using std::string, std::vector, std::map, std::unordered_map, std::tuple, std::pair, std::cout, std::endl,
std::to_string, std::shared_ptr, std::make_shared, std::unique_ptr, std::make_unique;

using Utilities::east, Utilities::west;

//struct FrameData
//{
//	int frames;
//	int colors;
//	Texture2D texture;
//};
using StanceTextures = map<string, FrameData>;
using TextureAtlas = unordered_map<string, StanceTextures>;

class Entity
{
	// FIND BETTER MEMBER FUNCTION NAMES
public:
	static bool stopMove;
	
	Entity() = default;

	// non-copyable
	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;

	// movable
	Entity(Entity&&) noexcept = default;
	Entity& operator=(Entity&&) noexcept = default;

	virtual ~Entity();


	Entity(fs::path dataPath,
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
		vector<string> dialogue = {},
		string name = "Unknown",
		int color = 0,
		bool dontScale = false,
		Vector2 patrolRoute = {0,0}
	);

	virtual void Display(int, int, float);
	void DrawShadow(int animationSpeed, float dt);
	void DrawPatrolRoute(int, Vector2);
	void DrawAssetOptions(int offsetX = 0);
	virtual void Update(float offsetX, float dt);

	virtual void Drag();

	void ShowAssetOptions();
	virtual string ClickAssetOption(float);

	// Dialogue manipulation functions
	virtual void AddText(string& textVariable);
	void AddText(vector<TextBox>& textVariable);
	void AddDialogue();
	void AddName();
	virtual void UpdateDialogue();
	virtual void DrawDialogue(float);

	virtual void Flip();
	virtual void SetStance(string, float);
	
	// Patroling functions
	void SetPatrolRoute();
	void DeactivateSetPatrolRoute();
	void Patrol(float offsetX, float dt);
	void BeginPatrol(float offsetX);


	virtual void MoveRight();
	virtual void MoveLeft();
	virtual void MoveRight(int, float);
	virtual void MoveLeft(int, float);

	// Getting option from Options
	string ClickOption();

	void Front();
	void Back();

	void Remove(bool save);

	// Shadow functions
	virtual void UpdateShadowRect();

	void UnloadMediaPortrait();

	// Getters
	float ID() const;
	float X() const;
	float Y() const;
	int Position() const;
	Texture2D* GetPortrait() const;
	virtual MediaStream* GetMediaPortrait();
	Texture2D* GetDialoguePortrait() const;
	string Name() const;
	Rectangle Rect() const;
	Rectangle DestRect() const;
	Rectangle CenterRect() const;
	vector<TextBox> Dialogue() const;
	Vector2 Center() const;
	string Type() const;
	Utilities::Direction Direction() const;
	bool Flipped() const;
	vector<Button>* GetButtons();
	//Setter masked as getter
	virtual bool& HasCollidedWithDante();
	//Setter masked as getter
	bool& ShowDialogue();
	//Setter masked as getter
	int& DialogueCounter();
	bool HasDialogue() const;
	bool HasAnimatedPortrait() const;

	// Setters
	void SetScrollSpeed(int);
	virtual void SetX(float);
	void SetX(float, float);
	virtual void SetY(float);
	virtual void ScaleDestRect(float);
	void SetPatrolDestination(Vector2, int);
	void SetEngage(bool& engage);

	bool marching{ false };
	bool scaled{ false }; // dont remember what it does!
	bool dontScale{ false };
	bool setRoute{ false };

protected:
	// Helper functions
	virtual void SaveCoordinates();
	void UpdateTextRect();
	virtual void CreateDirectories(string&) const;
	void CreateDialogueBoxes(vector<string> dialogue);
	//virtual void DrawPortrait(Utilities::Direction facing = west, bool dialogueBox = false) const;

	fs::path dataPath;
	fs::path unitPath;
	fs::path coordinatesPath;
	fs::path dialoguePath;

	string name;
	string town;
	string type;
	float id;
	int position; // position is used for sorting the units vector
	float x;
	float y;
	Utilities::Direction direction;
	string stance;
	vector<TextBox>dialogueBoxes;
	string unitName;
	vector<string> unitModes;
	shared_ptr<TextureManager> textureManager;
	StanceTextures* texturesMap;
	Texture2D* portrait = nullptr;
	Texture2D* dialoguePortrait = nullptr;
	Texture2D* animatedPortrait;
	MediaStreamPtr mediaPortrait;
	int portraitWidth{ 192 };
	int portraitHeight{ 192 };

	bool activateDrag{ false };
	bool save{ true };
	bool flipped;
	//float rotation = 0.f;
	float animationTimer{ 0.0f };
	int currentFrame;
	int scrollSpeed;
	int framesCount;
	
	const FrameData* spriteSheet;
	const Texture2D* texture;

	float frameWidth;
	float frameHeight;
	Rectangle rect;
	Rectangle sourceRect;
	Rectangle destRect;
	Vector2 center;
	Rectangle centerRect;
	
	AssetOptions Options;
	bool showRect{false};
	bool showAssetOptions{ false };
	bool drag;
	bool hasCollidedWithDante{ false };

	int framesCounter{ 0 };
	int colorCount{ 1 };
	int color{ 0 };

	// patrol variables
	Vector2 patrolDestination; // the variable that will be save
	Vector2 patrolRoute; // used for Patrol() function
	bool hasRoute{ false };
	bool beginPatrol{ false };
	Vector2 startingPosition;

	// Text input variables
	Rectangle textRect;	
	Button addButton = { "Add", 20 };
	Button backButton = { "Back", 20 };
	bool showDialogue{ false };
	bool addDialogue{ false };
	bool addName{ false };
	bool textBoxActive{ false };
	string text{ "" };
	int maxChars{ 64 };
	int fontSize{ 20 };
	int dialogueCounter{ 0 };
	bool showCursor{ true };
	int cursorBlinkRateOn{ 45 };    // frames cursor stays visible
	int cursorBlinkRateOff{ 15 };
	int cursorX;
	int cursorY;
	int cursorWidth;
	int cursorHeight;
	vector<std::string> options{ 
		"remove",
		"add name",
		"add dialogue",
		"flip",
		"front",
		"back",
		"=>",
		"<=",
		"scale",
		"dontScale"
	};

	//shadow variables
	Rectangle shadowSource;
	Rectangle shadowDest;
	Vector2 shadowOrigin;


	vector<Button> buttons;
	bool* engage = nullptr;
};

