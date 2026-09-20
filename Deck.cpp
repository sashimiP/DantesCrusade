#include "Deck.h"

Deck::Deck(shared_ptr<TextureManager> textureManager):
	textureManager(textureManager)
{

	defaultCard_x = SCR_WIDTH / 2;
	defaultCard_y = SCR_HEIGHT - 180;

	sparklingEffect = new AnimatedAsset("sparkling", 24, textureManager);
	cardDrawSparkling.reserve(numOfCardSparkles);

	for (int i = 0; i < numOfCardSparkles; i++)
		cardDrawSparkling.emplace_back(new AnimatedAsset("sparkling", 24, textureManager));

}

void Deck::SetDeck(vector<string> cardDeck)
{

	//cards.clear();

	if (cardDeck.empty()) return;
	availableCards = cardDeck;

	//const float cardW = 120.f;          // same as the base width you use in SetCard
	//const float gap = 1.f;           // space between cards
	//const float totalWidth = availableCards.size() * cardW
	//	+ (availableCards.size() - 1) * gap;

	//// Center the whole hand on the screen
	//float startX = (SCR_WIDTH - totalWidth) * 0.5f;
	//float y = SCR_HEIGHT - 180.f;   // or whatever you prefer

	//for (size_t i = 0; i < availableCards.size(); ++i)
	//{
	//	Card card(textureManager);
	//	float x = startX + i * (cardW + gap);
	//	card.SetCard(availableCards[i], x, y);
	//	cards.emplace_back(std::move(card));
	//}

	ShuffleDeck();

	DrawStartingHand();
}

void Deck::SetDeck(vector<string> cardDeck, vector<string>spellList)
{
	
	//cards.clear();

	if (cardDeck.empty()) return;
	availableCards = cardDeck;

	//const float cardW = 120.f;          // same as the base width you use in SetCard
	//const float gap = 1.f;           // space between cards
	//const float totalWidth = availableCards.size() * cardW
	//	+ (availableCards.size() - 1) * gap;

	//// Center the whole hand on the screen
	//float startX = (SCR_WIDTH - totalWidth) * 0.5f;
	//float y = SCR_HEIGHT - 180.f;   // or whatever you prefer

	//for (size_t i = 0; i < availableCards.size(); ++i)
	//{
	//	Card card(textureManager);
	//	float x = startX + i * (cardW + gap);
	//	card.SetCard(availableCards[i], x, y);
	//	cards.emplace_back(std::move(card));
	//}

	SetSpells(spellList);

	ShuffleDeck();

	DrawStartingHand();
}

void Deck::SetSpells(vector<string> spellList)
{
	if (spellList.empty()) return;

	availableSpells = spellList;

	availableCards.insert(availableCards.end(), availableSpells.begin(), availableSpells.end());
}

void Deck::ShuffleDeck()
{
	if (availableCards.empty()) return;

	static std::random_device rd;
	static std::mt19937 gen(rd());  // keep the generator alive across calls

	std::shuffle(availableCards.begin(), availableCards.end(), gen);
}

shared_ptr<Card> Deck::CreateCard(string type)
{
	auto it = find(availableSpells.begin(), availableSpells.end(), type);

	if (it != availableSpells.end())
	{
		CardType spellType = Utilities::spellStats[type].cardType;
		switch (spellType)
		{
		case CardType::Buff:
			return make_shared<BuffCard>(textureManager); break;
		case CardType::Debuff:
			return make_shared<DebuffCard>(textureManager); break;
		case CardType::StateAltering:
			return make_shared<StateAlteringCard>(textureManager); break;
		}
		
	}


	return make_shared<Card>(textureManager);
}

void Deck::DrawStartingHand()
{
	const float cardW = 120.f;          // same as the base width you use in SetCard
	const float totalWidth = startingNumberCards * cardW * 0.66;

	// Center the whole hand on the screen
	float startX = SCR_WIDTH/2 - totalWidth/2;
	float y = SCR_HEIGHT - 110.f;   // or whatever you prefer

	int handSize = (startingNumberCards < availableCards.size()) ? startingNumberCards : availableCards.size();

	cout <<"Available CARDS: "<< availableCards.size() << endl;
	for (size_t i = 0; i < handSize; ++i)
	{
		shared_ptr<Card> card = CreateCard(availableCards[i]);
		float x = startX + (cardW * 0.66) * i;
		card->SetCard(availableCards[i], x, y);
		hand.emplace_back(std::move(card));
		
	}
	availableCards.erase(availableCards.begin(), availableCards.begin() + handSize);
	cout << "Available CARDS: " << availableCards.size() << endl;
}

void Deck::Update()
{
	if (hand.empty()) return;


	Card* newHovered = nullptr;
	int hoveredIndex = 0;

	// Find which card (if any) the mouse is over.
	// We check from front to back so the top-most card wins.
	for (int i = static_cast<int>(hand.size()) - 1; i >= 0; --i)
	{
		hand[i]->Update();

		if (!selectedCard && CheckCollisionPointRec(GetMousePosition(), hand[i]->GetRectDest()))
		{
			newHovered = hand[i].get();
			hoveredIndex = i;
			break;
		}
	}

	// Changed hovered card?
	if (!selectedCard && (newHovered != mousedOverCard))
	{
		if (mousedOverCard)
			mousedOverCard->PlaceInHand();

		mousedOverCard = newHovered;
		mousedOverCardIndex = hoveredIndex;

		if (mousedOverCard)
		{
			mousedOverCard->MousedOver();

		}
	}

	if (showAfterDraw)
	{
		showAfterDrawTimer -= GetFrameTime();
		for (auto& sparkle : cardDrawSparkling)
			sparkle->Update();

		if (showAfterDrawTimer <= 0.0f)
		{

			float cardW = 120.f;
			float x = hand.empty() ? SCR_WIDTH / 2.f : hand.back()->GetRectDest().x + cardW * 0.66f;
			float y = hand.empty() ? SCR_HEIGHT - 150.f : hand.back()->GetRectDest().y;

			drawnCard->SetCard(x, y);          // or whatever positioning you need

			hand.emplace_back(std::move(drawnCard));

			availableCards.pop_back();
			ReadjustHand();

			showAfterDraw = false;
			drawnCard.reset();                 // instead of delete
			
		}
	}
}

void Deck::DisplayHand()
{
	if (hand.empty())
		return;

	for (auto& card : hand)
		if(!card->GetMouseOver())
			card->Display();

	if (mousedOverCard)
	{
		mousedOverCard->Display();
	}
	if (drawnCard && showAfterDraw)
	{
		drawnCard->Display();

		for (auto& sparkle : cardDrawSparkling)
			sparkle->Draw();

	}
}

void Deck::LeftMouseButtonDown()
{
	if (!mousedOverCard) return;

	selectedCard = mousedOverCard;
	selectedCard->Select();
	selectedCardIndex = mousedOverCardIndex;
}

void Deck::LeftMouseButtonReleased(bool placeCard)
{
	PlayCard(placeCard);
}

void Deck::PlayCard(bool placeCard)
{
	if (!selectedCard) return;


	selectedCard->Deselect();

	if (placeCard)
	{
		hand.erase(hand.begin() + selectedCardIndex);
		ReadjustHand();
	}

	mousedOverCard = nullptr;
	mousedOverCardIndex = -1;
	selectedCard = nullptr;
	selectedCardIndex = -1;
}

void Deck::ReadjustHand()
{
	if (hand.empty()) return;

	const float cardW = 120.f;          // same as the base width you use in SetCard
	const float totalWidth = hand.size() * cardW * 0.66;

	// Center the whole hand on the screen
	float startX = SCR_WIDTH/2 - totalWidth/2;
	float y = SCR_HEIGHT - 110.f;   // or whatever you prefer

	for (size_t i = 0; i < hand.size(); ++i)
	{
		float x = startX + (cardW * 0.66) * i;
		hand[i]->SetX(x);
		hand[i]->SetY(y);

		hand[i]->SetBaseX(x);
		hand[i]->SetBaseY(y);
	}
}

void Deck::DrawCard()
{
	if (availableCards.empty()) return;

	cout << "Display card!" << endl;

	//float cardW = 120.f;
	//float x = hand.back().GetRectDest().x + cardW * 0.66;
	//float y = hand.back().GetRectDest().y;

	float cardW = 240.f;
	float x = SCR_WIDTH - cardW * 2;
	float y = SCR_HEIGHT / 2 - 180.f;
	drawnCard = CreateCard(availableCards.back());
	drawnCard->Drawn(availableCards.back(), x, y);

	float sparkleWidth{ cardDrawSparkling[0]->GetDest().width };
	float sparkleHeight{ cardDrawSparkling[0]->GetDest().height };

	Vector2 topRight{
		x + drawnCard->GetRectDest().width - sparkleWidth,
		y + sparkleHeight * 0.5 
	};

	Vector2 topLeft{
		x,
		y - sparkleHeight * 0.5
	};

	Vector2 bottomRight{
		x + drawnCard->GetRectDest().width - sparkleWidth,
		y + drawnCard->GetRectDest().height - sparkleHeight
	};
	Vector2 bottomLeft{ 
		x, 
		y + drawnCard->GetRectDest().height * 0.5 - sparkleHeight * 0.5
	};
	
	cardDrawSparkling[0]->SetPosition(topRight.x, topRight.y);
	cardDrawSparkling[1]->SetPosition(topLeft.x, topLeft.y);
	cardDrawSparkling[2]->SetPosition(bottomRight.x, bottomRight.y);
	cardDrawSparkling[3]->SetPosition(bottomLeft.x, bottomLeft.y);

	showAfterDraw = true;
	showAfterDrawTimer = 2.0f;   // reset timer here
	//card.SetCard(availableCards.back(), x, y);
	//hand.emplace_back(std::move(card));

	//ReadjustHand();
	//availableCards.pop_back();
	
}

void Deck::ShowDrawnCard(Card* card)
{
	if (card == nullptr) return;

	float cardW = 120.f;
	float x = SCR_WIDTH - cardW * 4;
	float y = SCR_HEIGHT / 2;
}

shared_ptr<Card> Deck::GetSelectedCard()
{
	if (selectedCardIndex < 0 ||
		selectedCardIndex >= static_cast<int>(hand.size()))
		return nullptr;

	return hand[selectedCardIndex];
	
}

int Deck::GetHandSize()
{
	return hand.size();
}

int Deck::GetMousedOverManaCost()
{
	if (mousedOverCard)
		return mousedOverCard->GetManaCost();

	return -1;
}

bool Deck::DeckEmpty()
{
	return availableCards.empty();
}

bool Deck::LastCard()
{
	return (availableCards.size() == 1);
}