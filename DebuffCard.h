#pragma once
#include "SpellCard.h"
class DebuffCard :
    public SpellCard
{
public:
    DebuffCard() = default;
    DebuffCard(shared_ptr<TextureManager> textureManager);

    ~DebuffCard() = default;

    void PlayCard(BattleEntity& target) override;
};

