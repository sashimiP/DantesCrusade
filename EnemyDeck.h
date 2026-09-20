#pragma once
#include "Deck.h"

using Utilities::IsSpellLike;

class EnemyDeck :
    public Deck
{
public:
    EnemyDeck() = default;
    EnemyDeck(shared_ptr<TextureManager> textureManager);

    virtual ~EnemyDeck() = default;

    void DrawStartingHand();
    void Update();
    void DrawCard() override;
    void ReadjustHand() override;

    void DisplayHand();

    shared_ptr<Card> PlayChosenCard(const std::shared_ptr<Card>& wanted);
    std::shared_ptr<Card> PlayEntityCard(int availableMana);
    std::shared_ptr<Card> PlayBuffCard(int availableMana);
    std::shared_ptr<Card> PlayDebuffCard(int availableMana);

    bool FinishedLooking() const;
    void StartLooking();

    int GetIterCounter();

    void ShowHand();

private:
    Texture2D* cardBackTexture;
    Rectangle cardBackTextureSource;

    int iterCounter{0};
    float lookAtCardTimer = 0.5f;
    bool finishedLooking{ false };
    bool showHand{ false };

};

