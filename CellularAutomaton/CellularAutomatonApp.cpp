#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <ui/uielement.h>
#include <ui/button.h>
#include <ui/cellularautomaton.h>
#include <ui/scene.h>

//TODO: YUCK! Clean that mess!
std::vector<CellType> conwayCells;
std::vector<CellType> wireWorldCells;
bool isUsingConway{ true };

//TODO: Make this data driven
void CreateCellConfigs()
{
    {
        CellType deadCell{};
        deadCell.CellID = 0;
        deadCell.Color = olc::DARK_BLUE;
        deadCell.DefaultTransitionCellID = 0;
        deadCell.Transitions.emplace_back(1, 1, 3);
        conwayCells.push_back(deadCell);

        CellType aliveCell{};
        aliveCell.CellID = 1;
        aliveCell.Color = olc::WHITE;
        aliveCell.DefaultTransitionCellID = 0;
        aliveCell.Transitions.emplace_back(1, 1, 2);
        aliveCell.Transitions.emplace_back(1, 1, 3);
        conwayCells.push_back(aliveCell);
    }

    {
        CellType emptyCell{};
        emptyCell.CellID = 0;
        emptyCell.Color = olc::BLACK;
        emptyCell.DefaultTransitionCellID = 0;
        wireWorldCells.push_back(emptyCell);

        CellType electronHeadCell{};
        electronHeadCell.CellID = 1;
        electronHeadCell.Color = olc::BLUE;
        electronHeadCell.DefaultTransitionCellID = 2;
        wireWorldCells.push_back(electronHeadCell);

        CellType electronTailCell{};
        electronTailCell.CellID = 2;
        electronTailCell.Color = olc::RED;
        electronTailCell.DefaultTransitionCellID = 3;
        wireWorldCells.push_back(electronTailCell);

        CellType conductorCell{};
        conductorCell.CellID = 3;
        conductorCell.Color = olc::YELLOW;
        conductorCell.DefaultTransitionCellID = 3;
        conductorCell.Transitions.emplace_back(1, 1, 1);
        conductorCell.Transitions.emplace_back(1, 1, 2);
        wireWorldCells.push_back(conductorCell);
    }
}

Scene* CreateAutomatonScene()
{
    CreateCellConfigs();

    Scene* newScene{ new Scene };

    CellularAutomaton* cellularAutomaton{ new CellularAutomaton{ { 2, 32 } } };
    cellularAutomaton->SetupCellConfigs(conwayCells);

    Button* playButton{ new Button{ "Start", { 1, 1 }, { 40, 10 }, olc::Key::SPACE} };
    playButton->RegisterOnClick([playButton, cellularAutomaton]()
    {
        cellularAutomaton->ToggleSimulation();
        playButton->SetText(cellularAutomaton->IsSimulationPlaying() ? "Stop" : "Start");
    });

    Button* randomizeButton{ new Button{ "Randomize", { 44, 1 }, { 75, 10 }, olc::Key::R } };
    randomizeButton->RegisterOnClick([cellularAutomaton]()
    {
        cellularAutomaton->Randomize();
    });

    Button* clearButton{ new Button{ "Clear", { 121, 1 }, { 41, 10 }, olc::Key::C } };
    clearButton->RegisterOnClick([cellularAutomaton]()
    {
        cellularAutomaton->Clear();
    });

    Button* brushButton{ new Button{ "", { 1, 12 }, { 10, 10 }, olc::Key::B, } };
    brushButton->SetBorderColor(olc::WHITE);
    brushButton->SetBackColor(cellularAutomaton->GetCurrentBrushColor());
    brushButton->RegisterOnClick([brushButton, cellularAutomaton]()
    {
        cellularAutomaton->CycleToNextBrush();
        brushButton->SetBackColor(cellularAutomaton->GetCurrentBrushColor());
    });

    Button* switchCellsButton{ new Button{ "Conway", { 165, 1 }, { 81, 10 }, olc::Key::S } };
    switchCellsButton->RegisterOnClick([switchCellsButton, brushButton, cellularAutomaton]()
    {
        isUsingConway = !isUsingConway;
        cellularAutomaton->SetupCellConfigs(isUsingConway ? conwayCells : wireWorldCells);
        switchCellsButton->SetText(isUsingConway ? "Conway" : "Wire World");
        brushButton->SetBackColor(cellularAutomaton->GetCurrentBrushColor());
    });

    newScene->AddUIElement(cellularAutomaton);
    newScene->AddUIElement(playButton);
    newScene->AddUIElement(randomizeButton);
    newScene->AddUIElement(clearButton);
    newScene->AddUIElement(switchCellsButton);
    newScene->AddUIElement(brushButton);

    return newScene;
}


class CellularAutomatonApp : public olc::PixelGameEngine
{
public:
    CellularAutomatonApp()
        : m_CurrentScene{ CreateAutomatonScene() }
    {
        sAppName = "Cellular Automaton";
    }

    ~CellularAutomatonApp()
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
    CellularAutomatonApp cellularAutomatonApp;
    if (cellularAutomatonApp.Construct(262, 292, 2, 2))
        cellularAutomatonApp.Start();

    return 0;
}