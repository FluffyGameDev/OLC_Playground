#pragma once

#include "olcPixelGameEngine.h"
#include <ui/uielement.h>
#include <stack>

class Maze : public UIElement
{
public:
    Maze(const olc::vi2d& position);

    void Draw(olc::PixelGameEngine& engine) override;
    void Update(olc::PixelGameEngine& engine, float deltaTime) override;

private:
    bool FindAdjacentPosition(const olc::vi2d& previousPosition, const olc::vi2d& currentPosition, olc::vi2d& foundPosition);

    enum class CellType : uint8_t
    {
        Wall,
        Corridor
    };

    static constexpr uint32_t K_GRID_WIDTH{ 65 };
    static constexpr uint32_t K_GRID_HEIGHT{ 65 };
    static constexpr uint32_t K_CELL_SIZE{ 4 };
    
    CellType m_Cells[K_GRID_WIDTH][K_GRID_HEIGHT];
    std::stack<olc::vi2d> m_PositionStack;
    olc::vi2d m_CurrentPosition;
    olc::vi2d m_StartPosition;
    olc::vi2d m_EndPosition;
    float m_TimeSinceLastFrame;
    float m_FrameLength;
    bool m_IsGeneratingMaze;
};