#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <numeric>
#include <random>

class SortingStrategy
{
public:
    virtual void PerformStep(std::vector<uint8_t>& numbers, std::pair<size_t, size_t>& comparedIndexes) = 0;
    
    void Reset()
    {
        m_IsSortingFinished = false;
        OnReset();
    }

    inline bool IsSortingFinished() const { return m_IsSortingFinished; }

protected:
    virtual void OnReset() = 0;

    void NotifyIsFinished()
    {
        m_IsSortingFinished = true;
    }

private:
    bool m_IsSortingFinished{};
};

class BubbleSortingStrategy : public SortingStrategy
{
public:
    void PerformStep(std::vector<uint8_t>& numbers, std::pair<size_t, size_t>& comparedIndexes) override
    {
        if (m_CurrentIndex < numbers.size() - 1)
        {
            comparedIndexes = { m_CurrentIndex, m_CurrentIndex + 1 };

            if (numbers[comparedIndexes.first] > numbers[comparedIndexes.second])
            {
                std::swap(numbers[comparedIndexes.first], numbers[comparedIndexes.second]);
                m_DidASwap = true;
            }
            ++m_CurrentIndex;
        }
        else
        {
            if (!m_DidASwap)
            {
                NotifyIsFinished();
            }
            m_CurrentIndex = 0;
            m_DidASwap = false;
        }
    }

    void OnReset() override
    {
        m_CurrentIndex = 0;
        m_DidASwap = false;
    }

private:
    size_t m_CurrentIndex{ 0 };
    bool m_DidASwap{ false };
};

class SortingVisualizer : public olc::PixelGameEngine
{
public:
    SortingVisualizer()
        : m_IsSorting{ false }
        , m_SortingStrategy{ std::make_unique<BubbleSortingStrategy>() }
    {
        sAppName = "Sorting Visualizer";
    }

    bool OnUserCreate() override
    {
        m_Numbers.resize(128);
        std::iota(m_Numbers.begin(), m_Numbers.end(), 1);
        return true;
    }

    bool OnUserUpdate(float deltaTime) override
    {
        if (GetKey(olc::Key::R).bPressed)
        {
            std::random_device randomDevice;
            std::mt19937 generator(randomDevice());
            std::shuffle(m_Numbers.begin(), m_Numbers.end(), generator);

            m_IsSorting = false;
        }

        if (GetKey(olc::Key::SPACE).bPressed)
        {
            m_IsSorting = !m_IsSorting;
        }

        std::pair<size_t, size_t> comparedIndexes{};
        if (m_IsSorting)
        {
            m_SortingStrategy->PerformStep(m_Numbers, comparedIndexes);

            if (m_SortingStrategy->IsSortingFinished())
            {
                m_IsSorting = false;
                m_SortingStrategy->Reset();
            }
        }

        Clear(olc::BLACK);
        int32_t size = m_Numbers.size();
        for (int32_t i = 0; i < size; ++i)
        {
            olc::Pixel color{ (m_IsSorting && (i == comparedIndexes.first || i == comparedIndexes.second)) ? olc::RED : olc::WHITE };
            FillRect({ 2 * i, 3 * ScreenHeight() / 4 - m_Numbers[i] }, { 2, m_Numbers[i] }, color);
        }

        return true;
    }

private:
    std::vector<uint8_t> m_Numbers;
    std::unique_ptr<SortingStrategy> m_SortingStrategy;
    bool m_IsSorting;
};

int main()
{
    SortingVisualizer sortingVisualizer;
    if (sortingVisualizer.Construct(256, 256, 2, 2))
        sortingVisualizer.Start();

    return 0;
}