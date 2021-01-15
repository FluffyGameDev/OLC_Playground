#pragma once

#include "olcPixelGameEngine.h"

class UIElement
{
public:
    virtual void Draw(olc::PixelGameEngine& engine) = 0;
    virtual void Update(olc::PixelGameEngine& engine, float deltaTime) = 0;
};