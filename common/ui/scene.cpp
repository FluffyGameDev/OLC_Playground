#include <ui/scene.h>

Scene::~Scene()
{
    for (UIElement* element : m_UIElements)
    {
        delete element;
    }
    m_UIElements.clear();
}

void Scene::AddUIElement(UIElement* element)
{
    m_UIElements.push_back(element);
}

void Scene::Draw(olc::PixelGameEngine& engine)
{
    for (UIElement* element : m_UIElements)
    {
        element->Draw(engine);
    }
}

void Scene::Update(olc::PixelGameEngine& engine, float deltaTime)
{
    for (UIElement* element : m_UIElements)
    {
        element->Update(engine, deltaTime);
    }
}