#include <ui/maze.h>


Maze::Maze(const olc::vi2d& screenPosition)
    : m_Cells{ CellType::Wall }
    , m_VisitedCells{ false }
    , m_StartPosition{ 1, 1 }
    , m_EndPosition{ K_GRID_WIDTH - 2, K_GRID_HEIGHT - 2 }
    , m_PathStart{ 0, 0 }
    , m_PathEnd{ 0, 0 }
    , m_TimeSinceLastFrame{ 0.0f }
    , m_FrameLength{ 0.01f }
    , m_IsGeneratingMaze{ false }
    , m_IsSelectingPath{ false }
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
                case CellType::Wall: { color = olc::BLACK; break; }
                case CellType::Corridor: { color = olc::WHITE; break; }
            }

            engine.FillRect({ 2 + x * (int)K_CELL_SIZE, 2 + y * (int)K_CELL_SIZE }, { K_CELL_SIZE, K_CELL_SIZE }, color);
        }
    }

    engine.FillRect({ 2 + m_CurrentPosition.x * (int)K_CELL_SIZE, 2 + m_CurrentPosition.y * (int)K_CELL_SIZE }, { K_CELL_SIZE, K_CELL_SIZE }, olc::RED);

    for (const olc::vi2d& path : m_Path)
    {
        engine.FillRect({ 2 + path.x * (int)K_CELL_SIZE, 2 + path.y * (int)K_CELL_SIZE }, { K_CELL_SIZE, K_CELL_SIZE }, olc::RED);
    }

    if (m_IsSelectingPath)
    {
        engine.DrawLine(m_PathStart, engine.GetMousePos(), olc::RED);
    }
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
    else
    {
        if (engine.GetMouse(0).bPressed)
        {
            m_IsSelectingPath = true;
            m_PathStart = engine.GetMousePos();
            m_Path.clear();
        }
        else if (engine.GetMouse(0).bReleased)
        {
            m_IsSelectingPath = false;
            m_PathEnd = engine.GetMousePos();

            UpdatePath();
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

bool Maze::FindAdjacentUnusedPath(const olc::vi2d& currentPosition, olc::vi2d& foundPosition)
{
    //TODO: don't start at a random direction
    int32_t rnd{ rand() % 4 };
    olc::vi2d direction{};
    switch (rnd)
    {
        case 0: { direction = { 1, 0 }; break; }
        case 1: { direction = { 0, 1 }; break; }
        case 2: { direction = { -1, 0 }; break; }
        case 3: { direction = { 0, -1 }; break; }
    }

    for (uint32_t i = 0; i < 4; ++i)
    {
        foundPosition = { currentPosition.x + direction.x, currentPosition.y + direction.y };
        if (foundPosition.x >= 0 && foundPosition.x < K_GRID_WIDTH &&
            foundPosition.y >= 0 && foundPosition.y < K_GRID_HEIGHT &&
            m_Cells[foundPosition.x][foundPosition.y] == CellType::Corridor &&
            !m_VisitedCells[foundPosition.x][foundPosition.y])
        {
            return true;
        }

        int32_t tmp{ direction.x };
        direction.x = -direction.y;
        direction.y = tmp;
    }

    return false;
}

void Maze::UpdatePath()
{
    //TODO: use separate thread.
    //TODO: try recursion?

    std::memset(m_VisitedCells, false, sizeof(m_VisitedCells));

    olc::vi2d currentPosition{ (m_PathStart - olc::vi2d{ 2, 2 }) / K_CELL_SIZE};
    olc::vi2d endPosition{ (m_PathEnd - olc::vi2d{ 2, 2 }) / K_CELL_SIZE };
    
    if (m_Cells[currentPosition.x][currentPosition.y] == CellType::Wall ||
        m_Cells[endPosition.x][endPosition.y] == CellType::Wall)
    {
        return;
    }

    olc::vi2d nextPosition{};
    m_Path.push_back(currentPosition);
    m_VisitedCells[currentPosition.x][currentPosition.y] = true;
    while (currentPosition != endPosition && !m_Path.empty())
    {
        if (FindAdjacentUnusedPath(currentPosition, nextPosition))
        {
            currentPosition = nextPosition;
            m_VisitedCells[currentPosition.x][currentPosition.y] = true;
            m_Path.push_back(currentPosition);
        }
        else
        {
            m_Path.pop_back();
            currentPosition = m_Path.back();
        }
    }
}