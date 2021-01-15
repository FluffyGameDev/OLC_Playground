#pragma once

#include "olcPixelGameEngine.h"
#include <ui/uielement.h>

struct CellType
{
    struct Transition
    {
        Transition(uint8_t destinationCellID, uint8_t checkedCellID, uint8_t requiredCount);

        uint8_t DestinationCellID;
        uint8_t CheckedCellID;
        uint8_t RequiredCount;
    };

    uint8_t CellID;
    uint8_t DefaultTransitionCellID;
    olc::Pixel Color;
    std::vector<Transition> Transitions;
};

class CellularAutomaton : public UIElement
{
public:
    CellularAutomaton(const olc::vi2d& position);

    void Draw(olc::PixelGameEngine& engine) override;
    void Update(olc::PixelGameEngine& engine, float deltaTime) override;

    inline bool IsSimulationPlaying() const { return m_IsSimulationPlaying;  }
    olc::Pixel GetCurrentBrushColor() const;

    void Clear();
    void CycleToNextBrush();
    void Randomize();
    void SetupCellConfigs(const std::vector<CellType>& cellConfig);
    void ToggleSimulation();

private:
    uint32_t CountNeighborCellsOfType(int32_t i, int32_t j, uint8_t cellID);

    static constexpr uint32_t K_BUFFER_COUNT{ 2 };
    static constexpr uint32_t K_GRID_WIDTH{ 64 };
    static constexpr uint32_t K_GRID_HEIGHT{ 64 };
    static constexpr uint32_t K_CELL_SIZE{ 4 };

    uint8_t m_CellStates[K_BUFFER_COUNT][K_GRID_WIDTH][K_GRID_HEIGHT];
    std::vector<CellType> m_Cells;
    olc::vi2d m_Position;
    uint32_t m_CurrentBuffer;
    uint8_t m_CurrentBrush;
    float m_TimeSinceLastFrame;
    float m_FrameLength;
    bool m_IsSimulationPlaying;

};