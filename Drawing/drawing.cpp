#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "glm.hpp"

class DrawingApp : public olc::PixelGameEngine
{
public:
    DrawingApp()
        : m_TrianglePoint1{ 216, 100 }
        , m_TrianglePoint2{ 109, 246 }
        , m_TrianglePoint3{ 409, 456 }
        , m_CurvePoint1{ 10, 10 }
        , m_CurvePoint2{ 100, 10 }
        , m_CurvePoint3{ 100, 100 }
        , m_HeldPoint{ nullptr }
        , m_HoveredPoint{ nullptr }
    {
        sAppName = "Rasterization";
    }

    bool OnUserCreate() override
    {
        m_AllPoints.push_back(&m_TrianglePoint1);
        m_AllPoints.push_back(&m_TrianglePoint2);
        m_AllPoints.push_back(&m_TrianglePoint3);
        m_AllPoints.push_back(&m_CurvePoint1);
        m_AllPoints.push_back(&m_CurvePoint2);
        m_AllPoints.push_back(&m_CurvePoint3);

        return true;
    }

    bool OnUserUpdate(float deltaTime) override
    {
        Clear(olc::BLACK);

        m_HoveredPoint = nullptr;
        for (olc::vi2d* point : m_AllPoints)
        {
            olc::vi2d diff{ GetMousePos() - *point };
            if (diff.mag() < K_SELECTION_RADIUS)
            {
                m_HoveredPoint = point;
                break;
            }
        }

        if (GetMouse(0).bPressed)
        {
            m_HeldPoint = m_HoveredPoint;
        }
        if (GetMouse(0).bReleased)
        {
            m_HeldPoint = nullptr;
        }

        if (m_HeldPoint != nullptr)
        {
            *m_HeldPoint = GetMousePos();
        }

        MyDrawTriangle(m_TrianglePoint1, m_TrianglePoint2, m_TrianglePoint3);
        DrawTriangle(m_TrianglePoint1, m_TrianglePoint2, m_TrianglePoint3, olc::RED);

        MyDrawBezierCurveQuadratic(m_CurvePoint1, m_CurvePoint2, m_CurvePoint3);

        for (const olc::vi2d* point : m_AllPoints)
        {
            if (point != nullptr)
            {
                FillCircle(*point, K_SELECTION_RADIUS, m_HoveredPoint == point ? olc::YELLOW : olc::DARK_YELLOW);
            }
        }

        return true;
    }

    void MyDrawTriangle(const olc::vi2d& point1, const olc::vi2d& point2, const olc::vi2d& point3)
    {
        olc::vi2d topPoint{ point1 };
        olc::vi2d middlePoint{ point2 };
        olc::vi2d bottomPoint{ point3 };
        if (middlePoint.y < topPoint.y) { std::swap(topPoint, middlePoint); }
        if (bottomPoint.y < topPoint.y) { std::swap(topPoint, bottomPoint); }
        if (bottomPoint.y < middlePoint.y) { std::swap(bottomPoint, middlePoint); }

        float leftPosition{ (float)topPoint.x };
        float rightPosition{ leftPosition };

        //TODO: handles cases where 2 or more points have the same Y
        //TODO: fix precision issues. Lerp may help?

        {
            float leftDiff{ (float)(middlePoint.x - topPoint.x) / (float)(middlePoint.y - topPoint.y) };
            float rightDiff{ (float)(bottomPoint.x - topPoint.x) / (float)(bottomPoint.y - topPoint.y) };
            if (bottomPoint.x < middlePoint.x) { std::swap(rightDiff, leftDiff); }

            for (int32_t j = topPoint.y; j <= middlePoint.y; ++j)
            {
                DrawLine({ (int32_t)std::roundf(leftPosition), j }, { (int32_t)std::roundf(rightPosition), j });
                leftPosition += leftDiff;
                rightPosition += rightDiff;
            }
        }

        {
            float leftDiff{ (float)(bottomPoint.x - middlePoint.x) / (float)(bottomPoint.y - middlePoint.y) };
            float rightDiff{ (float)(bottomPoint.x - topPoint.x) / (float)(bottomPoint.y - topPoint.y) };
            if (bottomPoint.x < middlePoint.x) { std::swap(rightDiff, leftDiff); }

            for (int32_t j = middlePoint.y; j <= bottomPoint.y; ++j)
            {
                DrawLine({ (int32_t)std::roundf(leftPosition), j }, { (int32_t)std::roundf(rightPosition), j });
                leftPosition += leftDiff;
                rightPosition += rightDiff;
            }
        }
    }

    void MyDrawBezierCurveQuadratic(const olc::vi2d& point1, const olc::vi2d& point2, const olc::vi2d& point3)
    {
        olc::vi2d prevPoint{ point1 };

        for (int32_t i = 1; i <= K_CURVE_ITERATIONS; ++i)
        {
            float t{ (float)i / (float)K_CURVE_ITERATIONS };
            olc::vi2d curPoint
            {
                (int32_t)((1 - t) * (1 - t) * point1.x + 2 * (1 - t) * t * point2.x + t * t * point3.x),
                (int32_t)((1 - t) * (1 - t) * point1.y + 2 * (1 - t) * t * point2.y + t * t * point3.y)
            };

            DrawLine(prevPoint, curPoint);


            prevPoint = curPoint;
        }
    }

private:
    static constexpr int32_t K_SELECTION_RADIUS{ 10 };
    static constexpr int32_t K_CURVE_ITERATIONS{ 10 };

    olc::vi2d m_TrianglePoint1;
    olc::vi2d m_TrianglePoint2;
    olc::vi2d m_TrianglePoint3;
    olc::vi2d m_CurvePoint1;
    olc::vi2d m_CurvePoint2;
    olc::vi2d m_CurvePoint3;
    std::vector<olc::vi2d*> m_AllPoints;
    olc::vi2d* m_HeldPoint;
    olc::vi2d* m_HoveredPoint;
};

int main()
{
    DrawingApp drawingApp;
    if (drawingApp.Construct(512, 512, 1, 1))
        drawingApp.Start();

    return 0;
}