#pragma once
#include "Entity.h"
#include <queue>
#include <algorithm>
#include "AnimatedAsset.h"

using std::queue;
using Utilities::Relationship;
using Utilities::Stat;
using Utilities::DrawTextOutlined;
using Utilities::entityStats;

// Program the squares, so that with every movement on the grid, the squares are updated!!!
//shared_ptr<Rectangle> topSquare;
//shared_ptr<Rectangle> bottomSquare;
//shared_ptr<Rectangle> eastSquare;
//shared_ptr<Rectangle> westSquare;

class BattleEntity;

struct Cell {
	
	int gridIndex{ -1 };
	shared_ptr<Rectangle> rect{ };
	shared_ptr<BattleEntity> entity{ nullptr };
	Utilities::EntityType entityType{ Utilities::EntityType::Unknown };
	int entityHealth{ 0 };
	bool mouseOverCell{ false };
	bool selected{ false };
	Color color{ BATTLE_CELL_CLR };
	bool drawRect{ false };
	
};

enum class Side
{
	north,
	south,
	east,
	west,
	north_east,
	north_west,
	south_east,
	south_west
};

struct AdjacentCell
{
	Side side;
	Cell* cell{nullptr};
	bool mouseCollide{ false };
};

struct EnemyStats
{
	string type;
	float health;
};

class BattleEntity :
	public Entity
{
public:
	BattleEntity() = default;
	BattleEntity(Relationship relationship,
		const int cellIndex,
		shared_ptr<vector<Cell>> grid,
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
		vector<string> dialogue = {},
		string name = "Unknown",
		int color = 0
	);

	virtual ~BattleEntity();

	void SetMediaPortrait();
	virtual void Update(bool selected);
	virtual void Select();

	//void FindTargetAttack();

	virtual void Deselect();
	virtual void Move(int rectIndex);
	virtual void Move(vector<int>& path);
	virtual void MoveToCell();
	virtual vector<int> TrailPath(int startIndex, int endIndex);
	vector<int> ReachableCells(int startIndex, int movementPoints);
	bool CanReachCell(int cellIndex);

	void UpdateAdjacentCells();
	void UpdateShadowRect() override;
	virtual void Attack(shared_ptr<Cell> enemyCell);
	virtual void AttackEnemy();

	virtual void DrawShadow(int animationSpeed, float dt);
	virtual void Display(int, int, float);
	void DisplayTargeted(int animationSpeed);
	void DrawIcons();
	void DrawPortrait(int, float);
	void DrawAnimatedPortrait(float dt);
	// Draws HP/mana bar
	virtual void DrawStatsBar();



	virtual void SetCell(const Rectangle&);
	virtual void SetStance(string stance);
	void SetAttackType(string attackType);

	virtual void MouseOver(bool flag);
	void AreEnemiesAdjacent();

	//virtual void Lured(int enemyIndex, int moveToIndex, vector<int> path);
	virtual void Lured(int moveToIndex, vector<int> path);
	virtual void Retaliate();
	//void ActivateRetaliate(shared_ptr<Cell> enemyCell);
	void ActivateRetaliate(int dmg);
	//void ActivateGetHit(std::optional<int> attackerIndex = std::nullopt);

	void StartShake(float duration = 0.35f, float strength = 18.0f);
	void ShakeEntity();

	void ActivateGetHit(bool dontRetaliate = false, int dmg = 0);
	void ActivateHolyHit(int dmg = 0);
	void GetHit(); // not a getter!
	void GetHolyHit(); // not a getter!

	virtual void Death();

	//Getters
	virtual int GetMoveToIndex();
	Rectangle GetShadowRect();
	const AdjacentCell GetAttackDirection();
	const AdjacentCell* GetAvailableAdjacentCell(); 
	const bool GetMouseOver();
	vector<int> GetAdjacentCells(int cellIndex);
	const int GetCellIndex();
	const Utilities::Relationship GetRelationship();
	const bool GetHadItsTurn();
	const int GetRank();
	const int GetHealth();
	const int GetAttack();
	MediaStream* GetMediaPortrait() override;
	Utilities::EntitySize GetSize();
	bool GetSkipTurn();
	string GetType();
	bool GetTaunting();
	bool HasDied();

	//void SetOpposingArmy(vector<shared_ptr<BattleEntity>> opposingArmy);
	void SetEnemy(shared_ptr<Cell> enemyCell);
	void SetPosition(int newPos);
	void SetTargeted(bool targeted);

	void SetStatsTextColor(const int& startingStats, const int& currentStats, Color& statsColor);

	virtual void AddStat(Stat stat, int amount);
	virtual void SubStat(Stat stat, int amount);
	void AlterState(Stat stat, bool stateModifier);

	virtual	void EnemyRetaliate(int amount);

	void SetHadItsTurn(bool hadItsTurn);

	void ResetEnemy();
	void EndTurn();

	bool offence{ false };

	bool selected{ false };
	bool defending{ false };

	bool nextToEnemy{ false };

	// Used in the Battleground phase
	bool drawDialoguePortrait{ false };


	bool retaliated{ false };
	bool finishedAnimationSequence{ false };
	bool dead{ false };

protected:

	bool  shakeEntity = false;
	float shakeTime = 0.0f;
	float shakeDuration = 0.35f;
	float shakeStrength = 18.0f;   // pixels

	Utilities::Relationship relationship;
	int cellIndex{ -1 };
	int rectMoveToIndex{ -1 };
	int enemyIndex{ -1 };
	Utilities::Direction startingDirection = east;
	shared_ptr<BattleEntity> enemy = nullptr;

	shared_ptr<Cell> enemyCell = nullptr;

	shared_ptr<vector<Cell>> grid;
	bool enemyIsAdjacent{ false };

	int numberOfRows{ 0 };
	int numberOfColumns{ 0 };

	Vector2 moveToCenter;
	Vector2 enemyCenter;

	vector<int>movementPath;
	int tempIndex = 0;


	float timer = 0.0f;

	float hitDuration = 0.4f;
	//float attackDuration = 0.0f;

	// the exact second the attackers weapon makes contact with defending enetity
	int momentOfContact = 0;

	float coolOffDuration = 0.3f;

	bool reachedEnemy{ false };

	bool attacker = false;
	bool defender = false;


	bool finishedAttack{ false };

	bool move{ false };

	bool attacking{ false };

	//bool defending{ false };
	bool mouseOver{ false };
	bool lured{ false };

	bool targeted{ false };
	Color targetColor = {0, 255, 255, 255};
	float targetRedValue = 0.0f;
	float t = 0.0f;

	bool getHit{ false };
	bool getHolyHit{ false };
	bool getUp{ false };
	bool transform{ false };
	bool retaliate{ false };
	bool retaliating{ false };
	bool coolOff{ false };
	bool enemyHit{ false };
	bool returning{ false };
	bool dontRetaliate{ false };
	bool animateDeath{ false };
	bool stopAnimating{ false };

	bool ranOutOffHealth{ false };

	bool hadItsTurn{ false };

	bool animationFinished = false;

	float speed = static_cast<float>(BATTLE_SPEED);

	float scalingFactor{ 0.8f };

	//float shadowTimer{ 0.0f };
	//int shadowFrame{ 0 };
	Rectangle shadowRect;

	vector<int> adjacentCellIndexes;
	vector<AdjacentCell> adjacentCells;
	AdjacentCell attackDirection;
	string attackType = "attack";

	int rank;
    int maxHealth;
	int currentHealth;
    int startingAttack;
	int currentAttack;
    int armor;
    int holiness;
	int movementPoints;

	Utilities::EntitySize entitySize;

	vector<int> reachableCells;


	// it holds smart pointers to the BattleEntityies in the opposing army
	vector<shared_ptr<BattleEntity>> opposingArmy;


	//UI
	Texture2D* squareFrame;

	int portraitPosX;
	int portraitPosY;
	int portraitWidth;
	int portraitHeight;

	Rectangle squareFrameSourceRect;

	Vector2 origin;

	Rectangle portraitSource;
	Rectangle portraitDest;
	
	Texture2D* stats_bar;
	Texture2D* HP_bar;
	Texture2D* health100;
	Texture2D* health75;
	Texture2D* health50;
	Texture2D* health25;
	Texture2D* health10;


	// ***Animated Portrait data***
	int animSpeed = 15;
	//shared_ptr<Texture2D> animatedPortrait;

	float animPortraitWidth;
	float animPortraitHeight;

	int animPortCurrentFrame = 0;

	int animPortRow = 0;
	int animPortCol = 0;

	// Portrait Animation Data
	int animPortCols = 10;
	int animPortRows = 6;
	int totalPortraitFrames = animPortCols * animPortRows;

	float portraitFrameTimer = 0.0f;
	int portraitFrameIndex = 0; // 0 to 59

	Texture2D* healthIcon = nullptr;
	Rectangle healthIconSource;
	//Rectangle healthIconDest;
	Rectangle healthIconDest_portrait;

	Vector2 healthPos;

	Texture2D* damageIcon = nullptr;
	Rectangle damageIconSource;
	//Rectangle damageIconDest;
	Rectangle damageIconDest_portrait;
	
	Vector2 damagePos;

	Texture2D* manaCostIcon = nullptr;
	Rectangle manaIconSource;
	Rectangle manaIconDest;

	Vector2 manaCostPos;

	Color healthTextColor = RAYWHITE;
	Color damageTextColor = RAYWHITE;

	int statsFontSize = 30;

	float charSpacing = 2.0f;
	int outlineSize = 2;

	Font customFont = GetFontDefault();

	unique_ptr<AnimatedAsset> healthPopUp;
	string healthModifier;
	Vector2 healthPopUpTextPos;

	unique_ptr<AnimatedAsset> damagePopUp;
	string damageModifier;
	Vector2 damagePopUpTextPos;

	bool skipTurn{ false };
	bool drawTaunt{ true };
	bool taunting{ false };
	bool evade{ false };

	Texture2D* tauntIcon = nullptr;
	Rectangle tauntIconSource;
	Rectangle tauntIconDest;

	Texture2D* evadeIcon = nullptr;
	Rectangle evadeIconSource;
	Rectangle evadeIconDest;

	unique_ptr<AnimatedAsset> tauntAnimatedIcon;

	Texture2D* skipTurnIcon = nullptr;
	Rectangle skipTurnIconSource;
	Rectangle skipTurnIconDest;

	unique_ptr<AnimatedAsset> skipTurnAnimatedIcon;
	
};

