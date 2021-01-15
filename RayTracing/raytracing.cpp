#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "glm.hpp"
#include "gtc/constants.hpp"

class Sphere
{
public:
    Sphere(const glm::vec3& color, const glm::vec3& position, float radius, bool isReflective)
        : m_Color{ color }
        , m_Position{ position }
        , m_Radius{ radius }
        , m_IsReflective{ isReflective }
    {
    }

    const glm::vec3& GetColor() const { return m_Color; }
    const glm::vec3& GetPosition() const { return m_Position; }
    bool IsReflective() const { return m_IsReflective; }

    bool IsIntersectedByRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& intersectionDistance) const
    {
        glm::vec3 oc = rayOrigin - m_Position;
        float a = glm::dot(rayDirection, rayDirection);
        float b = 2.0 * glm::dot(oc, rayDirection);
        float c = dot(oc, oc) - m_Radius * m_Radius;
        float discriminant = b * b - 4 * a * c;
        if (discriminant < 0) {
            intersectionDistance = -1.0f;
            return false;
        }
        else {
            intersectionDistance = (-b - sqrt(discriminant)) / (2.0 * a);
            return intersectionDistance > 0.0f;
        }
    }

private:
    glm::vec3 m_Color;
    glm::vec3 m_Position;
    float m_Radius;
    bool m_IsReflective;
};

class RayTracingApp : public olc::PixelGameEngine
{
public:
    RayTracingApp()
        : m_EyePosition{ 5.0f, -1.0f, 0.0f }
        , m_LookAtPosition{ 0.0f, 0.0f, 0.0f }
        , m_UpDirection{ 0.0f, 1.0f, 0.0f }
        , m_LightPosition{ 10.0f, -10.0f, 0.5f }
        , m_DistanceToViewport{ 1.0f }
        , m_FOV{ glm::half_pi<float>() }
        , m_LookFromAngle{ 0.0f }
        , m_LookFromDistance{ 5.0f }
        , m_MaxBoundCount{ 0 }
    {
        sAppName = "Ray Tracing";
    }

    ~RayTracingApp()
    {
    }

    bool OnUserCreate() override
    {
        m_Shapes.emplace_back(glm::vec3{ 0.0f, 1.0f, 0.0f }, glm::vec3{ -1, 1, 0 }, 1.0f, false);
        m_Shapes.emplace_back(glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec3{ 2, 2, 0 }, 1.0f, false);
        m_Shapes.emplace_back(glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0, 1, 2 }, 1.0f, true);
        m_Shapes.emplace_back(glm::vec3{ 0.1f, 0.1f, 0.1f }, glm::vec3{ 1, 1, -2 }, 1.0f, true);
        m_Shapes.emplace_back(glm::vec3{ 0.05f, 0.05f, 0.05f }, glm::vec3{ 0, -1, 0 }, 1.0f, true);

        return true;
    }

    bool OnUserUpdate(float deltaTime) override
    {
        if (m_MaxBoundCount < 10 && GetKey(olc::Key::NP_ADD).bPressed)
        {
            ++m_MaxBoundCount;
        }
        if (m_MaxBoundCount > 0 && GetKey(olc::Key::NP_SUB).bPressed)
        {
            --m_MaxBoundCount;
        }

        m_LookFromAngle += deltaTime;
        m_EyePosition.x = std::cos(m_LookFromAngle) * m_LookFromDistance;
        m_EyePosition.z = std::sin(m_LookFromAngle) * m_LookFromDistance;


        int32_t viewportWidth{ ScreenWidth() };
        int32_t viewportHeight{ ScreenHeight() };

        glm::vec3 lookDirection{ glm::normalize(m_LookAtPosition - m_EyePosition) };
        glm::vec3 lookDirectionOrtho{ glm::normalize(glm::cross(glm::vec3{ lookDirection }, glm::vec3{ m_UpDirection })) };
        glm::vec3 viewportCenter{ m_EyePosition + lookDirection * m_DistanceToViewport };

        float aspectRatio{ (static_cast<float>(viewportWidth - 1)) / static_cast<float>(viewportHeight - 1) };
        float gx{ m_DistanceToViewport * glm::tan(m_FOV * 0.5f) };
        float gy{ gx * aspectRatio };

        glm::vec3 qxv{ ((2 * gx) / (viewportHeight - 1)) * lookDirectionOrtho };
        glm::vec3 qyv{ ((2 * gy) / (viewportWidth - 1)) * m_UpDirection };
        glm::vec3 p1mv{ lookDirection * m_DistanceToViewport - gx * lookDirectionOrtho - gy * m_UpDirection };

        for (int32_t i = 0; i < viewportWidth; ++i)
        {
            for (int32_t j = 0; j < viewportHeight; ++j)
            {
                glm::vec3 pij{ p1mv + qxv * static_cast<float>(i) + qyv * static_cast<float>(j) };
                glm::vec3 ray{ glm::normalize(pij) };

                
                glm::vec3 color{ TraceRay(m_EyePosition, ray, m_MaxBoundCount) };
                Draw({ i, j }, { (uint8_t)(color.r * 255), (uint8_t)(color.g * 255), (uint8_t)(color.b * 255) });
            }
        }


        std::stringstream ss1;
        ss1 << "FPS: " << GetFPS() << ", Frame Length: " << 1000.0f / GetFPS() << "ms";
        DrawString({ 0, 0 }, ss1.str(), olc::BLACK);

        std::stringstream ss2;
        ss2 << "Max Bounce: " << m_MaxBoundCount;
        DrawString({ 0, 8 }, ss2.str(), olc::BLACK);

        return true;
    }

    glm::vec3 TraceRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, uint32_t maxRayBounce) const
    {
        glm::vec3 color{ 1.0f, 1.0f, 1.0f };
        float hitDistance{ -1.0f };
        const Sphere* hitShape{ nullptr };
        for (const Sphere& shape : m_Shapes)
        {
            float t{};
            if (shape.IsIntersectedByRay(rayOrigin, rayDirection, t))
            {
                if (t < hitDistance || hitDistance < 0.0f)
                {
                    hitDistance = t;
                    hitShape = &shape;
                }
            }
        }

        if (hitShape != nullptr)
        {
            glm::vec3 intersectionPoint{ rayOrigin + rayDirection * hitDistance };
            glm::vec3 intersectionNormal{ glm::normalize(intersectionPoint - hitShape->GetPosition()) };

            glm::vec3 surfaceColor{};


            float bias = 1e-4;
            bool inside = false;
            if (hitShape->IsReflective() && maxRayBounce > 0) {
                float facingRatio = -glm::dot(rayDirection, intersectionNormal);
                float fresneleffect = mix(powf(1.0f - facingRatio, 3.0f), 1.0f, 0.5f);
                glm::vec3 refldir = glm::normalize(rayDirection - intersectionNormal * 2.0f * glm::dot(rayDirection, intersectionNormal));
                glm::vec3 reflection = TraceRay(intersectionPoint + intersectionNormal * bias, refldir, maxRayBounce - 1);
                surfaceColor = (reflection * fresneleffect);
            }

            glm::vec3 lightDirection{ glm::normalize(m_LightPosition - intersectionPoint) };
            float lightRatio{ std::max(glm::dot(lightDirection, intersectionNormal), 0.0f) };

            color = surfaceColor + hitShape->GetColor() * lightRatio;
        }

        return color;
    }

    float mix(float a, float b, float mix) const
    {
        return b * mix + a * (1 - mix);
    }

private:
    std::vector<Sphere> m_Shapes;
    glm::vec3 m_EyePosition;
    glm::vec3 m_LookAtPosition;
    glm::vec3 m_UpDirection;
    glm::vec3 m_LightPosition;
    float m_DistanceToViewport;
    float m_FOV;

    float m_LookFromAngle;
    float m_LookFromDistance;

    uint32_t m_MaxBoundCount;
};

int main()
{
    RayTracingApp rayTracingApp;
    if (rayTracingApp.Construct(264, 264, 2, 2))
        rayTracingApp.Start();

    return 0;
}