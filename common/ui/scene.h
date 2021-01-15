#include <ui/uielement.h>

class Scene
{
public:
    ~Scene();

    void AddUIElement(UIElement* element);

    void Draw(olc::PixelGameEngine& engine);
    void Update(olc::PixelGameEngine& engine, float deltaTime);

private:
    std::vector<UIElement*> m_UIElements;
};