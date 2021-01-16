#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <numeric>
#include <queue>
#include <random>

class SortingStrategy
{
public:
    virtual void PerformStep(std::vector<uint8_t>& numbers, std::pair<size_t, size_t>& comparedIndexes) = 0;
    virtual void Begin(std::vector<uint8_t>& numbers) {};

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

class QuickSortingStrategy : public SortingStrategy
{
public:
    void PerformStep(std::vector<uint8_t>& numbers, std::pair<size_t, size_t>& comparedIndexes) override
    {
        if (m_CurrentIndexes.first < m_CurrentIndexes.second)
        {
            comparedIndexes = m_CurrentIndexes;
            if (numbers[m_CurrentIndexes.first] > numbers[m_CurrentIndexes.second])
            {
                std::swap(numbers[m_CurrentIndexes.first], numbers[m_CurrentIndexes.second]);

                m_IsMovingLeftIndex = !m_IsMovingLeftIndex;
            }

            if (m_IsMovingLeftIndex)
            {
                ++m_CurrentIndexes.first;
            }
            else
            {
                --m_CurrentIndexes.second;
            }
        }
        else
        {
            std::pair<uint32_t, uint32_t> subSectionLeft{ m_SubSections.front().first, m_CurrentIndexes.first > 0 ? m_CurrentIndexes.first - 1 : m_CurrentIndexes.first };
            std::pair<uint32_t, uint32_t> subSectionRight{ m_CurrentIndexes.first < numbers.size() - 1 ? m_CurrentIndexes.first + 1 : m_CurrentIndexes.first, m_SubSections.front().second };

            m_SubSections.pop();
            if (subSectionLeft.first < subSectionLeft.second)
            {
                m_SubSections.push(subSectionLeft);
            }
            if (subSectionRight.first < subSectionRight.second)
            {
                m_SubSections.push(subSectionRight);
            }

            if (m_SubSections.empty())
            {
                NotifyIsFinished();
            }
            else
            {
                m_CurrentIndexes = m_SubSections.front();
                m_IsMovingLeftIndex = true;
            }
        }
    }

    void Begin(std::vector<uint8_t>& numbers) override
    {
        m_IsMovingLeftIndex = true;
        m_CurrentIndexes = { 0, (uint8_t)numbers.size() - 1 };
        m_SubSections.emplace(m_CurrentIndexes);
    }

    void OnReset() override
    {
        while (!m_SubSections.empty())
        {
            m_SubSections.pop();
        }
    }

private:
    std::queue<std::pair<uint32_t, uint32_t>> m_SubSections;
    std::pair<uint32_t, uint32_t> m_CurrentIndexes;
    bool m_IsMovingLeftIndex{ true };
};

class SortingVisualizer : public olc::PixelGameEngine
{
public:
    SortingVisualizer()
        : m_IsSorting{ false }
        , m_SortingStrategy{ std::make_unique<QuickSortingStrategy>() }
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
            if (m_IsSorting)
            {
                m_SortingStrategy->Begin(m_Numbers);
            }
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
        size_t size{ m_Numbers.size() };
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