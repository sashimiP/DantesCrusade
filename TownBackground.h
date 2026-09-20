#pragma once
#include "Background.h"
#include <cmath>
class TownBackground :
    public Background
{
public:
    TownBackground() = default;
    TownBackground(std::shared_ptr<TextureManager>, float, fs::path);
    //void MoveRight(float);
    //void MoveLeft(float);
    void Update(float, float) override;
    void Display() override;


private:

};

