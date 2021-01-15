#include <ui/maze.h>


Maze::Maze(const olc::vi2d& screenPosition)
    : m_Cells{ CellType::Wall }
    , m_StartPosition{ 1, 1 }
    , m_EndPosition{ K_GRID_WIDTH - 2, K_GRID_HEIGHT - 2 }
    , m_TimeSinceLastFrame{ 0.0f }
    , m_FrameLength{ 0.01f }
    , m_IsGeneratingMaze{ false }
{
    m_CurrentPosition = m_StartPosition;
    m_PositionStack.push(m_StartPosition);
    m_Cells[m_StartPosition.x][m_StartPosition.y] = CellType::Corridor;
}

void Maze::Draw(olc::PixelGameEngine& engine)
{
    //TODO: REFACTOR. There's no need to refresh all pixels each frame.
    for (int x = 0; x < K_GRID_WIDTH; x++)
    {
        for (int y = 0; y < K_GRID_HEIGHT; y++)
        {
            olc::Pixel color{ olc::BLANK };
            switch (m_Cells[x][y])
            {
                case CellType::Wall: { color = olc::WHITE; break; }
                case CellType::Corridor: { color = olc::BLACK; break; }
            }

            engine.FillRect({ 2 + x * (int)K_CELL_SIZE, 2 + y * (int)K_CELL_SIZE }, { K_CELL_SIZE, K_CELL_SIZE }, color);
        }
    }

    engine.FillRect({ 2 + m_StartPosition.x * (int)K_CELL_SIZE, 2 + m_StartPosition.y * (int)K_CELL_SIZE }, { K_CELL_SIZE, K_CELL_SIZE }, olc::RED);
    engine.FillRect({ 2 + m_EndPosition.x * (int)K_CELL_SIZE, 2 + m_EndPosition.y * (int)K_CELL_SIZE }, { K_CELL_SIZE, K_CELL_SIZE }, olc::GREEN);
    engine.FillRect({ 2 + m_CurrentPosition.x * (int)K_CELL_SIZE, 2 + m_CurrentPosition.y * (int)K_CELL_SIZE }, { K_CELL_SIZE, K_CELL_SIZE }, olc::RED);
}

void Maze::Update(olc::PixelGameEngine& engine, float deltaTime)
{
    if (engine.GetKey(olc::Key::SPACE).bPressed)
    {
        m_IsGeneratingMaze = !m_IsGeneratingMaze;
    }

    if (m_IsGeneratingMaze)
    {
        m_TimeSinceLastFrame += deltaTime;
        if (m_TimeSinceLastFrame >= m_FrameLength)
        {
            m_TimeSinceLastFrame -= m_FrameLength;

            olc::vi2d previousPosition{ m_CurrentPosition };
            if (!m_PositionStack.empty())
            {
                previousPosition = m_PositionStack.top();
            }

            olc::vi2d foundPosition{};

            if (FindAdjacentPosition(previousPosition, m_CurrentPosition, foundPosition))
            {
                olc::vi2d midPoint{ (m_CurrentPosition.x + foundPosition.x) / 2,
                                    (m_CurrentPosition.y + foundPosition.y) / 2 };

                m_Cells[foundPosition.x][foundPosition.y] = CellType::Corridor;
                m_Cells[midPoint.x][midPoint.y] = CellType::Corridor;

                m_PositionStack.push(m_CurrentPosition);
                m_CurrentPosition = foundPosition;
            }
            else
            {
                if (m_PositionStack.empty())
                {
                    m_IsGeneratingMaze = false;
                }
                else
                {
                    m_CurrentPosition = m_PositionStack.top();
                    m_PositionStack.pop();
                }
            }
        }
    }
}

bool Maze::FindAdjacentPosition(const olc::vi2d& previousPosition, const olc::vi2d& currentPosition, olc::vi2d& foundPosition)
{
    int32_t rnd{ rand() % 4 };
    olc::vi2d direction{};
    switch (rnd)
    {
        case 0: { direction = { 2, 0 }; break; }
        case 1: { direction = { 0, 2 }; break; }
        case 2: { direction = { -2, 0 }; break; }
        case 3: { direction = { 0, -2 }; break; }
    }

    for (uint32_t i = 0; i < 4; ++i)
    {
        foundPosition = { currentPosition.x + direction.x, currentPosition.y + direction.y };
        if (foundPosition.x >= 0 && foundPosition.x < K_GRID_WIDTH && 
            foundPosition.y >= 0 && foundPosition.y < K_GRID_HEIGHT &&
            (foundPosition.x != previousPosition.x || foundPosition.y != previousPosition.y) &&
            m_Cells[foundPosition.x][foundPosition.y] == CellType::Wall)
        {
            return true;
        }

        int32_t tmp{ direction.x };
        direction.x = -direction.y;
        direction.y = tmp;
    }

    return false;
}