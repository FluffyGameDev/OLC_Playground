#include <ui/cellularautomaton.h>


CellType::Transition::Transition(uint8_t destinationCellID, uint8_t checkedCellID, uint8_t requiredCount)
    : DestinationCellID{ destinationCellID }
    , CheckedCellID{ checkedCellID }
    , RequiredCount{ requiredCount }
{
}



CellularAutomaton::CellularAutomaton(const olc::vi2d& position)
    : m_CellStates{ 0 }
    , m_Position{ position }
    , m_CurrentBuffer{ 0 }
    , m_TimeSinceLastFrame{ 0.0f }
    , m_FrameLength{ 0.1f }
    , m_CurrentBrush{ 0 }
    , m_IsSimulationPlaying{ false }
{
}

void CellularAutomaton::Draw(olc::PixelGameEngine& engine)
{
    engine.DrawRect(m_Position, { K_GRID_WIDTH * K_CELL_SIZE + 1, K_GRID_HEIGHT * K_CELL_SIZE + 1 });
    for (int x = 0; x < K_GRID_WIDTH; x++)
    {
        for (int y = 0; y < K_GRID_HEIGHT; y++)
        {
            engine.FillRect({ m_Position.x + 1 + x * (int)K_CELL_SIZE, m_Position.y + 1 + y * (int)K_CELL_SIZE }, { K_CELL_SIZE, K_CELL_SIZE }, m_Cells[m_CellStates[m_CurrentBuffer][x][y]].Color);
        }
    }
}

void CellularAutomaton::Update(olc::PixelGameEngine& engine, float deltaTime)
{
    if (engine.GetMouse(0).bHeld)
    {
        int32_t x{ (engine.GetMouseX() - m_Position.x - 1) / (int)K_CELL_SIZE };
        int32_t y{ (engine.GetMouseY() - m_Position.y - 1) / (int)K_CELL_SIZE };
        if (x >= 0 && x < K_GRID_WIDTH && y >= 0 && y < K_GRID_HEIGHT)
        {
            m_CellStates[m_CurrentBuffer][x][y] = m_Cells[m_CurrentBrush].CellID;
        }
    }

    if (m_IsSimulationPlaying)
    {
        m_TimeSinceLastFrame += deltaTime;
        if (m_TimeSinceLastFrame >= m_FrameLength)
        {
            m_TimeSinceLastFrame -= m_FrameLength;

            uint32_t newBuffer{ (m_CurrentBuffer + 1) % K_BUFFER_COUNT };

            for (uint32_t i = 0; i < K_GRID_WIDTH; ++i)
            {
                for (uint32_t j = 0; j < K_GRID_HEIGHT; ++j)
                {
                    const CellType& currentCell{ m_Cells[m_CellStates[m_CurrentBuffer][i][j]] };
                    m_CellStates[newBuffer][i][j] = currentCell.DefaultTransitionCellID;

                    for (const CellType::Transition& transition : currentCell.Transitions)
                    {
                        uint32_t neighborCount{ CountNeighborCellsOfType(i, j, transition.CheckedCellID) };
                        if (neighborCount == transition.RequiredCount)
                        {
                            m_CellStates[newBuffer][i][j] = transition.DestinationCellID;
                        }
                    }
                }
            }

            m_CurrentBuffer = newBuffer;
        }
    }
}

olc::Pixel CellularAutomaton::GetCurrentBrushColor() const
{
    return m_Cells[m_CurrentBrush].Color;
}

void CellularAutomaton::Clear()
{
    for (int x = 0; x < K_GRID_WIDTH; x++)
    {
        for (int y = 0; y < K_GRID_HEIGHT; y++)
        {
            m_CellStates[m_CurrentBuffer][x][y] = false;
        }
    }
}

void CellularAutomaton::CycleToNextBrush()
{
    m_CurrentBrush = (m_CurrentBrush + 1) % m_Cells.size();
}

void CellularAutomaton::Randomize()
{
    for (int x = 0; x < K_GRID_WIDTH; x++)
    {
        for (int y = 0; y < K_GRID_HEIGHT; y++)
        {
            m_CellStates[m_CurrentBuffer][x][y] = static_cast<uint8_t>(rand() % m_Cells.size());
        }
    }
}

void CellularAutomaton::SetupCellConfigs(const std::vector<CellType>& cellConfig)
{
    m_Cells = cellConfig;
    m_CurrentBrush = 0;
    Clear();
}

void CellularAutomaton::ToggleSimulation()
{
    m_IsSimulationPlaying = !m_IsSimulationPlaying;
    m_TimeSinceLastFrame = 0.0f;
}

uint32_t CellularAutomaton::CountNeighborCellsOfType(int32_t i, int32_t j, uint8_t cellID)
{
    uint32_t neighborCount{ 0 };
    if (i > 0 && j > 0 && m_CellStates[m_CurrentBuffer][i - 1][j - 1] == cellID) ++neighborCount;
    if (i > 0 && m_CellStates[m_CurrentBuffer][i - 1][j] == cellID) ++neighborCount;
    if (i > 0 && j < K_GRID_HEIGHT - 1 && m_CellStates[m_CurrentBuffer][i - 1][j + 1] == cellID) ++neighborCount;
    if (j > 0 && m_CellStates[m_CurrentBuffer][i][j - 1] == cellID) ++neighborCount;
    if (j < K_GRID_HEIGHT - 1 && m_CellStates[m_CurrentBuffer][i][j + 1] == cellID) ++neighborCount;
    if (i < K_GRID_WIDTH - 1 && j > 0 && m_CellStates[m_CurrentBuffer][i + 1][j - 1] == cellID) ++neighborCount;
    if (i < K_GRID_WIDTH - 1 && m_CellStates[m_CurrentBuffer][i + 1][j] == cellID) ++neighborCount;
    if (i < K_GRID_WIDTH - 1 && j < K_GRID_HEIGHT - 1 && m_CellStates[m_CurrentBuffer][i + 1][j + 1] == cellID) ++neighborCount;

    return neighborCount;
}