#pragma once

#include "olcPixelGameEngine.h"
#include <ui/uielement.h>

class Button : public UIElement
{
public:
    Button(const std::string text, const olc::vi2d& position, const olc::vi2d& size, olc::Key shortcut);

    void Draw(olc::PixelGameEngine& engine) override;
    void Update(olc::PixelGameEngine& engine, float deltaTime) override;

    inline void RegisterOnClick(const std::function<void()>& onClickCallback) { m_OnClickCallback = onClickCallback; }
    inline void SetBackColor(const olc::Pixel& color) { m_BackColor = color; }
    inline void SetBorderColor(const olc::Pixel& color) { m_BorderColor = color; }
    inline void SetTextColor(const olc::Pixel& color) { m_TextColor = color; }
    inline void SetText(const std::string& text) { m_Text = text; }

private:
    std::string m_Text;
    olc::vi2d m_Position;
    olc::vi2d m_Size;
    olc::Pixel m_BackColor;
    olc::Pixel m_BorderColor;
    olc::Pixel m_TextColor;
    olc::Key m_Shortcut;
    std::function<void()> m_OnClickCallback;
};