#include <ui/button.h>

Button::Button(const std::string text, const olc::vi2d& position, const olc::vi2d& size, olc::Key shortcut)
    : m_Text{ text }
    , m_Position{ position }
    , m_Size{ size }
    , m_OnClickCallback{}
    , m_BackColor{ olc::WHITE }
    , m_BorderColor{ olc::BLANK }
    , m_TextColor{ olc::BLACK }
    , m_Shortcut{ shortcut }
{
}

void Button::Draw(olc::PixelGameEngine& engine)
{
    engine.FillRect(m_Position, m_Size, m_BackColor);
    if (m_BorderColor != olc::BLANK)
    {
        engine.DrawRect(m_Position, { m_Size.x - 1, m_Size.y - 1 }, m_BorderColor);
    }
    engine.DrawString(m_Position.x + 1, m_Position.y + 1, m_Text, m_TextColor);
}

void Button::Update(olc::PixelGameEngine& engine, float deltaTime)
{
    if (engine.GetKey(m_Shortcut).bPressed ||
        engine.GetMouse(0).bPressed && engine.GetMousePos().x >= m_Position.x && engine.GetMousePos().y >= m_Position.y &&
        engine.GetMousePos().x < m_Position.x + m_Size.x && engine.GetMousePos().y < m_Position.y + m_Size.y)
    {
        if (m_OnClickCallback)
        {
            m_OnClickCallback();
        }
    }
}