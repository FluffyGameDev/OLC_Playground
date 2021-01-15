#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <ui/maze.h>
#include <ui/scene.h>

Scene* CreateMazeGenAppScene()
{
    Scene* newScene{ new Scene };

    newScene->AddUIElement(new Maze{ {0, 0} });

    return newScene;
}


class MazeGenApp : public olc::PixelGameEngine
{
public:
    MazeGenApp()
        : m_CurrentScene{ CreateMazeGenAppScene() }
    {
        sAppName = "MazeGen";
    }

    ~MazeGenApp()
    {
        delete m_CurrentScene;
    }

    bool OnUserCreate() override
    {
        return true;
    }

    bool OnUserUpdate(float deltaTime) override
    {
        m_CurrentScene->Draw(*this);
        m_CurrentScene->Update(*this, deltaTime);

        return true;
    }

private:
    Scene* m_CurrentScene;
};

int main()
{
    MazeGenApp mazeGenApp;
    if (mazeGenApp.Construct(264, 264, 2, 2))
        mazeGenApp.Start();

    return 0;
}