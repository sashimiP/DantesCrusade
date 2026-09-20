#pragma once
#include "SpellCard.h"
class BuffCard :
    public SpellCard
{
public:
    BuffCard() = default;
    BuffCard(shared_ptr<TextureManager> textureManager);

    ~BuffCard() = default;

    void PlayCard(BattleEntity& target) override;

};

