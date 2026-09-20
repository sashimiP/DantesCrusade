#include "EnemyDeck.h"

EnemyDeck::EnemyDeck(shared_ptr<TextureManager> textureManager) :
	Deck(textureManager)
{
	cardBackTexture = this->textureManager->Asset("card_back");
	cardBackTextureSource = { 0.0f,
		0.0f,
		static_cast<float>(cardBackTexture->width),
		-static_cast<float>(cardBackTexture->height)
	};
}

void EnemyDeck::DrawStartingHand()
{
	const float cardW = 120.f;          // same as the base width you use in SetCard
	const float totalWidth = startingNumberCards * cardW * 0.66;

	// Center the whole hand on the screen
	float startX = SCR_WIDTH / 2 - totalWidth / 2;
	float y =  -70.0f;   // or whatever you prefer

	int handSize = (startingNumberCards < availableCards.size()) ? startingNumberCards : availableCards.size();

	cout << "Available Enemy CARDS: " << availableCards.size() << endl;
	for (size_t i = 0; i < handSize; ++i)
	{
		shared_ptr<Card> card = CreateCard(availableCards[i]);
		float x = startX + (cardW * 0.66) * i;
		card->SetCard(availableCards[i], x, y);
		hand.emplace_back(std::move(card));

	}
	availableCards.erase(availableCards.begin(), availableCards.begin() + handSize);
	ReadjustHand();
	cout << "Available CARDS: " << availableCards.size() << endl;
}

void EnemyDeck::Update()
{
	if (hand.empty()) return;
}

void EnemyDeck::DrawCard()
{
	if (availableCards.empty()) return;

	const float cardW = 120.f;          // same as the base width you use in SetCard
	const float totalWidth = startingNumberCards * cardW * 0.66;
	// Center the whole hand on the screen
	float x = SCR_WIDTH / 2 - totalWidth / 2;
	float y = -30.0f;   // or whatever you prefer

	drawnCard = CreateCard(availableCards.back());
	drawnCard->SetCard(availableCards.back(), x, y);
	hand.emplace_back(std::move(drawnCard));
	availableCards.pop_back();
	ReadjustHand();
}

void EnemyDeck::ReadjustHand()
{
	if (hand.empty()) return;

	const float cardW = 120.f;          // same as the base width you use in SetCard
	const float totalWidth = hand.size() * cardW * 0.66;

	// Center the whole hand on the screen
	float startX = SCR_WIDTH / 2 - totalWidth / 2;
	float y = -70.0f;   // or whatever you prefer

	for (size_t i = 0; i < hand.size(); ++i)
	{
		float x = startX + (cardW * 0.66) * i;
		hand[i]->SetX(x);
		hand[i]->SetY(y);

		hand[i]->SetBaseX(x);
		hand[i]->SetBaseY(y);
	}
}


void EnemyDeck::DisplayHand()
{
	if (hand.empty())
		return;

	if (showHand)
	{
		for (auto& card : hand)
			if (!card->GetMouseOver())
				card->Display();
	}
	else
	{
		//Hide the enemy's hand
		for (auto& card : hand)
			DrawTexturePro(*cardBackTexture, cardBackTextureSource, card->GetRectDest(), { 0.0f, 0.0f }, 0.0f, RAYWHITE);
	}
			

}

std::shared_ptr<Card> EnemyDeck::PlayChosenCard(const std::shared_ptr<Card>& wanted)
{
	finishedLooking = false;

	if (!wanted || hand.empty())
	{
		finishedLooking = true;
		return nullptr;
	}

	if (iterCounter >= static_cast<int>(hand.size()))
	{
		iterCounter = 0;
		lookAtCardTimer = 0.5f;
		finishedLooking = true;
		return nullptr;
	}

	auto it = hand.begin() + iterCounter;
	(*it)->SetY(0.0f);

	lookAtCardTimer -= GetFrameTime();
	if (lookAtCardTimer > 0.0f)
		return nullptr;

	if (it->get() == wanted.get())
	{
		auto played = *it;
		hand.erase(it);
		ReadjustHand();
		StartLooking();
		finishedLooking = true;
		return played;
	}

	(*it)->SetY((*it)->GetBaseY());
	iterCounter++;
	lookAtCardTimer = 0.5f;
	return nullptr;
}

std::shared_ptr<Card> EnemyDeck::PlayEntityCard(int availableMana)
{
	finishedLooking = false;

	if (hand.empty())
	{
		finishedLooking = true;
		return nullptr;
	}

	// Finished browsing the whole hand
	if (iterCounter >= static_cast<int>(hand.size()))
	{
		iterCounter = 0;
		lookAtCardTimer = 0.5f;
		finishedLooking = true;   // went through the whole hand
		return nullptr;
	}

	auto it = hand.begin() + iterCounter;

	// Raise the current card while looking at it
	(*it)->SetY(0.0f);  // use your real "raised" position

	lookAtCardTimer -= GetFrameTime();
	if (lookAtCardTimer > 0.0f)
		return nullptr;   // still looking at this card

	// Timer expired – decide
	if ((*it)->GetCardType() == Utilities::CardType::Entity &&
		(*it)->GetManaCost() <= availableMana)
	{
		std::shared_ptr<Card> playedCard = *it;
		hand.erase(it);
		ReadjustHand();

		iterCounter = 0;
		lookAtCardTimer = 0.5f;
		finishedLooking = true;

		return playedCard;
	}

	// Not playable – put it back and move to the next card

	std::cout << "skip " << (*it)->GetType()
		<< " iter=" << iterCounter
		<< "/" << hand.size() << '\n';

	(*it)->SetY((*it)->GetBaseY());   // restore real hand position
	iterCounter++;
	lookAtCardTimer = 0.5f;
	return nullptr;
}

std::shared_ptr<Card> EnemyDeck::PlayBuffCard(int availableMana)
{
	finishedLooking = false;

	if (hand.empty())
	{
		finishedLooking = true;
		return nullptr;
	}

	// Finished browsing the whole hand
	if (iterCounter >= static_cast<int>(hand.size()))
	{
		iterCounter = 0;
		lookAtCardTimer = 0.5f;
		finishedLooking = true;   // went through the whole hand
		return nullptr;
	}

	auto it = hand.begin() + iterCounter;

	// Raise the current card while looking at it
	(*it)->SetY(0.0f);  // use your real "raised" position

	lookAtCardTimer -= GetFrameTime();
	if (lookAtCardTimer > 0.0f)
		return nullptr;   // still looking at this card

	bool isBuffCard = (*it)->GetCardType() == CardType::Buff || (*it)->GetSubType() == CardType::Buff;

	// Timer expired – decide
	if (isBuffCard &&
		(*it)->GetManaCost() <= availableMana)
	{
		std::shared_ptr<Card> playedCard = *it;
		hand.erase(it);
		ReadjustHand();

		iterCounter = 0;
		lookAtCardTimer = 0.5f;
		finishedLooking = true;

		return playedCard;
	}

	// Not playable – put it back and move to the next card
	(*it)->SetY((*it)->GetBaseY());   // restore real hand position
	iterCounter++;
	lookAtCardTimer = 0.5f;
	return nullptr;
}

std::shared_ptr<Card> EnemyDeck::PlayDebuffCard(int availableMana)
{
	finishedLooking = false;

	if (hand.empty())
	{
		finishedLooking = true;
		return nullptr;
	}

	// Finished browsing the whole hand
	if (iterCounter >= static_cast<int>(hand.size()))
	{
		iterCounter = 0;
		lookAtCardTimer = 0.5f;
		finishedLooking = true;   // went through the whole hand
		return nullptr;
	}

	auto it = hand.begin() + iterCounter;

	// Raise the current card while looking at it
	(*it)->SetY(0.0f);  // use your real "raised" position

	lookAtCardTimer -= GetFrameTime();
	if (lookAtCardTimer > 0.0f)
		return nullptr;   // still looking at this card

	bool isBuffCard = (*it)->GetCardType() == CardType::Debuff || (*it)->GetSubType() == CardType::Debuff;

	// Timer expired – decide
	if (isBuffCard &&
		(*it)->GetManaCost() <= availableMana)
	{
		std::shared_ptr<Card> playedCard = *it;
		hand.erase(it);
		ReadjustHand();

		iterCounter = 0;
		lookAtCardTimer = 0.5f;
		finishedLooking = true;

		return playedCard;
	}

	// Not playable – put it back and move to the next card
	(*it)->SetY((*it)->GetBaseY());   // restore real hand position
	iterCounter++;
	lookAtCardTimer = 0.5f;
	return nullptr;
}


bool EnemyDeck::FinishedLooking() const
{
	return finishedLooking;
}

void EnemyDeck::StartLooking()
{
	iterCounter = 0;
	lookAtCardTimer = 0.5f;
	finishedLooking = false;
}

int EnemyDeck::GetIterCounter()
{
	return iterCounter;
}

void EnemyDeck::ShowHand()
{
	showHand = !showHand;
}