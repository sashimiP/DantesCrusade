#pragma once
#include <string>
#include <vector>
#include<iostream>

#include "Card.h"
#include "SpellCard.h"
#include "BuffCard.h"
#include "DebuffCard.h"
#include "StateAlteringCard.h"
#include"TextureManager.h"
#include "Utilities.h"
#include "AnimatedAsset.h"

using std::string, std::vector, std::map, std::unordered_map, std::tuple, std::pair, std::cout, std::endl,
std::to_string, std::shared_ptr, std::make_shared, std::unique_ptr, std::make_unique,
Utilities::UI_Asset;

//struct Card
//{
//	string type;
//	Texture2D* texture;
//	float x;
//	float y;
//	Rectangle source;
//	Rectangle destination;
//	Vector2 center;
//};


class Deck
{
public:
	Deck() = default;
	Deck(shared_ptr<TextureManager> textureManager);
	virtual ~Deck() = default;

	void SetDeck(vector<string> cardDeck);

	void SetDeck(vector<string> cardDeck, vector<string>spellList);
	void SetSpells(vector<string> spellList);
	void ShuffleDeck();
	
	shared_ptr<Card> CreateCard(string type);
	virtual void DrawStartingHand();

	virtual void Update();
	virtual void DisplayHand();
	void LeftMouseButtonDown();
	void LeftMouseButtonReleased(bool placeCard);


	void PlayCard(bool placeCard = false);
	virtual void ReadjustHand();
	virtual void DrawCard();
	void ShowDrawnCard(Card* card);

	shared_ptr<Card> GetSelectedCard();
	int GetHandSize();
	const std::vector<std::shared_ptr<Card>>& GetHand() const { return hand; }
	int GetMousedOverManaCost();

	bool DeckEmpty();
	bool LastCard();

protected:

	vector<string> availableCards;

	vector<string> availableSpells;

	shared_ptr<TextureManager> textureManager;
	//vector<Card> cards;
	vector<shared_ptr<Card>> hand;

	float defaultCard_x;
	float defaultCard_y;
	float defaultCard_width;
	float defaultCard_heigh;
	Rectangle defaultCard_source;
	Rectangle defaultCard_dest;

	int startingNumberCards{ 6 };
	int maxNumberCardsHand{ 10 };
	int deckSize;

	Card* mousedOverCard{ nullptr };
	int mousedOverCardIndex{ -1 };


	Card* selectedCard{ nullptr };
	int selectedCardIndex{ -1 };

	bool mousedOverCard_flag{ false };

	shared_ptr<Card> drawnCard;
	float showAfterDrawTimer = 1.5f;
	bool cardDrawn{ false };
	bool showAfterDraw{ false };

	AnimatedAsset* sparklingEffect = nullptr;

	vector<AnimatedAsset*> cardDrawSparkling;
	int numOfCardSparkles = 4;

	//Rectangle cardDrawSparklingSource;

	//Rectangle cardDrawSparklingDest;

};

