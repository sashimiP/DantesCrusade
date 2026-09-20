#pragma once
#include<functional>

#include "raylib.h"
#include "BattleEntity.h"
#include "FootSoldier.h"
#include "RangedSoldier.h"
#include "MountedSoldier.h"
#include "StrikeAndReturnSoldier.h"
#include "StrikeAndLure.h"
#include "SpiderQueen.h"
#include "Angel.h"
#include "Background.h"
#include "Menu.h"
#include "Portrait.h"
#include "Deck.h"
#include "EnemyDeck.h"
#include "AnimatedAsset.h"
#include "BattleHero.h"
#include "AbilityIcon.h"

#include <random>


//struct Cell {
//	shared_ptr<Rectangle> rect{nullptr};
//	shared_ptr<BattleEntity> entity{ nullptr };
//	int entityHealth{ 0 };
//	bool mouseOverCell{ false };
//	bool selected{ false };
//};

using Utilities::EntityType, Utilities::EntityTypeFromString, Utilities::UI_Asset;
using Utilities::Relationship, Utilities::IsSpellLike, Utilities::EntitySize;

struct EntityMedia
{
	string type;
	MediaStreamPtr media = nullptr;
};

enum class EnemyActionKind { HeroAbility, Debuff, Entity, Buff, None };

struct EnemyAction {
	EnemyActionKind kind = EnemyActionKind::None;
	std::shared_ptr<Card> card;
	BattleEntity* target = nullptr;
	float score = 0.0f;
};

//struct UI_Asset
//{
//	Texture2D* texture;
//	Rectangle source;
//	Rectangle dest;
//};

constexpr int MAX_MANA = 10;
constexpr int MAX_ENTITIES = 6;

class Battleground
{
public:
	Battleground(shared_ptr<TextureManager> textureManager);
	~Battleground() = default;

	void CreateBattleGrid();

	//void SetArmy(const unordered_map<string, float>& armyMap, vector<shared_ptr<BattleEntity>>& army, int i, Utilities::Direction direction, Utilities::Relationship relationship);

	//void SetArmy(const unordered_map<int, string>& armyMap, vector<shared_ptr<BattleEntity>>& army, int i, Utilities::Direction direction, Utilities::Relationship relationship);

	void SetArmy(const vector<string>& armyList, vector<shared_ptr<BattleEntity>>& army, int i, Utilities::Direction direction, Utilities::Relationship relationship);

	void SummonEntityWithEffect(string type, vector<shared_ptr<BattleEntity>>& army, int i, Utilities::Direction direction, Utilities::Relationship relationship);

	void SummonEntityWithEffectSetFlag(string type, vector<shared_ptr<BattleEntity>>& army, int i, Utilities::Direction direction, Utilities::Relationship relationship, bool* flag);

	void AddEntity(string type, vector<shared_ptr<BattleEntity>>& army, int gridPos, Utilities::Direction direction, Utilities::Relationship relationship);

	int FindAvailableCell(int gridPos, Utilities::EntitySize entitySize, Utilities::Relationship relationship);
	// Give each BattleEnitity a vector of its opponent's entire army
	void SetOpposingArmies();
	//void SetBattleground(const unordered_map<string, float>& army, const unordered_map<string, float>& enemyArmy);

	//void SetBattleground(const unordered_map<int, string>& army, const unordered_map<int, string>& enemyArmy);

	void SetBattleground(
		const vector<string>& army,
		const vector<string>& enemyArmy
	);

	void SetBattleground(
		const vector<string>& army,
		const vector<string>& armySpells,
		const vector<string>& enemyArmy,
		const vector<string>& enemyArmySpells
	);

	void Update();
	void UpdateArmies();
	void RightMouseButtonReleased(const Vector2& mouse);
	void LeftMouseButtonReleased(const Vector2& mouse);
	void TryPlaySpell(const shared_ptr<Card>& card); 
	void TryPlaySpellOn(const shared_ptr<Card>& card, BattleEntity* target);
	void LeftMouseButtonDown();
	void LeftMouseButtonPressed();

	void DeSelectOthers(const BattleEntity& selectedEntity);
	void UpdateAttackCursor(BattleEntity& entity);
	void ResetCursor();
	void DeselectEntity();
	bool TauntEnemyPresent() const;
	
	void PopulateManaStones(vector<UI_Asset>& manaStones, Rectangle destination, Texture2D* texture);

	void LeftMouseButtonReleased();
	
	void LoadPortraits(const char*, vector<Portrait>&);
	void CreatePortraitGrid(const map <string, Texture2D>&, vector<Portrait>&);
	//void SelectPortrait(const vector<Portrait>& Portrait);

	void AnimateCardDraw(Rectangle& cardBackTextureDest, Rectangle cardDeckDest, Vector2 moveToCenter, Deck* cardDeck, bool& endTurnButtonPressed);
	void UpdateDeckEmptyFlag(Deck* cardDeck);
	void SetLastCardFlag(Deck* cardDeck);

	void StartShake(float duration = 0.35f, float strength = 18.0f);
	void ShakeScreen();

	void DrawShadows(float dt);
	void DrawBackground();
	void DrawGrid();
	void DrawArmies(float dt);
	void DrawAttackCursor();
	void DrawUI();
	void DisplayCards();
	void DrawMenu();

	void DisplayPlayerWon();
	void DisplayPlayerLost();

	void DrawManaStones(vector<UI_Asset> manaStones, int currentMana, int usedMana);
	void DrawManaCostLight();

	void EnemyDisplayChosenCard();
	void ActivateDisplayChosenCard(shared_ptr<Card>card, function<void()>onDone);

	void Gameplay();
	void EnemyPlayCards();
	void EnemyPlayEntity();

	void EnemyPlayBuffSpell();
	void EnemyPlayDebuffSpell();
	void EnemyHeroPlayAbility();
	void EnemyHeroPlayAbility(BattleEntity* target);
	void EnemyHeroSummonEntity(string type, vector<shared_ptr<BattleEntity>>& army, int i, Utilities::Direction direction, Utilities::Relationship relationship);

	BattleEntity* PickSpellTarget(Card& card);
	BattleEntity* PickEnemyHeroAbilityTarget();

	float ScoreEntity(const Card& c, int manaLeft, int armySize);
	float ScoreDebuff(const Card& c, BattleEntity* target, int manaLeft);
	float ScoreBuff(const Card& c, BattleEntity* target, int manaLeft);
	float ScoreHeroAbility(BattleEntity* target, int cost, int manaLeft);
	BattleEntity* BestIn(const std::vector<std::shared_ptr<BattleEntity>>& pool,
		const std::function<float(BattleEntity*)>& scoreFn);
	EnemyAction PickEnemyPlay(int mana);

	void EnemyBattlePhase();
	vector<shared_ptr<BattleEntity>> EnemyTargets();
	float ScoreAttackTarget(BattleEntity& attacker, BattleEntity& target);
	shared_ptr<BattleEntity> PickAttackTarget(const std::shared_ptr<BattleEntity>& attacker);

	void EnemyEndTurn();
	void ResetSkipTurnState(vector<shared_ptr<BattleEntity>>& army);
	void ResetHadItsTurnState(vector<shared_ptr<BattleEntity>>& army);
	void ShuffleTurnPanel();
	void ClearDeadEntities(vector<shared_ptr<BattleEntity>>& army);

	void SetBackground(string background);
	void SetBackground(Background& background);
	void ShowMenu();

	void SetBattleMode(bool* battleMode, function<void()> callback = nullptr);

	bool IsSpellSeekingTarget();



private:

	vector<EntityMedia> battleEntityPortraits;
	vector<string> availableUnitsMedia;
	bool drawUnitMedia{ false };
	int currentUnit{ 0 };
	int maxUnits{ 0 };
	int alliedGridPos;
	int enemyGridPos;

	bool  shakeScreen = false;
	float shakeTime = 0.0f;
	float shakeDuration = 0.35f;
	float shakeStrength = 18.0f;   // pixels
	Vector2 bgHome{0.0f, 0.0f};


	//Cards and Deck
	unique_ptr <Deck> cardDeck;
	unique_ptr <EnemyDeck> enemyCardDeck;

	shared_ptr<Card> selectedCard;
	Rectangle cardPlacementZone;

	Button endTurnButton{ "End Turn", 30 };
	bool endTurnButtonPressed{ false };
	bool endTurnButtonPressed_enemy{ false };


	shared_ptr<AnimatedAsset> spell;


	// Battlemode UI and Menu
	Rectangle portraitSource;
	Rectangle portraitDest;

	Button nextUnitLeft{ "<=", 20 };
	Button nextUnitRight{ "=>", 20 };
	Button closeSelection{ "Back", 20 };
	Button addUnit{ "Add Unit", 20 };

	Menu menu;
	Button showMenuButton{ "Menu", 20 };
	bool showMenu_{ false };
	bool createArmy_{ false };
	bool createEnemyArmy_{ false };
	bool* battleMode_ = nullptr;

	function<void()> backToEditorCallback;

	Color basicCellColor = BATTLE_CELL_CLR;
	Color mouseOverCellColor = BATTLE_CELL_MOUSE_OVER_CLR;

	Texture2D* attackCursor = nullptr;
	Rectangle attackCursorSource;
	Rectangle attackCursorDest;
	float attackCursorRotation;
	Vector2 attackCursorOrigin;

	int numRows;
	int numColumns;

	shared_ptr<TextureManager> textureManager;
	shared_ptr<vector<Cell>> grid;
	vector<shared_ptr<Rectangle>> gridRects;
	float cellWidth = BATTLE_CELL_WIDTH;
	float cellHeight = BATTLE_CELL_HEIGHT;

	std::unique_ptr<Background> background;
	//Background* background = nullptr;

	unordered_map<string, float> countArmy;
	unordered_map<string, float> countEnemyArmy;

	/*unordered_map<int, string> armyMap;
	unordered_map<int, string> enemyArmyMap;*/

	vector<string> armyList;
	vector<string> enemyArmyList;

	vector<string> armySpells;
	vector<string> enemySpells;

	vector<shared_ptr<BattleEntity>> army;
	vector<shared_ptr<BattleEntity>> enemyArmy;
	vector<shared_ptr<BattleEntity>> battleEntities;

	shared_ptr<BattleEntity> danteHero;
	shared_ptr<BattleEntity> enemyHero;

	int heroGridPos;
	int enemyHeroGridPos;

	bool danteHeroAbilityActive{ false };

	bool mouseOverCell{ false };
	bool hideCursor{ false };
	bool showCursor{ true };
	BattleEntity* hoveredEntity = nullptr;
	shared_ptr<BattleEntity> selectedEntity;
	
	int selectedEntityIndex{ -1 };
	int defendingEntityIndex{ -1 };
	int attackingEntityIndex{ -1 };

	shared_ptr<BattleEntity> attacker = nullptr;
	shared_ptr<BattleEntity> defender = nullptr;

	int turnsCounter{ 0 };

	// GAMEPLAY VARIABLES

	int turn = 1;

	int currentMana = 1;
	int usedMana = 0;
	int availableMana = currentMana;

	int currentMana_enemy = 0;
	int usedMana_enemy = 0;
	int availableMana_enemy = currentMana_enemy;

	//EnemyCardPhase enemyCardPhase = EnemyCardPhase::DrawWait;

	float enemyTimer{ 1.5f };

	EnemyAction plannedPlay;
	bool hasPlan = false;
	
	bool cardFinishedDrawing{ false };
	float cardDrawnTimer{ 1.5f };

	bool enemyTurn{ false };

	bool enemyFinishedPlayingCards{ false };
	bool enemyFinishedPlayingEntity{ false };
	bool enemyFinishedPlayingDebuff{ false };

	bool enemyFinishedPlayingSpells{ false };
	bool enemyHeroPlayAbility{ false };

	bool enemySummoningEntity{ false };
	bool enemyCastingSpell{ false };

	bool enemyDisplayChosenCard{ false };
	float enemyDisplayCardTimer{ 0.f };
	shared_ptr<Card> chosenCard;
	function<void()> chosenCardCallback = nullptr;

	float finishedPlayingCardsTimer{ 2.f };

	bool beginEnemyBattlePhase{ false };

	bool enemyFinishedTurn{ false };

	float showPlayerTurnBanner{ 2.0f };
	bool showBanner{ false };

	int targetIndex{ 0 };
	int currentAttackerIndex{ 0 };
	
	Texture2D* playerTurnBanner{ nullptr };
	Rectangle playerTurnBannerSource;
	Rectangle playerTurnBannerDest;


	// UI variables

	bool playerWon{ false };
	bool playerLost{ false };

	AnimatedAsset* playerWonBannerFireworks = nullptr;
	Texture2D* playerWonBanner = nullptr;
	Texture2D* playerLostBanner = nullptr;
	Texture2D* playerLostScreen = nullptr;

	Rectangle playerWonBannerSource;
	Rectangle playerLostBannerSource;

	bool drawGrid{ true };

	Color usedManaColor = { 0, 0, 0, 150 };

	int rightSquareFrameWidth;
	int rightSquareFrameHeight;
	Rectangle leftPortraitDestRect;

	int squareFrameWidth;
	int squareFrameHeight;

	int squareFramePosX;
	int topFramePosY;
	int bottomFramePosY;

	vector<shared_ptr<BattleEntity>>enemyTurnPanel;

	Texture2D* squareFrame;

	Rectangle squareFrameSourceRect;

	Rectangle topSquareFrameDestRect;
	Rectangle bottomSquareFrameDestRect;
	Rectangle portraitsZone;

	Vector2 origin;

	bool deckEmpty{ false };
	bool enemyDeckEmpty{ false };

	bool lastCard{ false };
	bool lastCardEnemy{ false };

	Texture2D* deckTexture{nullptr};
	Texture2D* enemyDeckTexture{ nullptr };
	Texture2D* cardBackTexture{ nullptr };

	Rectangle deckTextureSource;
	Rectangle enemyDeckTextureSource;
	Rectangle cardBackTextureSource;

	Rectangle alliedDeckTextureDest;
	Rectangle enemyDeckTextureDest;
	Rectangle cardBackTextureDest;

	//float cardBackBaseX;
	//float cardBackBaseY;
	Rectangle cardBackMoveToBox;
	Vector2 moveToCenter;

	Rectangle cardBackTextureSource_enemy;
	Rectangle cardBackTextureDest_enemy;

	//float cardBackBaseX_enemy;
	//float cardBackBaseY_enemy;
	Rectangle cardBackMoveToBox_enemy;
	Vector2 moveToCenter_enemy;

	Texture2D* manaBanner{ nullptr };
	Rectangle manaBannerTextureSource;

	Rectangle manaBannerTextureDest;
	Rectangle manaBannerTextureDest_enemy;

	Texture2D* manaStone{ nullptr };
	Rectangle manaStoneTextureSource;

	Rectangle manaStoneTextureDest;
	Rectangle manaStoneTextureDest_enemy;

	vector<UI_Asset> manaStones;
	vector<UI_Asset> manaStones_enemy;

	AnimatedAsset* manaShimmer = nullptr;
	AnimatedAsset* manaShimmer_enemy = nullptr;
	bool showManaShimmer = false;

	Texture2D* manaCostTexture{ nullptr };
	vector<UI_Asset> manaCostLights;

	AnimatedAsset* round_sparkle_burst = nullptr;

};

