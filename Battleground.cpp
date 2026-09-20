#include "Battleground.h"

// Helper function
void sortByPosition(vector<shared_ptr<BattleEntity>>& entities)
{
	std::sort(entities.begin(), entities.end(),
		[](const auto& a, const auto& b) {
			return a->Position() < b->Position();
		});
}


Battleground::Battleground(shared_ptr<TextureManager> textureManager):
	textureManager(textureManager),
	menu({ "create_army", "create_enemy_army", "shake_screen", "grid", "show_enemy_hand", "back", "exit_battle_mode" })

{
	numRows = std::floor((SCR_HEIGHT / cellHeight)) - 4;
	numColumns = (SCR_WIDTH / cellWidth)-9; // if you change number of columns, also change them in BattleEntity

	alliedGridPos =  1;
	enemyGridPos = numColumns - 2;

	heroGridPos = 0;
	enemyHeroGridPos = numColumns - 1;

	cout << "ROWS: "<< numRows << " COLUMNS: " << numColumns << endl;
	grid = std::make_shared<std::vector<Cell>>();
	background = std::make_unique<Background>(textureManager, GLOBAL_SPEED, fs::path());
	background->LoadLayers("dark_forest_battle");

	CreateBattleGrid();

	grid->at(heroGridPos).color = BATTLE_CELL_MOUSE_OVER_CLR;
	grid->at(enemyHeroGridPos).color = BATTLE_CELL_MOUSE_OVER_CLR;

	enemyTurnPanel.reserve(6);

	// UI
	squareFrame = this->textureManager->Asset("square_frame");

	squareFrameWidth = 200.f;
	squareFrameHeight = 360.f;

	squareFramePosX = 0;
	topFramePosY = 0;
	bottomFramePosY = squareFrameHeight;


	//entityTurnPanels.reserve(12); // reserve the total number of portraits (six entities per army)

	
	squareFrameSourceRect = Rectangle{ 0.0f, 0.0f, static_cast<float>(squareFrame->width), static_cast<float>(squareFrame->height) };
	topSquareFrameDestRect = Rectangle{ static_cast<float>(squareFramePosX), static_cast<float>(topFramePosY), static_cast<float>(squareFrameWidth), static_cast<float>(squareFrameHeight) };
	bottomSquareFrameDestRect = Rectangle{ static_cast<float>(squareFramePosX), static_cast<float>(bottomFramePosY), static_cast<float>(squareFrameWidth), static_cast<float>(squareFrameHeight) };

	portraitsZone = {
		topSquareFrameDestRect.x,
		topSquareFrameDestRect.y,
		topSquareFrameDestRect.width,
		topSquareFrameDestRect.height + bottomSquareFrameDestRect.height
	};

	leftPortraitDestRect = {
		SCR_WIDTH - 64.0f,
		0.f,
		64.0,
		64.0
	};

	origin = { 0.0f, 0.0f };

	// Buttons
	showMenuButton.SetPosition(SCR_WIDTH / 2, 0);
	showMenuButton.onClick = [&]()
		{
			ShowMenu();
		};

	nextUnitRight.onClick = [&]()
		{
			SetMediaPosition(*battleEntityPortraits[currentUnit].media, 0.0);
			SetMediaState(*battleEntityPortraits[currentUnit].media, MEDIA_STATE_STOPPED);
			if (currentUnit+1 > maxUnits)
				currentUnit = 0;
			else
				currentUnit++;
			cout << "CURRENT UNIT: " << currentUnit << endl;
		};

	nextUnitLeft.onClick = [&]()
		{
			SetMediaPosition(*battleEntityPortraits[currentUnit].media, 0.0);
			SetMediaState(*battleEntityPortraits[currentUnit].media, MEDIA_STATE_STOPPED);
			if (currentUnit-1 < 0)
				currentUnit = maxUnits;
			else
				currentUnit--;
			cout << "CURRENT UNIT: " << currentUnit << endl;
		};

	closeSelection.onClick = [&]()
		{
			SetMediaPosition(*battleEntityPortraits[currentUnit].media, 0.0);
			SetMediaState(*battleEntityPortraits[currentUnit].media, MEDIA_STATE_STOPPED);
			drawUnitMedia = false;
			createArmy_ = false;
			createEnemyArmy_ = false;
		};

	addUnit.onClick = [&]()
		{
			if (enemyTurn) return;

			string& entityType = battleEntityPortraits[currentUnit].type;

			if (createArmy_)
			{
				SummonEntityWithEffect(entityType, this->army, alliedGridPos,
					east, Utilities::Relationship::Ally);
			}
			else if(createEnemyArmy_)
				SummonEntityWithEffect(entityType, this->enemyArmy, enemyGridPos, west, Utilities::Relationship::Enemy);
		};

	menu.CreateGrid(showMenuButton.GetRect().x, showMenuButton.GetRect().y);
	menu.CreateOptionsCells();

	
	//Cards and Deck
	cardDeck = std::make_unique<Deck>(textureManager);
	enemyCardDeck = std::make_unique<EnemyDeck>(textureManager);
	cardPlacementZone = { SCR_WIDTH / 4, SCR_HEIGHT / 4, cellWidth * 4, cellHeight * (numRows - 1)};

	endTurnButton.onClick = [&]()
		{

			dynamic_cast<BattleHero*>(danteHero.get())->ResetAbilityUsed();
			endTurnButtonPressed = true;
			enemyTurn = true;
			enemyTimer = 1.5f;
			if (currentMana_enemy < MAX_MANA)
			{
				currentMana_enemy++;
				float x = manaStones_enemy[currentMana_enemy - 1].dest.x + manaStones_enemy[currentMana_enemy - 1].dest.width / 2 - manaShimmer->GetDest().width / 2;
				float y = manaStones_enemy[currentMana_enemy - 1].dest.y + manaStones_enemy[currentMana_enemy - 1].dest.height / 2 - manaShimmer->GetDest().height / 2;
				manaShimmer->SetPosition(x, y);
				manaShimmer->SetDrawOnce(true);
			}
			usedMana_enemy = 0;
			availableMana_enemy = currentMana_enemy;
			cardFinishedDrawing = false;
			enemyDeckEmpty = enemyCardDeck->DeckEmpty();
			attacker.reset();
			defender.reset();
			selectedEntity.reset();
			selectedEntityIndex = -1;
			ClearDeadEntities(army);
			ClearDeadEntities(enemyArmy);
			enemyTurnPanel.clear();
			ResetHadItsTurnState(enemyArmy);
			
			cout << "ENEMIES IN TURN PANEL:" << endl;
			for (auto& enemy : enemyArmy)
			{
				if (enemy->GetSkipTurn())
					continue;
				cout << enemy->GetType() << endl;
				enemyTurnPanel.push_back(enemy);
			}

			ShuffleTurnPanel();

			turnsCounter = 0;
			ResetSkipTurnState(army);

			enemyFinishedPlayingCards = false;
			enemyFinishedPlayingEntity = false ;
			enemyFinishedPlayingSpells = false;
			enemyFinishedPlayingDebuff = false;
			enemyCardDeck->StartLooking();
			enemyFinishedTurn = false;
			beginEnemyBattlePhase = false;
			finishedPlayingCardsTimer = 2.f;

			hasPlan = false;

			srand(time(0));
			enemyHeroPlayAbility = 0.3 > ((double)rand()) / INT_MAX;

			if (enemyHeroPlayAbility)
				cout << "CHOOSE PLAY EHEMY HERO ABILITY" << endl;

			std::cout << "start look iter=" << enemyCardDeck->GetIterCounter()
				<< " hand=" << enemyCardDeck->GetHandSize()
				<< " finished=" << enemyCardDeck->FinishedLooking() << '\n';
			/*showManaShimmer = true;*/
		};

	endTurnButton.SetPosition(SCR_WIDTH - endTurnButton.GetRect().width, SCR_HEIGHT / 2);

	selectedCard = make_shared<Card>();
	deckTexture = this->textureManager->Asset("deck");
	enemyDeckTexture = this->textureManager->Asset("enemy_deck");
	cardBackTexture = this->textureManager->Asset("card_back");

	deckTextureSource = { 0.0f, 0.0f, static_cast<float>(deckTexture->width), static_cast<float>(deckTexture->height) };

	enemyDeckTextureSource = { 0.0f,
		0.0f,
		static_cast<float>(enemyDeckTexture->width),
		static_cast<float>(enemyDeckTexture->height)
	};

	float scale = std::min(
		120.f / deckTextureSource.width,
		180.f / deckTextureSource.height
	);

	float x = endTurnButton.GetRect().x;
	float y = endTurnButton.GetRect().y + endTurnButton.GetRect().height + 10.0f;
	float width = deckTexture->width * scale;
	float height = deckTexture->height * scale;

	alliedDeckTextureDest = { x, y, width, height };

	y = endTurnButton.GetRect().y - height - 10.0f;

	enemyDeckTextureDest = { x, y, width, height };


	cardBackTextureSource = { 0.0f,
		0.0f,
		static_cast<float>(cardBackTexture->width),
		static_cast<float>(cardBackTexture->height)
	};

	y = endTurnButton.GetRect().y + endTurnButton.GetRect().height + 10.0f;
	width = cardBackTexture->width * scale;
	height = cardBackTexture->height * scale;

	cardBackTextureDest = { x, y, width, height };

	//cardBackBaseX = cardBackTextureDest.x;
	//cardBackBaseY = cardBackTextureDest.y;

	x = SCR_WIDTH - 240.f * 2;
	y = SCR_HEIGHT / 2 - 180.f;
	cardBackMoveToBox = { x, y, 240.f, 360.f };

	moveToCenter = {
		cardBackMoveToBox.x + cardBackMoveToBox.width / 2.0f,
		cardBackMoveToBox.y + cardBackMoveToBox.height - cardBackTextureDest.height / 2.0f
	};

	x = endTurnButton.GetRect().x;
	y = endTurnButton.GetRect().y - height - 10.0f;

	cardBackTextureSource_enemy = { 0.0f,
		0.0f,
		static_cast<float>(cardBackTexture->width),
		-static_cast<float>(cardBackTexture->height)
	};

	cardBackTextureDest_enemy = { x, y, width, height };
	//cardBackBaseX_enemy = cardBackTextureDest.x;
	//cardBackBaseY_enemy = cardBackTextureDest.y;

	x = SCR_WIDTH * 0.66f;
	y = 90.f;

	cardBackMoveToBox_enemy = { x, y, 120.f, 180.f };

	moveToCenter_enemy = {
		cardBackMoveToBox_enemy.x + cardBackMoveToBox_enemy.width / 2.0f,
		cardBackMoveToBox_enemy.y + cardBackMoveToBox_enemy.height - cardBackTextureDest.height / 2.0f
	};

	manaCostTexture = textureManager->Asset("mana_cost_light");
	manaStone = textureManager->Asset("mana_stone");
	
	manaStoneTextureSource = {
		0.0f,
		0.0f,
		static_cast<float>(manaStone->width),
		static_cast<float>(manaStone->height)
	};

	manaBanner = textureManager->Asset("mana_banner");
	manaBannerTextureSource = {
		0.0f,
		0.0f,
		static_cast<float>(manaBanner->width),
		static_cast<float>(manaBanner->height)
	};

	width = manaStoneTextureSource.width * 12 + 10*2;

	manaBannerTextureDest = {
		SCR_WIDTH - width,
		alliedDeckTextureDest.y + alliedDeckTextureDest.height + 10.f, 
		width,
		manaBannerTextureSource.height * 2
	};

	manaBannerTextureDest_enemy = {
		SCR_WIDTH - width,
		enemyDeckTextureDest.y - manaBannerTextureSource.height * 2 - 10.f,
		width,
		manaBannerTextureSource.height * 2
	};

	width = manaStoneTextureSource.width;
	height = manaStoneTextureSource.height;

	manaStoneTextureDest = {
		manaBannerTextureDest.x + width,
		manaBannerTextureDest.y + manaBannerTextureDest.height/2 - height/2,
		manaStoneTextureSource.width,
		manaStoneTextureSource.height
	};

	manaStoneTextureDest_enemy = {
		manaBannerTextureDest_enemy.x + width,
		manaBannerTextureDest_enemy.y + manaBannerTextureDest_enemy.height / 2 - height / 2,
		manaStoneTextureSource.width,
		manaStoneTextureSource.height
	};


	playerTurnBanner = textureManager->Asset("player_turn_banner");
	playerTurnBannerSource = { 
		0.0f,
		0.0f,
		static_cast<float>(playerTurnBanner->width),
		static_cast<float>(playerTurnBanner->height)
	};

	playerTurnBannerDest = {
		SCR_WIDTH * 0.5f - playerTurnBannerSource.width * 0.5f,
		SCR_HEIGHT * 0.5f - playerTurnBannerSource.height * 0.5f,
		playerTurnBannerSource.width,
		playerTurnBannerSource.height
	};

	playerWonBanner = textureManager->Asset("player_won_banner");
	playerWonBannerSource = {
		0.0f,
		0.0f,
		static_cast<float>(playerWonBanner->width),
		static_cast<float>(playerWonBanner->height)
	};

	playerLostBanner = textureManager->Asset("player_lost_banner");
	playerLostBannerSource = {
		0.0f,
		0.0f,
		static_cast<float>(playerLostBanner->width),
		static_cast<float>(playerLostBanner->height)
	};

	playerLostScreen = textureManager->Asset("player_lost_screen");

	playerWonBannerFireworks = new AnimatedAsset("player_won_banner_fireworks", 31, textureManager);
	playerWonBannerFireworks->SetDest(playerTurnBannerDest);
	
	/*for (int i = 0; i < MAX_MANA; i++)
	{
		Rectangle dest = {
			manaStoneTextureDest.x + (2 + width) * i,
			manaStoneTextureDest.y,
			manaStoneTextureDest.width,
			manaStoneTextureDest.height
		};

		manaStones.emplace_back(manaStone, manaStoneTextureSource, dest);
	}*/

	PopulateManaStones(manaStones, manaStoneTextureDest, manaStone);
	PopulateManaStones(manaCostLights, manaStoneTextureDest, manaCostTexture);
	PopulateManaStones(manaStones_enemy, manaStoneTextureDest_enemy, manaStone);

	manaShimmer = new AnimatedAsset("mana_shimmer", 9, textureManager);

	round_sparkle_burst = new AnimatedAsset("round_sparkle_burst", 17, textureManager);
}

void Battleground::CreateBattleGrid()
{
	float x{ cellWidth};
	float y{ SCR_HEIGHT/4};
	int gridIndex = 0;
	for (int i{}; i < numRows; i++)
	{
		x = SCR_WIDTH/4;
		for (int j{}; j < numColumns; j++)
		{
			//Rectangle cell = { x,y,squareWidth, squareHeight };

			grid->emplace_back(gridIndex++, make_shared<Rectangle>(x, y, cellWidth, cellHeight));
            gridRects.push_back(grid->back().rect);
			x += cellWidth;

		}
		y += cellHeight;
	}
}

shared_ptr<BattleEntity> CreateEntity(Utilities::Relationship relationship,
	const string& type,
	shared_ptr<TextureManager> textureManager,
	Utilities::Direction direction,
	int cellIndex,
	shared_ptr<vector<Cell>> grid)
{
	int color = 0;
	
	EntityType entityType = EntityTypeFromString(type);
	string projectile_type = Utilities::projectileTypes[entityType];
	/*float& attackDuration = Utilities::attackDurations[entityType];*/

	if (entityType == EntityType::SuccubusWhip ||
		entityType == EntityType::Gorgon ||
		entityType == EntityType::SuccubusBlue ||
		entityType == EntityType::SuccubusGolden ||
		entityType == EntityType::SuccubusRed ||
		entityType == EntityType::SuccubusWizard ||
		entityType == EntityType::VampireLady ||
		entityType == EntityType::SpiderQueen ||
		entityType == EntityType::BluePixie ||
		entityType == EntityType::Angel ||
		entityType == EntityType::CentaurBow ||
		entityType == EntityType::ElfBow||
		entityType == EntityType::DarkSword)
	{
		color = 0;
	}

	//if (entityType == EntityType::SuccubusWizard)
	//{
	//	color = 1;
	//}

	switch (entityType)
	{
	//case EntityType::Knight:
	//case EntityType::HeavyKnight:
	//case EntityType::SuccubusWhip:
	//case EntityType::VampireLady:
	//case EntityType::ElfSword:
	//case EntityType::ElfSpear:
	//case EntityType::DarkSword:
	//case EntityType::Imp:
	//case EntityType::Griffin:
	//

	//	return make_shared<FootSoldier>(relationship, attackDuration, cellIndex, grid, fs::path(), std::string(), textureManager, 0.0f, 3,
	//		type, 0.0f, 0.0f, direction,
	//		false, "idle", std::vector<std::string>(), "Unknown", color);

	case EntityType::Archer:
	case EntityType::SuccubusWizard:
	case EntityType::BlueWizard:

		return make_shared<RangedSoldier>(projectile_type, 0.8f, relationship, cellIndex, grid, "none", "none", textureManager, 0.0f, 3,
			type, 0.0f, 0.0f, direction,
			false, "idle", std::vector<std::string>(), "Unknown", color);

	case EntityType::Gorgon:
	case EntityType::BluePixie:
	case EntityType::CentaurBow:
	case EntityType::ElfBow:
	case EntityType::Witch:
	case EntityType::ShadowPriest:
	

		return make_shared<RangedSoldier>(projectile_type, 1.f, relationship, cellIndex, grid, "none", "none", textureManager, 0.0f, 3,
			type, 0.0f, 0.0f, direction,
			false, "idle", std::vector<std::string>(), "Unknown", color);

	case EntityType::Angel:
		return make_shared<Angel>(projectile_type, 1.f, relationship, cellIndex, grid, "none", "none", textureManager, 0.0f, 3,
			type, 0.0f, 0.0f, direction,
			false, "idle", std::vector<std::string>(), "Unknown", color);
	case EntityType::MountedKnight:
	

		return make_shared<MountedSoldier>(relationship, cellIndex, grid, "none", "none", textureManager, 0.0f, 3,
			type, 0.0f, 0.0f, direction,
			false, "idle", std::vector<std::string>(), "Unknown", color);

	case EntityType::SuccubusRed:
	case EntityType::SuccubusGolden:
	case EntityType::Knight:
	case EntityType::HeavyKnight:
	case EntityType::SuccubusWhip:
	case EntityType::VampireLady:
	case EntityType::ElfSword:
	case EntityType::ElfSpear:
	case EntityType::DarkSword:
	case EntityType::Imp:
	case EntityType::Griffin:
	case EntityType::GreenGoblin:
	case EntityType::FieldMaid:
	case EntityType::FencingMaiden:
	case EntityType::BattleBride:
	case EntityType::BlueGolem:


		return make_shared<StrikeAndReturnSoldier>(relationship, cellIndex, grid, "none", "none", textureManager, 0.0f, 3,
			type, 0.0f, 0.0f, direction,
			false, "idle", std::vector<std::string>(), "Unknown", color);

	case EntityType::DanteHero:
	case EntityType::EnemyHero:
		return make_shared<BattleHero>(relationship, cellIndex, grid, "none", "none", textureManager, 0.0f, 3,
			type, 0.0f, 0.0f, direction,
			false, "idle", std::vector<std::string>(), "Unknown", color);


	case EntityType::SuccubusBlue:

		return make_shared<StrikeAndLure>(relationship, cellIndex, grid, "none", "none", textureManager, 0.0f, 3,
			type, 0.0f, 0.0f, direction,
			false, "idle", std::vector<std::string>(), "Unknown", color);

	case EntityType::SpiderQueen:

		return make_shared<SpiderQueen>(relationship, cellIndex, grid, "none", "none", textureManager, 0.0f, 3,
			type, 0.0f, 0.0f, direction,
			false, "idle", std::vector<std::string>(), "Unknown", color);
	default:

		//return make_shared<FootSoldier>(relationship, attackDuration, cellIndex, grid, fs::path(), std::string(), textureManager, 0.0f, 3,
		//	type, 0.0f, 0.0f, direction,
		//	false, "idle", std::vector<std::string>(), "Unknown", color);

		return make_shared<StrikeAndReturnSoldier>(relationship, cellIndex, grid, "none", "none", textureManager, 0.0f, 3,
			type, 0.0f, 0.0f, direction,
			false, "idle", std::vector<std::string>(), "Unknown", color);

		//return nullptr;	
	}
}

void Battleground::SetArmy(const vector<string>& armyList, vector<shared_ptr<BattleEntity>>& army, int i, Utilities::Direction direction, Utilities::Relationship relationship)
{
	if (armyList.empty()) return;
	for (auto& entity : armyList)
	{
		auto soldier = CreateEntity(relationship, entity, textureManager, direction, i, grid);
		cout << soldier->Type() << endl;
		soldier->SetPosition(i);
		army.emplace_back(soldier);
		//entityTurnPanels.emplace_back(soldier);
		if (i < 0 || i >= static_cast<int>(grid->size()))
		{
			std::cerr << "SetArmy ERROR: grid index out of range: " << i
				<< " grid size=" << grid->size() << "\n";
			break;
		}
		grid->at(i).entity = soldier;
		grid->at(i).entityHealth = soldier->GetHealth();
		grid->at(i).entityType = EntityTypeFromString(soldier->Type());
		battleEntities.push_back(army[army.size() - 1]);
		i += numColumns;
	}
}

//void Battleground::SetOpposingArmies()
//{
//	for (auto& entity : army)
//		entity->SetOpposingArmy(enemyArmy);
//
//	for(auto& enemyEntity: enemyArmy)
//		enemyEntity->SetOpposingArmy(army);
//
//
//}

void Battleground::SummonEntityWithEffect(string type, vector<shared_ptr<BattleEntity>>& army, int i, Utilities::Direction direction, Utilities::Relationship relationship)
{
	auto& entitySize = entityStats[type].size;
	int gridPos = FindAvailableCell(i, entitySize, relationship);

	round_sparkle_burst->SetPosition(
		grid->at(gridPos).rect->x - grid->at(gridPos).rect->width / 2,
		grid->at(gridPos).rect->y - grid->at(gridPos).rect->height / 2
	);

	round_sparkle_burst->SetDrawOnce(true, 0.0f, [this, type, &army, gridPos, direction, relationship, entitySize]()
		{
			AddEntity(type, army, gridPos,
				direction, relationship);
			if (entitySize == EntitySize::Big || entityStats[type].rank >= 6)
				StartShake(0.4f, 25.0f);
		});
}

void Battleground::SummonEntityWithEffectSetFlag(string type, vector<shared_ptr<BattleEntity>>& army, int i, Utilities::Direction direction, Utilities::Relationship relationship, bool* flag)
{
	auto& entitySize = entityStats[type].size;
	int gridPos = FindAvailableCell(i, entitySize, relationship);
	
	round_sparkle_burst->SetPosition(
		grid->at(gridPos).rect->x - grid->at(gridPos).rect->width / 2,
		grid->at(gridPos).rect->y - grid->at(gridPos).rect->height / 2
	);

	round_sparkle_burst->SetDrawOnce(true, 0.0f, [this, flag, type, &army, gridPos, direction, relationship]()
		{
			AddEntity(type, army, gridPos,
				direction, relationship);
			*flag = true;
		});
}

void Battleground::AddEntity(string type, vector<shared_ptr<BattleEntity>>& army, int gridPos, Utilities::Direction direction, Utilities::Relationship relationship)
{
	if (army.size() == MAX_ENTITIES)
		return;


	auto soldier = CreateEntity(relationship, type, textureManager, direction, gridPos, grid);
	cout << soldier->Type() << endl;
	soldier->SetPosition(gridPos);
	army.emplace_back(soldier);
	//entityTurnPanels.emplace_back(soldier);
	grid->at(gridPos).entity = soldier;
	grid->at(gridPos).entityHealth = soldier->GetHealth();
	grid->at(gridPos).entityType = EntityTypeFromString(soldier->Type());
	battleEntities.push_back(army[army.size() - 1]);

	//grid->at(gridPos).color = YELLOW;

	if (army.size() >= MAX_ENTITIES-1)
	{
		int target = heroGridPos;
		std::stable_partition(battleEntities.begin(), battleEntities.end(), [target](shared_ptr<BattleEntity> entity) {
			return entity->GetCellIndex() != target;
			});
	}

	/*if (enemyArmy.size() >= MAX_ENTITIES-1)
	{
		int target = enemyHeroGridPos;
		std::stable_partition(battleEntities.begin(), battleEntities.end(), [target](shared_ptr<BattleEntity> entity) {
			return entity->GetCellIndex() != target;
			});
	}*/

}

int Battleground::FindAvailableCell(int i, Utilities::EntitySize entitySize, Utilities::Relationship relationship)
{
	int gridPos = i;

	if (relationship == Utilities::Relationship::Ally)
	{
		for (int j = 0; j < MAX_ENTITIES; j++)
			if (grid->at(gridPos).entity ||
				grid->at(gridPos + 1).entity ||
				grid->at(gridPos + 2).entity)
				gridPos += numColumns;

		cout << "Grid possition: " << gridPos << endl;

		switch (entitySize)
		{
		case Utilities::EntitySize::Medium: gridPos += 1; break;
		case Utilities::EntitySize::Big: gridPos += 0; break;
		default: gridPos += 2; break;
		}
	}
	else if (relationship == Utilities::Relationship::Enemy)
	{
		for (int j = 0; j < MAX_ENTITIES; j++)
			if (grid->at(gridPos).entity ||
				grid->at(gridPos - 1).entity ||
				grid->at(gridPos - 2).entity)
				gridPos += numColumns;

		cout << "Grid possition: " << gridPos << endl;

		switch (entitySize)
		{
		case Utilities::EntitySize::Medium: gridPos -= 1; break;
		case Utilities::EntitySize::Big: gridPos -= 0; break;
		default: gridPos -= 2; break;
		}
	}

	return gridPos;
}


void Battleground::SetBattleground(const vector<string>& army, const vector<string>& enemyArmy)
{
	if (battleMode_)
	{
		availableUnitsMedia = textureManager->GetAvailableUnitsMediaFiles();
		cout << "AVAILABLE BATTLE ENTITIES WITH MEDIA PORTRAITS: " << availableUnitsMedia.size() << endl;
		battleEntityPortraits.reserve(availableUnitsMedia.size());
		maxUnits = availableUnitsMedia.size() - 1;

		for (string& unit : availableUnitsMedia)
		{
			string type = unit.substr(0, unit.size() - 2);
			battleEntityPortraits.emplace_back(EntityMedia{ std::move(type), textureManager->UnitMedia(unit) });
		}
		cout << "AVAILABLE BATTLE ENTITIES WITH MEDIA PORTRAITS: " << battleEntityPortraits.size() << endl;


		auto mediaPortrait = battleEntityPortraits[0].media.get();
		// SAFETY CHECK
		if (mediaPortrait)
		{
			float scale = std::min(
				240.f / (float)mediaPortrait->videoTexture.width,
				360.f / (float)mediaPortrait->videoTexture.height
			);

			//cout << "SET DIALOGUE PLATFORM" << endl;
			portraitSource = {
			0.0f,
			0.0f,
			static_cast<float>(mediaPortrait->videoTexture.width),
			static_cast<float>(mediaPortrait->videoTexture.height)
			};

			float portraitPosX = SCR_WIDTH / 2;
			float portraitPosY = SCR_HEIGHT / 3;

			portraitDest = {
				static_cast<float>(portraitPosX),
				static_cast<float>(portraitPosY),
				-static_cast<float>(mediaPortrait->videoTexture.width * scale),
				static_cast<float>(mediaPortrait->videoTexture.height * scale)
			};

			showMenuButton.SetPosition(SCR_WIDTH - showMenuButton.GetRect().width - 5,
				SCR_HEIGHT - showMenuButton.GetRect().height - 5
			);

			cout << "PORTRAIT WIDTH: " << portraitDest.width << endl;
			nextUnitRight.SetPosition(portraitDest.x - portraitDest.width - nextUnitRight.GetRect().width, portraitDest.y + portraitDest.height);

			nextUnitLeft.SetPosition(portraitDest.x, portraitDest.y + portraitDest.height);

			closeSelection.SetPosition(portraitDest.x - portraitDest.width/2, portraitDest.y - closeSelection.GetRect().height-5);

			addUnit.SetPosition(portraitDest.x + nextUnitLeft.GetRect().width, portraitDest.y + portraitDest.height);
		}
		
	}

	if (!this->army.empty())this->army.clear();
	if (!this->enemyArmy.empty()) this->enemyArmy.clear();
	if (!battleEntities.empty()) battleEntities.clear();
	//if (!entityTurnPanels.empty()) entityTurnPanels.clear();

	for (auto& cell : *grid)
	{
		cell.entity.reset();
		cell.entityHealth = 0;
	}
	cout << "SET BATTLEGROUND, ARMY SIZE IS: " << army.size() << endl;
	this->armyList.reserve(army.size());
	this->enemyArmyList.reserve(enemyArmy.size());

	this->armyList = army;
	this->enemyArmyList = enemyArmy;

	//int i{ 0 };
	//SetArmy(armyList, this->army, i, east, Utilities::Relationship::Ally);


	//i = numColumns - 1;
	//SetArmy(enemyArmyList, this->enemyArmy, i, west, Utilities::Relationship::Enemy);

	//SetOpposingArmies();

	// ShuffleTurnPanel();

	cardDeck->SetDeck(armyList);
	enemyCardDeck->SetDeck(enemyArmyList);
}

void Battleground::SetBattleground(
	const vector<string>& army,
	const vector<string>& armySpells,
	const vector<string>& enemyArmy,
	const vector<string>& enemyArmySpells
)
{
	if (battleMode_)
	{
		availableUnitsMedia = textureManager->GetAvailableUnitsMediaFiles();
		cout << "AVAILABLE BATTLE ENTITIES WITH MEDIA PORTRAITS: " << availableUnitsMedia.size() << endl;
		battleEntityPortraits.reserve(availableUnitsMedia.size());
		maxUnits = availableUnitsMedia.size() - 1;

		for (string& unit : availableUnitsMedia)
		{
			string type = unit.substr(0, unit.size() - 2);
			battleEntityPortraits.emplace_back(EntityMedia{ std::move(type), textureManager->UnitMedia(unit) });
		}
		cout << "AVAILABLE BATTLE ENTITIES WITH MEDIA PORTRAITS: " << battleEntityPortraits.size() << endl;


		auto mediaPortrait = battleEntityPortraits[0].media.get();
		// SAFETY CHECK
		if (mediaPortrait)
		{
			float scale = std::min(
				240.f / (float)mediaPortrait->videoTexture.width,
				360.f / (float)mediaPortrait->videoTexture.height
			);

			//cout << "SET DIALOGUE PLATFORM" << endl;
			portraitSource = {
			0.0f,
			0.0f,
			static_cast<float>(mediaPortrait->videoTexture.width),
			static_cast<float>(mediaPortrait->videoTexture.height)
			};

			float portraitPosX = SCR_WIDTH / 2;
			float portraitPosY = SCR_HEIGHT / 3;

			portraitDest = {
				static_cast<float>(portraitPosX),
				static_cast<float>(portraitPosY),
				-static_cast<float>(mediaPortrait->videoTexture.width * scale),
				static_cast<float>(mediaPortrait->videoTexture.height * scale)
			};

			showMenuButton.SetPosition(SCR_WIDTH - showMenuButton.GetRect().width - 5,
				SCR_HEIGHT - showMenuButton.GetRect().height - 5
			);

			cout << "PORTRAIT WIDTH: " << portraitDest.width << endl;
			nextUnitRight.SetPosition(portraitDest.x - portraitDest.width - nextUnitRight.GetRect().width, portraitDest.y + portraitDest.height);

			nextUnitLeft.SetPosition(portraitDest.x, portraitDest.y + portraitDest.height);

			closeSelection.SetPosition(portraitDest.x - portraitDest.width / 2, portraitDest.y - closeSelection.GetRect().height - 5);

			addUnit.SetPosition(portraitDest.x + nextUnitLeft.GetRect().width, portraitDest.y + portraitDest.height);
		}

	}

	if (!this->army.empty())this->army.clear();
	if (!this->enemyArmy.empty()) this->enemyArmy.clear();
	if (!battleEntities.empty()) battleEntities.clear();
	if (!this->armySpells.empty()) this->armySpells.clear();
	if (!this->enemySpells.empty()) this->enemySpells.clear();
	//if (!entityTurnPanels.empty()) entityTurnPanels.clear();

	for (auto& cell : *grid)
	{
		cell.entity.reset();
		cell.entityHealth = 0;
	}
	cout << "SET BATTLEGROUND, ARMY SIZE IS: " << army.size() << endl;
	this->armyList.reserve(army.size());
	this->armySpells.reserve(armySpells.size());

	this->enemyArmyList.reserve(enemyArmy.size());
	this->enemySpells.reserve(enemyArmySpells.size());

	this->armyList = army;
	this->armySpells = armySpells;

	this->enemyArmyList = enemyArmy;
	this->enemySpells = enemyArmySpells;

	//int i{ 0 };
	//SetArmy(armyList, this->army, i, east, Utilities::Relationship::Ally);


	//i = numColumns - 1;
	//SetArmy(enemyArmyList, this->enemyArmy, i, west, Utilities::Relationship::Enemy);

	//SetOpposingArmies();

	// ShuffleTurnPanel();

	cardDeck->SetDeck(armyList, this->armySpells);
	enemyCardDeck->SetDeck(enemyArmyList, this->enemySpells);

	deckEmpty = cardDeck->DeckEmpty();
	enemyDeckEmpty = enemyCardDeck->DeckEmpty();

	danteHero = CreateEntity(Utilities::Relationship::Ally, "dante_hero", textureManager, east, heroGridPos, grid);
	cout << danteHero->Type() << endl;
	danteHero->SetPosition(heroGridPos);
	dynamic_cast<BattleHero*>(danteHero.get())->SetManaPointers(availableMana, currentMana, usedMana);
	dynamic_cast<BattleHero*>(danteHero.get())->DeathCallback = [this]()
		{
			playerLost = true;
		};

	grid->at(heroGridPos).entity = danteHero;
	grid->at(heroGridPos).entityHealth = danteHero->GetHealth();
	grid->at(heroGridPos).entityType = EntityTypeFromString(danteHero->Type());

	battleEntities.emplace_back(danteHero);
	

	enemyHero = CreateEntity(Utilities::Relationship::Enemy, "dark_rider", textureManager, west, enemyHeroGridPos, grid);
	cout << enemyHero->Type() << endl;
	enemyHero->SetPosition(enemyHeroGridPos);
	dynamic_cast<BattleHero*>(enemyHero.get())->SetManaPointers(availableMana_enemy, currentMana_enemy, usedMana_enemy);
	dynamic_cast<BattleHero*>(enemyHero.get())->DeathCallback = [this]()
		{
			playerWon = true;
		};

	grid->at(enemyHeroGridPos).entity = enemyHero;
	grid->at(enemyHeroGridPos).entityHealth = enemyHero->GetHealth();
	grid->at(enemyHeroGridPos).entityType = EntityTypeFromString(enemyHero->Type());

	battleEntities.emplace_back(enemyHero);
}


void Battleground::Update()
{
	ShakeScreen();
	if (!showMenu_)
		showMenuButton.Update();

	UpdateArmies();
	
	if (playerWon)
		playerWonBannerFireworks->Update();

	if (playerWon || playerLost)
		return;


	auto mouse = GetMousePosition();
	Gameplay();
	hoveredEntity = nullptr;  // track which entity is under mouse
	int hoveredIndex = -1;                  // track its grid index
	bool clickedACell = false;
	//if (selectedEntity)
	//	std::cout << "still selected: " << selectedEntity->GetType() << '\n';
	const bool mouseOverPortraits = CheckCollisionPointRec(mouse, portraitsZone);
	danteHeroAbilityActive = dynamic_cast<BattleHero*>(danteHero.get())->GetSeekTarget();
	if(danteHeroAbilityActive)
		std::cout << "ability active: " << selectedEntity->GetType() << '\n';
	for (int i = 0; i < grid->size(); i++)
	{
		auto& currentCell = grid->at(i);
		bool mouseOverCell = CheckCollisionPointRec(mouse, *currentCell.rect);
		//mouseOverEntity = (mouseOverCell && currentCell.entity) ? true:false;

		if (currentCell.entity)
		{
			// let entity know whether it's being hovered
			currentCell.entity->MouseOver(mouseOverCell);
			
			currentCell.entity->SetTargeted(mouseOverCell && IsSpellSeekingTarget());
			

			// remember the entity currently under mouse
			if (mouseOverCell)
			{
				/*if (currentCell.entity) cout << currentCell.gridIndex << "HAS ENTITY "<< currentCell.entity->GetType() << endl;*/

				hoveredEntity = currentCell.entity.get();
				hoveredIndex = i;
				
			}
		}
		/*else
			mouseOverEntity = false;*/
		if (mouseOverPortraits && danteHeroAbilityActive)
		{
			continue;
		}
		// if mouse is not over this cell, skip click logic
		if (!mouseOverCell) continue;

		clickedACell = true;

		// LEFT CLICK
		if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
		{
			if (selectedEntity && selectedEntity->offence)
				continue;

			if (currentCell.entity)
			{
				// if new entity, deselect previously selected
				if (selectedEntityIndex != i &&
					selectedEntityIndex > -1 &&
					grid->at(selectedEntityIndex).entity)
				{
					grid->at(selectedEntityIndex).entity->Deselect();
					grid->at(selectedEntityIndex).color = basicCellColor;
				}

				// select this entity
				currentCell.color = mouseOverCellColor;
				selectedEntity = currentCell.entity;
				selectedEntity->Select();
				if (selectedEntity->GetRelationship() == Relationship::Enemy)
					cout << "ENEMY" << endl;
				else
					cout << "ALLY" << endl;
				selectedEntityIndex = i;
			}
			else if (selectedEntity && !mouseOverPortraits && !danteHeroAbilityActive) // clicked empty cell, clear selection
			{
				DeselectEntity();
			}
		}
		// RIGHT CLICK
		else if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))
		{
			if (!selectedEntity) continue;

			if (selectedEntity->GetSkipTurn()) 
			{
				//DeselectEntity();
				continue;
			}

			if (enemyTurn)
			{
				//DeselectEntity();
				continue;
			}

			if (selectedEntity->GetHadItsTurn())
			{
				//DeselectEntity();
				continue;
			}

			if (currentCell.entity) // ATTACK
			{
				if (currentCell.entity.get() == selectedEntity.get())
					continue;

				if (currentCell.entity->GetRelationship() == selectedEntity->GetRelationship())
					continue;

				if (selectedEntity->GetRelationship() == Relationship::Enemy)
				{
					DeselectEntity();
					continue;
				}

				if (TauntEnemyPresent() && !currentCell.entity->GetTaunting())
				{
					//DeselectEntity();
					continue;
				}

				defender = currentCell.entity;
				defender->SetEnemy(make_shared<Cell>(grid->at(selectedEntityIndex)));
				grid->at(selectedEntityIndex).entity->Attack(make_shared<Cell>(currentCell));
				attackingEntityIndex = selectedEntity->GetMoveToIndex();

				if (attackingEntityIndex <= -1)
				{
					if (defender)
					{
						defender->ResetEnemy();
						defender.reset();
					}
					DeselectEntity();
					continue;
				}

			/*	if (attackingEntityIndex != selectedEntityIndex)
				{
					grid->at(selectedEntityIndex).entity = nullptr;
					grid->at(selectedEntityIndex).color = basicCellColor;
					grid->at(attackingEntityIndex).entity = selectedEntity;
					grid->at(attackingEntityIndex).color = mouseOverCellColor;
				}*/

				selectedEntityIndex = attackingEntityIndex;

				attacker = (*grid)[selectedEntityIndex].entity;
				//int defIndex = defender->GetCellIndex();
				//attacker->SetPosition(defIndex + 1);
				sortByPosition(battleEntities);
			}
			else // Used to be MOVE
			{

				//selectedEntity->Move(i);
				//currentCell.entity = selectedEntity;
				//grid->at(selectedEntityIndex).entity = nullptr;
				//grid->at(selectedEntityIndex).color = basicCellColor;
				//selectedEntityIndex = i;
				//grid->at(selectedEntityIndex).color = mouseOverCellColor;

				DeselectEntity();
				continue;
				
			}
		}
	}

	if (!clickedACell && IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && !mouseOverPortraits && !danteHeroAbilityActive)
	{
		
		if (selectedEntity)
		{
			cout << "Deselect entity" << endl;
			DeselectEntity();
		}
	}

	// --- Update cursor after loop ---
	if (hoveredEntity && hoveredEntity != selectedEntity.get())
	{
		UpdateAttackCursor(*hoveredEntity);
	}
	else
	{
		// no valid hover, reset cursor
		ShowCursor();
		attackCursor = nullptr;
	}

	if (drawUnitMedia)
	{
		closeSelection.Update();
		nextUnitRight.Update();
		nextUnitLeft.Update();
		addUnit.Update();

		if (battleEntityPortraits[currentUnit].media)
		{
			const bool shouldShow = drawUnitMedia;
			if (shouldShow)
			{
				//cout << type << " shouldShow\n";
				if (GetMediaState(*battleEntityPortraits[currentUnit].media) != MEDIA_STATE_PLAYING)
				{

					SetMediaState(*battleEntityPortraits[currentUnit].media, MEDIA_STATE_PLAYING);

					//cout << type << " PLAYING\n";
				}
				//cout << type << " PLAYING\n";
				UpdateMedia(battleEntityPortraits[currentUnit].media.get());
			}
			//else
			//{
			//	// pause (keeps the current frame) or STOPPED if you prefer a full reset
			//	if (GetMediaState(*battleEntityPortraits[currentUnit]) != MEDIA_STATE_STOPPED)
			//	{
			//		SetMediaPosition(*battleEntityPortraits[currentUnit], 0.0);
			//		SetMediaState(*battleEntityPortraits[currentUnit], MEDIA_STATE_STOPPED);

			//	}
			//}
		}
	}

	cardDeck->Update();
	endTurnButton.Update();

	AnimateCardDraw(cardBackTextureDest_enemy, enemyDeckTextureDest, moveToCenter_enemy, enemyCardDeck.get(), endTurnButtonPressed);

	enemyCardDeck->Update();

	if (enemyTurn && enemyFinishedTurn)
	{
		enemyTimer -= GetFrameTime();
		if (enemyTimer <= 0.0f)
		{
			EnemyEndTurn();
		}
	}

	if (showBanner)
	{
		showPlayerTurnBanner -= GetFrameTime();

		if (showPlayerTurnBanner <= 0.0f)
			showBanner = false;
	}
	else
	{
		AnimateCardDraw(cardBackTextureDest, alliedDeckTextureDest, moveToCenter, cardDeck.get(), endTurnButtonPressed_enemy);
	}
}

void Battleground::UpdateArmies()
{

	/*for (int i = 0; i < grid->size(); i++)
	{
		if (grid->at(i).entity)
			grid->at(i).entity->Update((selectedEntity!=nullptr));
	}*/

	for (auto& entity : battleEntities)
		entity->Update(selectedEntity != nullptr);

	/*for (int i = 0; i < static_cast<int>(grid->size()); ++i)
	{
		auto& e = grid->at(i).entity;
		if (!e) continue;

		bool isSelected = selectedEntity && e.get() == selectedEntity.get();
		e->Update(isSelected);
	}*/

	//grid[defendingEntityIndex].entity->Lured(attackingEntityIndex);
}

void Battleground::RightMouseButtonReleased(const Vector2& mouse)
{

}

void Battleground::LeftMouseButtonReleased(const Vector2& mouse)
{
	
}

void Battleground::DeSelectOthers(const BattleEntity& selectedEntity)
{
	for (auto& entity : army)
	{
		if (entity && entity.get() != &selectedEntity)
		{
			entity->Deselect();
		}
	}

}

void Battleground::UpdateAttackCursor(BattleEntity& entity)
{
	// Reset if nothing is selected
	if (!selectedEntity)
	{
		ResetCursor();
		return;
	}

	// Reset if mouse is not over this entity
	if (!entity.GetMouseOver())
	{
		ResetCursor();
		return;
	}

	// We have a selected entity + hovering another entity
	HideCursor();

	// Decide if the selected entity is ranged
	const auto& type = Utilities::EntityTypeFromString(selectedEntity->Type());
	bool ranged = (std::ranges::find(Utilities::rangedUnits, type) != Utilities::rangedUnits.end());
	bool nextToEnemy = selectedEntity->nextToEnemy;
	float rotationModifier = 0.0f;
	string assetName = "attack_cursor";
	//auto entityDirection = entity.Direction();
	//string dir = (entityDirection == east) ? "east" : "west";


	if (ranged && !nextToEnemy)
	{
		// All ranged use the same cursor
		assetName = "ranged_cursor";
	}
	else
	{
		// Melee: pick based on attack direction
		auto attackDirection = entity.GetAttackDirection().side;

		switch (attackDirection)
		{
		case Side::north:    rotationModifier = 1.0f;    break;
		case Side::south: rotationModifier = 3.0f; break;

		case Side::east:
			rotationModifier = 2.0f; break;

		case Side::west:
			break;

		default:
			ResetCursor();
			return;
		}
	}

	attackCursor = textureManager->Asset(assetName);
	attackCursorSource = { 0, 0, static_cast<float>(attackCursor->width), static_cast<float>(attackCursor->height) };
	attackCursorDest = { 0, 0, static_cast<float>(attackCursor->width), static_cast<float>(attackCursor->height) };
	attackCursorOrigin = {attackCursorSource.width / 2.0f, attackCursorSource.height / 2.0f};
	attackCursorRotation = 90.0f * rotationModifier;
}

void Battleground::ResetCursor()
{
	ShowCursor();
	attackCursor = nullptr;
}

void Battleground::DeselectEntity()
{	

	if (selectedEntity)
	{
		cout << selectedEntity->GetType() << " DESELECT ENTITY" << endl;
		selectedEntity->Deselect();
		selectedEntity.reset();
	}
	
	if (selectedEntityIndex >= 0 &&
		selectedEntityIndex < static_cast<int>(grid->size()))
	{
		grid->at(selectedEntityIndex).color = basicCellColor;
	}
	
	selectedEntityIndex = -1;

	
}

bool Battleground::TauntEnemyPresent() const
{
	return std::ranges::any_of(enemyArmy, [](const auto& e) {
		return e && e->GetTaunting();
		});
}

void Battleground::PopulateManaStones(vector<UI_Asset>& manaStones, Rectangle destination, Texture2D* texture)
{
	int width = manaStoneTextureSource.width;
	for (int i = 0; i < MAX_MANA; i++)
	{
		Rectangle dest = {
			destination.x + (2 + width) * i,
			destination.y,
			destination.width,
			destination.height
		};
		Rectangle source{ 0.0f, 0.0f, static_cast<float>(texture->width), static_cast<float>(texture->height) };
		manaStones.emplace_back(texture, source, dest);
	}
}

void Battleground::LeftMouseButtonReleased()
{
	bool placeCard = false;

	if (selectedCard && !enemyTurn)
	{
		const bool overZone = CheckCollisionRecs(selectedCard->GetRectDest(), cardPlacementZone);
		const bool canAfford = selectedCard->GetManaCost() <= availableMana;
		const bool availableSpace = army.size() < MAX_ENTITIES;

		switch (selectedCard->GetCardType())
		{
		case Utilities::CardType::Entity:
			// Entity: drop on the placement zone
			if (overZone && canAfford && availableSpace)
			{
				const auto& type = selectedCard->GetType();

				SummonEntityWithEffect(type, army, alliedGridPos, east, Relationship::Ally);				

				usedMana += selectedCard->GetManaCost();
				availableMana = currentMana - usedMana;
				placeCard = true;
			}
			break;

		case Utilities::CardType::Spell:
		case Utilities::CardType::Buff:
		case Utilities::CardType::Debuff:
		case Utilities::CardType::StateAltering:

			// Spell: needs a target
			if (hoveredEntity && canAfford)
			{
				TryPlaySpell(selectedCard);

				usedMana += selectedCard->GetManaCost();
				availableMana = currentMana - usedMana;
				placeCard = true;
				
			}
			break;
		}
	}

	cardDeck->LeftMouseButtonReleased(placeCard);
	selectedCard.reset();

	if (hoveredEntity)
	{
		auto& targetRect = *grid->at(hoveredEntity->GetCellIndex()).rect;
					
		dynamic_cast<BattleHero*>(danteHero.get())->AbilitySetTarget(hoveredEntity, targetRect, [this]() { StartShake(); });
		dynamic_cast<BattleHero*>(enemyHero.get())->AbilitySetTarget(hoveredEntity, targetRect, [this]() { StartShake(); });
					
	}
	else
	{
		/*dynamic_cast<BattleHero*>(danteHero.get())->DeselectAbility();
		dynamic_cast<BattleHero*>(enemyHero.get())->DeselectAbility();*/
	}
	
	//if (enemyHeroSpecial->GetSeekTarget())
	//{
	//	if (hoveredEntity)
	//	{
	//		auto& targetRect = *grid->at(hoveredEntity->GetCellIndex()).rect;
	//		enemyHeroSpecial->SetTarget(hoveredEntity, targetRect);
	//	}
	//	else
	//	{
	//		enemyHeroSpecial->Deselect();
	//	}
	//}

	if (showMenu_)
	{
		const string& option = menu.GetOption();
		//if (option.size() > 0)
		//	playClickButtonSound = true;

		if (option == "create_army")
		{
			drawUnitMedia = true;
			createArmy_ = true;
		}
		else if (option == "create_enemy_army")
		{
			drawUnitMedia = true;
			createEnemyArmy_ = true;
		}
		else if (option == "show_enemy_hand")
		{
			dynamic_cast<EnemyDeck*>(enemyCardDeck.get())->ShowHand();
		}
		else if (option == "shake_screen")
		{
			StartShake(0.4f, 30.0f);
		}
		else if (option == "grid")
		{
			drawGrid = !drawGrid;
		}
		else if (option == "back")
		{
			showMenu_ = false;
			drawUnitMedia = false;
			createArmy_ = false;
			createEnemyArmy_ = false;
		}
		else if (option == "exit_battle_mode")
		{
			if (createEnemyArmy_ || createArmy_) return;
			*battleMode_ = false;
			if (backToEditorCallback)
				backToEditorCallback();
		}
	}
}

void Battleground::TryPlaySpell(const shared_ptr<Card>& card)
{
	auto* spellCard = dynamic_cast<SpellCard*>(card.get());
	if (!spellCard || !hoveredEntity)
		return;

	spell = spellCard->GetSpell();
	auto& cellRect = *grid->at(hoveredEntity->GetCellIndex()).rect;

	spell->SetPosition(
		cellRect.x + cellRect.width * 0.5f - spell->GetDest().width * 0.5f,
		cellRect.y + cellRect.height - spell->GetDest().height
	);

	BattleEntity* target = hoveredEntity;
	spell->SetDrawOnce(true, 0.5f, [this, card, spellCard, target]()
		{
			if (card && target)
			{
				StartShake(0.4f, 22.0f);
				spellCard->PlayCard(*target);
				
			}
		});
}

void Battleground::TryPlaySpellOn(const shared_ptr<Card>& card, BattleEntity* target)
{
	auto* spellCard = dynamic_cast<SpellCard*>(card.get());
	if (!spellCard || !target)
		return;
	enemyCastingSpell = true;
	spell = spellCard->GetSpell();
	auto& cellRect = *grid->at(target->GetCellIndex()).rect;

	spell->SetPosition(
		cellRect.x + cellRect.width * 0.5f - spell->GetDest().width * 0.5f,
		cellRect.y + cellRect.height - spell->GetDest().height
	);

	spell->SetDrawOnce(true, 0.5f, [this, card, spellCard, target]()
		{
			if (card && target)
			{
				StartShake(0.4f, 22.0f);
				spellCard->PlayCard(*target);
				enemyCastingSpell = false;
			}
		});
}

void Battleground::LeftMouseButtonDown()
{
	cardDeck->LeftMouseButtonDown();
	selectedCard = cardDeck->GetSelectedCard();
}

void Battleground::LeftMouseButtonPressed()
{
	if (danteHero)
		dynamic_cast<BattleHero*>(danteHero.get())->LeftMouseButtonPressed();

	if (enemyHero)
		dynamic_cast<BattleHero*>(enemyHero.get())->LeftMouseButtonPressed();
}

void Battleground::LoadPortraits(const char* type, vector<Portrait>& Portraits)
{
	auto portraitData = textureManager->GetPortraitData(type);
	if (portraitData && !portraitData->empty())
	{
		CreatePortraitGrid(*portraitData, Portraits);
	}

}

void Battleground::CreatePortraitGrid(const map <string, Texture>& nameTextureMap, vector<Portrait>& Portraits)
{
	const float width{ 64.0f };
	const float height{ 64.0f };
	float x{ 0.0f };
	float y{ 0.0f };
	vector<Rectangle> Cells;
	Cells.reserve(nameTextureMap.size());

	for (const auto& [name, texture] : nameTextureMap) {
		Rectangle rect{ x, y, width, height };
		Cells.push_back(rect);
		Portraits.emplace_back(name, texture, rect);

		// Update position for next cell
		y += height;

		// Check if we need to move to next column
		if (y + height > SCR_HEIGHT - 128.0f) {  // 128 = 64*2 margin
			y = 0.0f;
			x += width;
		}
	}
}

void Battleground::AnimateCardDraw(Rectangle& cardBackTextureDest, Rectangle cardDeckDest, Vector2 moveToCenter, Deck* cardDeck, bool& endTurnButtonPressed)
{
	if (!endTurnButtonPressed) return;
	
	if (cardDeck->DeckEmpty())
	{
		endTurnButtonPressed = false;
		cardFinishedDrawing = true;

		UpdateDeckEmptyFlag(cardDeck);

		return;

	}

	// Vector from the center of the moving card to the target center
	float cardCenterX = cardBackTextureDest.x + cardBackTextureDest.width * 0.5f;
	float cardCenterY = cardBackTextureDest.y + cardBackTextureDest.height * 0.5f;

	float dx = moveToCenter.x - cardCenterX;
	float dy = moveToCenter.y - cardCenterY;
	float distance = sqrtf(dx * dx + dy * dy);
	float speed = 500.0f;
	SetLastCardFlag(cardDeck);
	if (distance > 3.0f)                     // still moving
	{
		// Normalize and move
		dx /= distance;
		dy /= distance;

		                 // adjust to taste
		float dt = GetFrameTime();
		cardBackTextureDest.x += dx * dt * speed;
		cardBackTextureDest.y += dy * dt * speed;
	}
	else                                     // arrived at the center
	{
		// Snap exactly to center
		cardBackTextureDest.x = moveToCenter.x - cardBackTextureDest.width * 0.5f;
		cardBackTextureDest.y = moveToCenter.y - cardBackTextureDest.height * 0.5f;

		// Trigger the actual draw
		cardDeck->DrawCard();

		// Reset animation state
		endTurnButtonPressed = false;
		/*showManaShimmer = false;*/

		// Optional: put the card-back texture back to the deck position
		// so it’s ready for the next draw
		cardBackTextureDest.x = cardDeckDest.x;
		cardBackTextureDest.y = cardDeckDest.y;

		cardFinishedDrawing = true;

	}
}

void Battleground::UpdateDeckEmptyFlag(Deck* cardDeck)
{
	if (auto* enemyDeck = dynamic_cast<EnemyDeck*>(cardDeck))
		enemyDeckEmpty = enemyDeck->DeckEmpty();
	else
	{
		cout << "DECK IS EMPTY" << endl;
		deckEmpty = cardDeck->DeckEmpty();
	}
}

void Battleground::SetLastCardFlag(Deck* cardDeck)
{
    const bool lastCard = cardDeck->LastCard();

    if (dynamic_cast<EnemyDeck*>(cardDeck))
        this->lastCardEnemy = lastCard;
    else
        this->lastCard = lastCard;
}


void Battleground::StartShake(float duration, float strength)
{
	shakeScreen = true;
	shakeTime = duration;
	shakeDuration = duration;
	shakeStrength = strength;
}

void Battleground::ShakeScreen()
{
	if (!shakeScreen) return;

	shakeTime -= GetFrameTime();
	if (shakeTime <= 0.0f)
	{
		background->SetPosition(bgHome.x, bgHome.y);  // whatever setter you have
		for (auto& entity : battleEntities)
		{
			int cellIndex = entity->GetCellIndex();
			entity->SetCell(*(*grid)[cellIndex].rect);
		}
		shakeScreen = false;
		return;
	}

	float t = shakeTime / shakeDuration;      // 1 -> 0
	float mag = shakeStrength * t * t;        // strong at impact, dies out

	float ox = (GetRandomValue(-100, 100) / 100.0f) * mag;
	float oy = (GetRandomValue(-100, 100) / 100.0f) * mag * 0.5f; // less vertical

	for (auto& entity : battleEntities)
	{
		float x = entity->X();
		float y = entity->Y();
		entity->SetX(x + ox/mag); // weaker shake for entities
		entity->SetY(y + oy/mag);
	}

	background->SetPosition(bgHome.x + ox, bgHome.y + oy);
}

//void Battleground::SelectPortrait(const vector<Portrait>& Portrait)
//{
//
//	for (const auto& portrait : Portrait)
//	{
//		if (CheckCollisionPointRec(GetMousePosition(), portrait.GetRect()))
//		{
//			const auto& type = portrait.GetType();
//			//cout << type << endl;
//			//if (type == "dante") return;
//			if (type == "0back") Back();
//			else if (ShowUnits() || ShowNPCs() || ShowProps())
//			{
//				auto it = std::find(unitTypes->begin(), unitTypes->end(), type);
//				if (it != unitTypes->end())
//				{
//					if (addingUnitsToCaptain)
//						AddUnitToCaptain(type);
//					else
//						AddUnit(type, position);
//				};
//			}
//
//		}
//	}
//}

void Battleground::DrawShadows(float dt)
{
	/*for (auto& entity : army)
	{
		entity->DrawShadow(ANIMATION_SPEED, dt);
	}

	for (auto& entity : enemyArmy)
	{
		entity->DrawShadow(ANIMATION_SPEED, dt);
	}*/

	if (battleEntities.empty()) return;

	// Draw all entities except the last one
	for (size_t i = 0; i < battleEntities.size(); ++i)
	{
		battleEntities[i]->DrawShadow(ANIMATION_SPEED, dt);
	}

}

void Battleground::DrawBackground()
{
	background->Display();
}

void Battleground::DrawGrid()
{
	if (!drawGrid) return;

	if (selectedCard) DrawRectangleRec(cardPlacementZone, mouseOverCellColor);

	
	for (const auto& cell : *grid)
	{
		//auto color = (cell.mouseOverCell) ? cellHoovered : cellColor;

		//if (cell.entity && cell.entity->selected)
		//{
		//	color = cellHoovered;
		//}
		DrawRectangleRec((*cell.rect), cell.color);
		DrawRectangleLinesEx((*cell.rect), 1, DARKBLUE);

		/*if (cell.entityHealth > 0) DrawText(to_string(cell.entityHealth).c_str(),
			cell.rect->x + cell.rect->width - 20,
			cell.rect->y + cell.rect->height - 20,
			20,
			BLUE);*/
	}

}

void Battleground::DrawArmies(float dt)
{
	enemyCardDeck->DisplayHand();

	if (battleEntities.empty()) return;

	// Draw all entities except the last one
	for (size_t i = 0; i < battleEntities.size(); ++i)
	{
		battleEntities[i]->Display(ANIMATION_SPEED, 0, dt);
	}

	// Draw the last entity only when GetDrawOnce() returns false
	//if (!round_sparkle_burst->GetDrawOnce())
	//{
	//	battleEntities.back()->Display(ANIMATION_SPEED, 0, dt);
	//}

	round_sparkle_burst->DrawOnce();


}

void Battleground::DrawAttackCursor()
{
	if (attackCursor == nullptr) return;

	auto mousePos = GetMousePosition();
	//attackCursorDest.x = mousePos.x - attackCursor->width / 2.0f;
	//attackCursorDest.y = mousePos.y - attackCursor->height / 2.0f;

	attackCursorDest.x = mousePos.x;
	attackCursorDest.y = mousePos.y;

	DrawTexturePro(*attackCursor,
		attackCursorSource,
		attackCursorDest,
		attackCursorOrigin,
		attackCursorRotation,
		RAYWHITE
	);

}

void Battleground::DrawUI()
{
	// Background behind the RIGHT portrait
	//if(selectedEntity == nullptr || (selectedEntity && selectedEntity->GetRelationship() == Relationship::Enemy))
		DrawRectangleRec(topSquareFrameDestRect, INTERFACE_BG_COLOR);
	// Background behind the LEFT portrait
	//if(hoveredEntity == nullptr || (hoveredEntity && hoveredEntity->GetRelationship() == Relationship::Ally))
		DrawRectangleRec(bottomSquareFrameDestRect, INTERFACE_BG_COLOR);
	// Display RIGHT portrait frame
	DrawTexturePro(*squareFrame, squareFrameSourceRect, topSquareFrameDestRect, origin, 0.0f, RAYWHITE);
	// Display LEFT portrait frame
	DrawTexturePro(*squareFrame, squareFrameSourceRect, bottomSquareFrameDestRect, origin, 0.0f, RAYWHITE);

	DrawRectangleRec(portraitsZone, INTERFACE_BG_COLOR);



	float tempY = leftPortraitDestRect.y;
	//for (int i{}; i < entityTurnPanels.size(); i++)
	//{
	//	DrawTexturePro(*entityTurnPanels[i].unit->GetPortrait(), squareFrameSourceRect, leftPortraitDestRect, origin, 0.0f, RAYWHITE);
	//	if (entityTurnPanels[i].unit->selected) DrawRectangleRec(leftPortraitDestRect, mouseOverCellColor);			
	//	DrawTexturePro(*squareFrame, squareFrameSourceRect, leftPortraitDestRect, origin, 0.0f, RAYWHITE);
	//	leftPortraitDestRect.y += 64.f;

	//}
	//leftPortraitDestRect.y = tempY;

}

void Battleground::DisplayCards()
{
	cardDeck->DisplayHand();
	
	if (hoveredEntity)
		hoveredEntity->DisplayTargeted(ANIMATION_SPEED);

	dynamic_cast<BattleHero*>(danteHero.get())->DisplayAbilityEffect();
	dynamic_cast<BattleHero*>(enemyHero.get())->DisplayAbilityEffect();

	endTurnButton.Display();

	if (!lastCard)
	{
		DrawTexturePro(*deckTexture,
			deckTextureSource,
			alliedDeckTextureDest,
			origin,
			0.0f,
			RAYWHITE
		);
	}

	if (!lastCardEnemy)
	{
		DrawTexturePro(*enemyDeckTexture,
			enemyDeckTextureSource,
			enemyDeckTextureDest,
			origin,
			0.0f,
			RAYWHITE
		);
	}

	if (endTurnButtonPressed_enemy && !showBanner && !deckEmpty)
	{
		DrawTexturePro(*cardBackTexture,
			cardBackTextureSource,
			cardBackTextureDest,
			origin,
			0.0f,
			RAYWHITE
		);
	}

	DrawTexturePro(*manaBanner, manaBannerTextureSource, manaBannerTextureDest, origin, 0.0f, RAYWHITE);

	DrawTexturePro(*manaBanner, manaBannerTextureSource, manaBannerTextureDest_enemy, origin, 0.0f, RAYWHITE);

	DrawManaStones(manaStones, currentMana, usedMana);
	DrawManaCostLight();

	manaShimmer->DrawOnce();
	DrawManaStones(manaStones_enemy, currentMana_enemy, usedMana_enemy);

	//DrawRectangleRec(cardBackMoveToBox_enemy, YELLOW);

	if (endTurnButtonPressed && !enemyDeckEmpty)
	{
		DrawTexturePro(*cardBackTexture,
			cardBackTextureSource_enemy,
			cardBackTextureDest_enemy,
			origin,
			0.0f,
			RAYWHITE
		);
	}

	if (showBanner)
	{
		DrawTexturePro(*playerTurnBanner, playerTurnBannerSource, playerTurnBannerDest, { 0.f,0.f }, 0.0f, RAYWHITE);
	}

	DisplayPlayerWon();
	DisplayPlayerLost();

	if (spell) spell->DrawOnce();

	EnemyDisplayChosenCard();
}

void Battleground::DrawMenu()
{
	if (showMenu_)
		menu.DrawMenu();
	else
		showMenuButton.Display();

	if (drawUnitMedia)
	{

		nextUnitRight.Display();
		nextUnitLeft.Display();
		closeSelection.Display();
		addUnit.Display();

		if (battleEntityPortraits[currentUnit].media && battleEntityPortraits[currentUnit].media->videoTexture.id != 0)
		{
			DrawTexturePro(
				battleEntityPortraits[currentUnit].media->videoTexture,
				portraitSource,
				portraitDest,
				{ 0, 0 },
				0.0f,
				WHITE
			);

			DrawTexturePro(*squareFrame,
				squareFrameSourceRect,
				portraitDest,
				origin,
				0.0f,
				RAYWHITE
			);

		}

	}
}

void Battleground::DisplayPlayerWon()
{
	if (!playerWon) return;

	playerWonBannerFireworks->Draw();
}

void Battleground::DisplayPlayerLost()
{
	if (!playerLost) return;

	DrawTexture(*playerLostScreen, 0, 0, RAYWHITE);
	DrawTexturePro(*playerLostBanner, playerLostBannerSource, playerTurnBannerDest, { 0.f,0.f }, 0.0f, RAYWHITE);
}

void Battleground::DrawManaStones(vector<UI_Asset> manaStones, int currentMana, int usedMana)
{
	if (manaStones.empty() || currentMana <= 0) return;

	for (int i = 0; i < currentMana; i++)
	{
		DrawTexturePro(*manaStones[i].texture, manaStones[i].source, manaStones[i].dest, origin, 0.0f, RAYWHITE);
	}

	//if(!manaShimmer->GetDrawOnce())
	//	DrawTexturePro(*manaStones[currentMana - 1].texture, manaStones[currentMana - 1].source, manaStones[currentMana - 1].dest, origin, 0.0f, RAYWHITE);

	for (int i = 0; i < usedMana; i++)
	{
		DrawTexturePro(*manaStones[i].texture, manaStones[i].source, manaStones[i].dest, origin, 0.0f, usedManaColor);
	}

}
void Battleground::DrawManaCostLight()
{
	int cost = cardDeck->GetMousedOverManaCost();
	if (cost <= 0)
		return;

	int start = usedMana;
	int end = usedMana + cost;

	if (end > availableMana)
		return;   // can't afford — don’t preview

	start = std::max(start, 0);
	end = std::min(end, static_cast<int>(manaCostLights.size()));
	end = std::min(end, currentMana);   // only stones that exist this turn

	for (int i = start; i < end; ++i)
	{
		DrawTexturePro(*manaCostLights[i].texture,
			manaCostLights[i].source,
			manaCostLights[i].dest,
			origin, 0.0f, RAYWHITE);
	}
}



void Battleground::ShuffleTurnPanel()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());  // keep the generator alive across calls

	std::shuffle(enemyTurnPanel.begin(), enemyTurnPanel.end(), gen);
}

void Battleground::ClearDeadEntities(vector<shared_ptr<BattleEntity>>& army)
{
	for (auto it = army.begin(); it != army.end(); )
	{
		if ((*it)->HasDied())
		{
			int cellIndex = (*it)->GetCellIndex();
			grid->at(cellIndex).entity = nullptr;
			grid->at(cellIndex).entityHealth = 0;
			grid->at(cellIndex).entityType = Utilities::EntityType::Unknown;

			// remove from the combined list
			battleEntities.erase(
				std::remove_if(battleEntities.begin(), battleEntities.end(),
					[&](const std::shared_ptr<BattleEntity>& e) {
						return e.get() == it->get();
					}),
				battleEntities.end()
			);

			it = army.erase(it);
		}
		else
		{
			++it;
		}
	}
}

std::shared_ptr<BattleEntity> findEnemy(
	const std::vector<std::shared_ptr<BattleEntity>>& opposingArmy,
	std::shared_ptr<BattleEntity> selectedEntity)
{
	if (!selectedEntity)
		return nullptr;
	cout << "BEGIN SERCHING FOR ENEMIES" << endl;
	// 1) Alive units only
	std::vector<int> candidates;
	candidates.reserve(opposingArmy.size());

	for (int i = 0; i < static_cast<int>(opposingArmy.size()); ++i)
	{
		auto& e = opposingArmy[i];
		if (!e || e->HasDied())
			continue;
		candidates.push_back(i);
	}

	if (candidates.empty())
		return nullptr;

	// 2) If anyone is taunting, only those
	std::vector<int> taunters;
	for (int i : candidates)
	{
		if (opposingArmy[i]->GetTaunting())
			taunters.push_back(i);
	}

	const std::vector<int>& pool = taunters.empty() ? candidates : taunters;

	// 3) Rank categories (same / lower / higher)
	const int entityRank = selectedEntity->GetRank();

	auto categoryOf = [&](int x) {
		int r = opposingArmy[x]->GetRank();
		if (r == entityRank) return 0;
		if (r < entityRank) return 1;
		return 2;
		};

	std::vector<int> sorted = pool;
	std::sort(sorted.begin(), sorted.end(), [&](int a, int b) {
		int ca = categoryOf(a);
		int cb = categoryOf(b);
		if (ca != cb)
			return ca < cb;
		if (ca == 1) return opposingArmy[a]->GetRank() > opposingArmy[b]->GetRank();
		if (ca == 2) return opposingArmy[a]->GetRank() < opposingArmy[b]->GetRank();
		return false;
		});

	int bestCategory = categoryOf(sorted[0]);

	std::vector<int> sameCategory;
	for (int idx : sorted)
	{
		if (categoryOf(idx) != bestCategory)
			break;
		sameCategory.push_back(idx);
	}

	// 4) Lowest health in that category
	auto bestIt = std::min_element(sameCategory.begin(), sameCategory.end(),
		[&](int a, int b) {
			return opposingArmy[a]->GetHealth() < opposingArmy[b]->GetHealth();
		});

	cout << "Target: " << opposingArmy[*bestIt]->GetType() << endl;
	return opposingArmy[*bestIt];
}



void Battleground::EnemyDisplayChosenCard()
{
	if (!enemyDisplayChosenCard || !chosenCard) return;

	chosenCard->Display();
	enemyDisplayCardTimer -= GetFrameTime();
	
	if (enemyDisplayCardTimer > 0.f)
		return;

	enemyDisplayChosenCard = false;
	auto cb = std::move(chosenCardCallback);
	chosenCard.reset();

	if (cb) cb();
}

void Battleground::ActivateDisplayChosenCard(shared_ptr<Card>card, function<void()>onDone)
{
	chosenCard = std::move(card);
	chosenCard->PrepForDisplay(SCR_WIDTH * 0.5f, SCR_HEIGHT * 0.2f);
	
	chosenCardCallback = std::move(onDone);
	enemyDisplayCardTimer = 2.0f;
	enemyDisplayChosenCard = true;


}

void Battleground::Gameplay()
{
	EnemyPlayCards();

	if (enemyFinishedPlayingCards)
	{
		finishedPlayingCardsTimer -= GetFrameTime();
		if (finishedPlayingCardsTimer <= 0.0f)
			beginEnemyBattlePhase = true;
	}

	if (!enemyFinishedTurn && beginEnemyBattlePhase)
	{
		EnemyBattlePhase();
	}	
}

//void Battleground::EnemyPlayCards()
//{
//	if (enemyTurn && !enemyFinishedPlayingCards && cardFinishedDrawing)
//	{
//		cardDrawnTimer -= GetFrameTime();
//
//		if (cardDrawnTimer > 0.0f)
//			return;
//
//		
//		if (enemyHeroPlayAbility)
//		{
//			EnemyHeroPlayAbility();
//		}
//		else
//		{
//			EnemyPlayDebuffSpell();
//		}
//
//
//		//EnemyHeroPlayAbility();
//		if(enemyFinishedPlayingDebuff && !enemyCastingSpell)
//			EnemyPlayEntity();
//
//		if (enemyFinishedPlayingEntity)
//			EnemyPlayBuffSpell();
//
//		enemyFinishedPlayingCards = enemyFinishedPlayingEntity && enemyFinishedPlayingSpells && !enemyCastingSpell;
//	}
//}


void Battleground::EnemyPlayCards()
{
	if (!enemyTurn || enemyFinishedPlayingCards || !cardFinishedDrawing)
		return;
	if ((cardDrawnTimer -= GetFrameTime()) > 0.0f)
		return;

	if (enemyCastingSpell || enemySummoningEntity)
		return;

	if (!hasPlan)
	{
		plannedPlay = PickEnemyPlay(availableMana_enemy);
		hasPlan = true;
		enemyCardDeck->StartLooking();

		if (plannedPlay.score <= 0.f)
		{
			enemyFinishedPlayingCards = true;
			hasPlan = false;
			return;
		}
	}

	if (plannedPlay.kind == EnemyActionKind::HeroAbility)
	{
		EnemyHeroPlayAbility(plannedPlay.target);
		hasPlan = false;                 // plan again with leftover mana
		return;
	}

	auto card = enemyCardDeck->PlayChosenCard(plannedPlay.card);
	if (!card)
	{
		if (enemyCardDeck->FinishedLooking())
			enemyFinishedPlayingCards = true;
		return;
	}

	BattleEntity* target = plannedPlay.target;
	auto type = plannedPlay.kind;

	enemySummoningEntity = (type == EnemyActionKind::Entity);
	enemyCastingSpell = (type != EnemyActionKind::Entity);

	ActivateDisplayChosenCard(card, [this, card, target, type]()
		{
			if (type == EnemyActionKind::Entity)
				EnemyHeroSummonEntity(card->GetType(), enemyArmy, enemyGridPos,
					west, Relationship::Enemy);
			else
				TryPlaySpellOn(card, target);

			usedMana_enemy += card->GetManaCost();
			availableMana_enemy = currentMana_enemy - usedMana_enemy;
			hasPlan = false; // next card, if any
		});
}

void Battleground::EnemyPlayEntity()
{
	if (enemyFinishedPlayingEntity) return;

	shared_ptr<Card> card = nullptr;
	bool availableSpace = enemyArmy.size() < MAX_ENTITIES;

	if (availableSpace)
		card = enemyCardDeck->PlayEntityCard(availableMana_enemy);
	else
		enemyFinishedPlayingEntity = true;


	if (card != nullptr)
	{
		SummonEntityWithEffect(card->GetType(), this->enemyArmy, enemyGridPos, west, Utilities::Relationship::Enemy);
		usedMana_enemy += card->GetManaCost();
		availableMana_enemy = currentMana_enemy - usedMana_enemy;

		enemyFinishedPlayingEntity = true;
	}
	else if (enemyCardDeck->FinishedLooking())
	{
		// no playable card
		enemyFinishedPlayingEntity = true;
	}

}

void Battleground::EnemyHeroPlayAbility()
{
	if (enemyFinishedPlayingDebuff) return;

	const int& manaCost = dynamic_cast<BattleHero*>(enemyHero.get())->GetAbilityManaCost();

	if (availableMana_enemy < manaCost)
	{
		enemyFinishedPlayingDebuff = true;
		return;
	}

	if (army.empty())
	{
		enemyFinishedPlayingDebuff = true;
		return;
	}

	
	const auto& ability = dynamic_cast<BattleHero*>(enemyHero.get())->GetSpecialAbility();
	BattleEntity* target = PickEnemyHeroAbilityTarget();

	if (target)
	{
		enemyCastingSpell = true;
		auto& targetRect = *grid->at(target->GetCellIndex()).rect;
		ability->SetTarget(target, targetRect, [&]() {
				enemyCastingSpell = false; 
				ShakeScreen();
			}
		);
		usedMana_enemy += manaCost;
		availableMana_enemy = currentMana_enemy - usedMana_enemy;
		//finishedPlayingCardsTimer += 2.1f;
	}

	enemyFinishedPlayingDebuff = true;
	return;
	
}

void Battleground::EnemyHeroPlayAbility(BattleEntity* target)
{
	if (enemyFinishedPlayingDebuff) return;

	const int& manaCost = dynamic_cast<BattleHero*>(enemyHero.get())->GetAbilityManaCost();

	if (availableMana_enemy < manaCost)
	{
		enemyFinishedPlayingDebuff = true;
		return;
	}

	if (army.empty())
	{
		enemyFinishedPlayingDebuff = true;
		return;
	}


	const auto& ability = dynamic_cast<BattleHero*>(enemyHero.get())->GetSpecialAbility();
	//BattleEntity* target = PickEnemyHeroAbilityTarget();

	if (target)
	{
		enemyCastingSpell = true;
		auto& targetRect = *grid->at(target->GetCellIndex()).rect;
		ability->SetTarget(target, targetRect, [&]() {
			enemyCastingSpell = false;
			ShakeScreen();
			}
		);
		usedMana_enemy += manaCost;
		availableMana_enemy = currentMana_enemy - usedMana_enemy;
		//finishedPlayingCardsTimer += 2.1f;
	}

	enemyFinishedPlayingDebuff = true;
	return;

}

void Battleground::EnemyHeroSummonEntity(string type, vector<shared_ptr<BattleEntity>>& army, int i, Utilities::Direction direction, Utilities::Relationship relationship)
{
	enemySummoningEntity = true;
	auto& entitySize = entityStats[type].size;
	int gridPos = FindAvailableCell(i, entitySize, relationship);

	round_sparkle_burst->SetPosition(
		grid->at(gridPos).rect->x - grid->at(gridPos).rect->width / 2,
		grid->at(gridPos).rect->y - grid->at(gridPos).rect->height / 2
	);

	round_sparkle_burst->SetDrawOnce(true, 0.0f, [this, type, &army, gridPos, direction, relationship]()
		{
			AddEntity(type, army, gridPos,
				direction, relationship);
			enemySummoningEntity = false;
		});

	
}

void Battleground::EnemyPlayDebuffSpell()
{
	if (enemyFinishedPlayingDebuff) return;

	if (availableMana_enemy <= 0)
	{
		enemyFinishedPlayingDebuff = true;
		return;
	}

	if (army.empty())
	{
		enemyFinishedPlayingDebuff = true;
		return;
	}

	auto card = enemyCardDeck->PlayDebuffCard(availableMana_enemy);
	if (card)
	{
		BattleEntity* target = PickSpellTarget(*card);
		if (target)
		{
			TryPlaySpellOn(card, target);
			usedMana_enemy += card->GetManaCost();
			availableMana_enemy = currentMana_enemy - usedMana_enemy;
			//finishedPlayingCardsTimer += 2.1f;
		}
		enemyFinishedPlayingDebuff = true;
		return;
	}

	if (enemyCardDeck->FinishedLooking())
	{
		enemyFinishedPlayingDebuff = true;
	}

}

void Battleground::EnemyPlayBuffSpell()
{
	if (enemyFinishedPlayingSpells) return;
	
	if (availableMana_enemy <= 0)
	{
		enemyFinishedPlayingSpells = true;
		return;
	}

	if (enemyArmy.empty())
	{
		enemyFinishedPlayingSpells = true;
		return;
	}

	auto card = enemyCardDeck->PlayBuffCard(availableMana_enemy);
	if (card)
	{
		BattleEntity* target = PickSpellTarget(*card);
		if (target)
		{
			TryPlaySpellOn(card, target);
			usedMana_enemy += card->GetManaCost();
			availableMana_enemy = currentMana_enemy - usedMana_enemy;
			//finishedPlayingCardsTimer += 2.1f;
		}
		enemyFinishedPlayingSpells = true;
		return;
	}

	if (enemyCardDeck->FinishedLooking())
	{
		enemyFinishedPlayingSpells = true;
	}
	
}


BattleEntity* Battleground::PickSpellTarget(Card& card)
{
	auto type = card.GetCardType();
	auto subType = card.GetSubType();
	bool friendly = (type == CardType::Buff || subType == CardType::Buff);

	auto& pool = friendly ? enemyArmy : army;
	if (pool.empty())
		return nullptr;

	// cheapest: first living unit; or reuse findEnemy for attacks
	for (auto& e : pool)
		if (e && !e->HasDied())
			return e.get();
	return nullptr;
}

BattleEntity* Battleground::PickEnemyHeroAbilityTarget()
{
	auto& pool = army;
	if (pool.empty())
		return nullptr;

	// cheapest: first living unit; or reuse findEnemy for attacks
	for (auto& e : pool)
		if (e && !e->HasDied())
			return e.get();
	return nullptr;

}

float Battleground::ScoreEntity(const Card& card, int manaLeft, int armySize)
{
	if (card.GetManaCost() > manaLeft) return -1.f;
	if (armySize >= MAX_ENTITIES) return - 1.f;

	float score = 20.f;
	score += entityStats[card.GetType()].damage * 3.f;
	score += entityStats[card.GetType()].health;
	
	if (entityStats[card.GetType()].taunting) score += 15.f;

	if (armySize == 0) score += 25.f; // empty board = must summon!

	score -= card.GetManaCost() * 1.5f; // leave mana for spells

	return score;
}

float Battleground::ScoreDebuff(const Card& card, BattleEntity* target, int manaLeft)
{
	if (!target || card.GetManaCost() > manaLeft) return -1.f;
	float score = 10.f + target->GetAttack() * 2.f;  // kill high attack target first
	if (target->GetHealth() <= spellStats[card.GetType()].statsModifier) score += 30.f; // lethal
	if (target->GetTaunting()) score += 12.f;

	return score;
}

float Battleground::ScoreBuff(const Card& card, BattleEntity* target, int manaLeft)
{
	if (!target || card.GetManaCost() > manaLeft) return -1.f;
	return 8.f + target->GetAttack() * 2.f;         // buff your best attacker
}

float Battleground::ScoreHeroAbility(BattleEntity* target, int cost, int manaLeft)
{
	if (!target || cost > manaLeft) return -1.f;

	if (enemyHero
		&& dynamic_cast<BattleHero*>(enemyHero.get())->GetAbilityUsed())
		return -1.f;

	float score = 14.f + target->GetAttack() * 2.f;
	if (target->GetHealth() <= heroAbilities[enemyHero->GetType()].statModifier) score += 30.f;
	return score;
}

BattleEntity* Battleground::BestIn(const std::vector<std::shared_ptr<BattleEntity>>& pool,
	const std::function<float(BattleEntity*)>& scoreFn)
{
	BattleEntity* best = nullptr;
	float bestS = -1.f;

	for (auto& entity : pool)
	{
		if (!entity || entity->HasDied()) continue;
		float score = scoreFn(entity.get());
		if (score > bestS)
		{ 
			bestS = score;
			best = entity.get();
		}
	}
	return best;
}


EnemyAction Battleground::PickEnemyPlay(int mana)
{
	EnemyAction best;

	// hero ability
	auto* hero = dynamic_cast<BattleHero*>(enemyHero.get());

	if (hero)
	{
		auto* target = BestIn(army, [&](BattleEntity* e) {
			return ScoreHeroAbility(e, hero->GetAbilityManaCost(), mana);
			});
		float score = ScoreHeroAbility(target, hero->GetAbilityManaCost(), mana);
		if (score > best.score) best = { EnemyActionKind::HeroAbility, nullptr, target, score };
	}

	for (auto& card : enemyCardDeck->GetHand())
	{
		if (!card) continue;

		if (card->GetCardType() == CardType::Entity)
		{
			float score = ScoreEntity(*card, mana, (int)enemyArmy.size());
			if (score > best.score) best = { EnemyActionKind::Entity, card, nullptr, score };
		}
		else if (IsSpellLike(card->GetCardType()))
		{
			bool buff = card->GetCardType() == CardType::Buff ||
				card->GetSubType() == CardType::Buff;

			auto& pool = buff ? enemyArmy : army;

			auto* target = BestIn(pool, [&](BattleEntity* entity) {
				return buff ? ScoreBuff(*card, entity, mana) : ScoreDebuff(*card, entity, mana);
				});

			float score = buff ? ScoreBuff(*card, target, mana) : ScoreDebuff(*card, target, mana);

			if (score > best.score) best = { buff ? EnemyActionKind::Buff
												  : EnemyActionKind::Debuff, card, target, score };
		}
	}

	return best;
}


void Battleground::EnemyBattlePhase()
{
	if (enemyFinishedTurn)
		return;

	if (enemyTurnPanel.empty())
	{
		enemyFinishedTurn = true;
		return;
	}

	// Still animating the current attacker
	if (attacker && !attacker->GetHadItsTurn())
		return;

	// Current attacker just finished
	if (attacker && attacker->GetHadItsTurn())
	{
		attacker = nullptr;
		defender = nullptr;
		turnsCounter++;
	}

	// No one left
	if (turnsCounter >= static_cast<int>(enemyTurnPanel.size()))
	{
		enemyFinishedTurn = true;
		turnsCounter = 0;
		return;
	}

	// Start the next unit's attack ONCE
	selectedEntity = enemyTurnPanel[turnsCounter];
	if (!selectedEntity || selectedEntity->dead)
	{
		turnsCounter++;
		return;
	}

	selectedEntityIndex = selectedEntity->GetCellIndex();
	defender = PickAttackTarget(selectedEntity);

	if (!defender)
	{
		selectedEntity->EndTurn();   // or however you mark "had its turn"
		turnsCounter++;
		return;
	}

	defender->SetEnemy(make_shared<Cell>(grid->at(selectedEntityIndex)));

	int defenderIndex = defender->GetCellIndex();
	selectedEntity->Attack(make_shared<Cell>(grid->at(defenderIndex)));

	attackingEntityIndex = selectedEntity->GetMoveToIndex();
	if (attackingEntityIndex < 0)
	{
		turnsCounter++;
		return;
	}

	if (attackingEntityIndex != selectedEntityIndex)
	{
		grid->at(selectedEntityIndex).entity = nullptr;
		grid->at(selectedEntityIndex).color = basicCellColor;
		grid->at(attackingEntityIndex).entity = selectedEntity;
		grid->at(attackingEntityIndex).color = mouseOverCellColor;
		selectedEntityIndex = attackingEntityIndex;
	}

	attacker = grid->at(selectedEntityIndex).entity;
	sortByPosition(battleEntities);
}

vector<shared_ptr<BattleEntity>> Battleground::EnemyTargets()
{
	std::vector<std::shared_ptr<BattleEntity>> pool;
	for (auto& enemy : army)
		if (enemy && !enemy->HasDied())
		{
			pool.push_back(enemy);
		}


	if (danteHero && !danteHero->HasDied())
	{
		pool.push_back(danteHero);
	}
	return pool;
}

float Battleground::ScoreAttackTarget(BattleEntity& attacker, BattleEntity& target)
{
	if (target.HasDied()) return -1.f;

	bool isHero = (&target == danteHero.get());
	int dmg = attacker.GetAttack();
	bool lethal = target.GetHealth() <= dmg;
	int health = attacker.GetHealth();

	float score = 0.f;

	if (isHero && lethal)	
		score += 100.f; // enemy hero dies, best outcome
	else if (isHero)			
		score += 15.f + (20 - target.GetHealth()) * 1.5f;

	if (target.GetTaunting())
		score += 50.f;

	if (!isHero)
	{
		if (lethal)
			score += 35.f;

		score += target.GetAttack() * 3.f;
		score += (10 - target.GetHealth()) * 1.5f;

		if (target.GetSkipTurn()) // avoid targets that will skip their next turn
			score -= 20.f;

		if (health <= target.GetAttack()) // avoid deadly targets
			score -= 15.f;
	}


	return score;
}

shared_ptr<BattleEntity> Battleground::PickAttackTarget(const std::shared_ptr<BattleEntity>& attacker)
{
	std::shared_ptr<BattleEntity> best;
	float bestScore = -1.f;

	auto consider = [&](const shared_ptr<BattleEntity>& entity)
		{
			if (!entity || entity->HasDied())
				return;

			float score = ScoreAttackTarget(*attacker, *entity);
			if (score > bestScore)
			{
				bestScore = score;
				best = entity;
			}
		};

	bool tauntUp = false;

	for (auto& entity : army)
		if (entity && entity->GetTaunting()) { tauntUp = true; break; }

	for (auto& entity : army)
	{
		if (tauntUp && entity && !entity->GetTaunting())
			continue;

		consider(entity);
	}

	if (!tauntUp)
		consider(danteHero);

	return best;
}

void Battleground::EnemyEndTurn()
{
	endTurnButtonPressed_enemy = true;

	if (currentMana < MAX_MANA)
	{
		currentMana++;

		float x = manaStones[currentMana - 1].dest.x + manaStones[currentMana - 1].dest.width / 2 - manaShimmer->GetDest().width / 2;
		float y = manaStones[currentMana - 1].dest.y + manaStones[currentMana - 1].dest.height / 2 - manaShimmer->GetDest().height / 2;

		manaShimmer->SetPosition(x, y);
		manaShimmer->SetDrawOnce(true);
	}
	usedMana = 0;
	availableMana = currentMana;
	enemyTurn = false;
	showPlayerTurnBanner = 2.0f;
	showBanner = true;



	cardDrawnTimer = 1.5f;
	ClearDeadEntities(army);
	ClearDeadEntities(enemyArmy);
	selectedEntity.reset();
	attacker.reset();
	defender.reset();
	hoveredEntity = nullptr;

	cardFinishedDrawing = false;
	deckEmpty = cardDeck->DeckEmpty();
	ResetSkipTurnState(enemyArmy);
	dynamic_cast<BattleHero*>(enemyHero.get())->ResetAbilityUsed();

	ResetHadItsTurnState(army);
}

void Battleground::ResetSkipTurnState(vector<shared_ptr<BattleEntity>>& army)
{
	for (auto& entity : army)
		entity->AlterState(Stat::SkipTurn, false);
}

void Battleground::ResetHadItsTurnState(vector<shared_ptr<BattleEntity>>& army)
{
	for (auto& entity : army)
		entity->AlterState(Stat::FinishedTurn, false);
}

void Battleground::SetBackground(string background)
{
	//this->background.reset();
	this->background->LoadLayers(background);
}

void Battleground::SetBackground(Background& background)
{
	/*this->background = &background;*/
}

void Battleground::ShowMenu()
{
	showMenu_ = true;
	cout << "Show Battleground menu" << endl;
}

void Battleground::SetBattleMode(bool* battleMode, function<void()> callback)
{
	battleMode_ = battleMode;
	backToEditorCallback = std::move(callback);
}

bool Battleground::IsSpellSeekingTarget()
{
	
	if (danteHero && dynamic_cast<BattleHero*>(danteHero.get())->GetSeekTarget())
		return true;
	if (enemyHero && dynamic_cast<BattleHero*>(enemyHero.get())->GetSeekTarget())
		return true;
	return selectedCard && IsSpellLike(selectedCard->GetCardType());
	
}