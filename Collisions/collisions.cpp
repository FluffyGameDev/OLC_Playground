#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "glm.hpp"

struct Agent
{
    Agent(const glm::vec2& position, const glm::vec2& velocity, float radius)
        : Position{ position }
        , Velocity{ velocity }
        , Radius{ radius }
    {
    }

    glm::vec2 Position;
    glm::vec2 Velocity;
    float Radius;
};

bool IsCollisionDetected(const glm::vec2& position1, float radius1, const glm::vec2& position2, float radius2)
{
    return glm::length(position2 - position1) <= (radius1 + radius2);
}

class CollisionsApp : public olc::PixelGameEngine
{
public:
    CollisionsApp()
    {
        sAppName = "Collisions App";
    }

    bool OnUserCreate() override
    {
        m_Agents.reserve(K_AGENT_COUNT);
        for (uint32_t i = 0; i < K_AGENT_COUNT; ++i)
        {
            glm::vec2 position{};
            float radius{};
            bool hasSpawned{ false };
            for (uint32_t j = 0; j < K_MAX_SPAWN_ATTEMPTS; ++j)
            {
                radius = (float)((rand() % (K_MAX_RADIUS - K_MIN_RADIUS)) + K_MIN_RADIUS);
                position = glm::vec2
                {
                    (float)(rand() % (uint32_t)(ScreenWidth() - 2 * radius)) + radius,
                    (float)(rand() % (uint32_t)(ScreenHeight() - 2 * radius)) + radius
                };

                hasSpawned = std::all_of(m_Agents.begin(), m_Agents.end(), [&position, radius](const Agent& other)
                    {
                        return !IsCollisionDetected(position, radius, other.Position, other.Radius);
                    });
            }

            if (hasSpawned)
            {
                glm::vec2 velocity
                {
                    (float)((rand() % (K_MAX_VELOCITY - K_MIN_VELOCITY)) + K_MIN_VELOCITY) * ((rand() % 2) * 2 - 1),
                    (float)((rand() % (K_MAX_VELOCITY - K_MIN_VELOCITY)) + K_MIN_VELOCITY) * ((rand() % 2) * 2 - 1)
                };

                m_Agents.emplace_back(position, velocity, radius);
            }
        }
        return true;
    }

    bool OnUserUpdate(float deltaTime) override
    {
        UpdateAgents(deltaTime);
        HandleAgentCollisions();
        RenderAgents();

        return true;
    }

private:

    void UpdateAgents(float deltaTime)
    {
        for (Agent& agent : m_Agents)
        {
            agent.Position += agent.Velocity * deltaTime;
            if (agent.Position.x < agent.Radius && agent.Velocity.x < 0.0f ||
                agent.Position.x >(float)ScreenWidth() - agent.Radius && agent.Velocity.x > 0.0f)
            {
                agent.Velocity.x *= -1;
            }
            if (agent.Position.y < agent.Radius && agent.Velocity.y < 0.0f ||
                agent.Position.y >(float)ScreenHeight() - agent.Radius && agent.Velocity.y > 0.0f)
            {
                agent.Velocity.y *= -1;
            }
        }
    }

    void HandleAgentCollisions()
    {
        for (uint32_t i = 0; i < m_Agents.size() - 1; ++i)
        {
            for (uint32_t j = i + 1; j < m_Agents.size(); ++j)
            {
                Agent& agent1{ m_Agents[i] };
                Agent& agent2{ m_Agents[j] };

                if (IsCollisionDetected(agent1.Position, agent1.Radius, agent2.Position, agent2.Radius))
                {
                    glm::vec2 collisionNormal1{ glm::normalize(agent2.Position - agent1.Position) };
                    glm::vec2 collisionNormal2{ -collisionNormal1 };

                    //Probably not 100% correct. There are some cases where the velocity is not correct.
                    //BUG when a collision is detected several frames in a row between the same objects.
                    agent1.Velocity = (agent1.Velocity - 2 * glm::dot(agent1.Velocity, collisionNormal1) * collisionNormal1);
                    agent2.Velocity = (agent2.Velocity - 2 * glm::dot(agent2.Velocity, collisionNormal2) * collisionNormal2);

                    //TODO: energy transmission?
                }
            }
        }
    }

    void RenderAgents()
    {
        Clear(olc::BLACK);
        for (const Agent& agent : m_Agents)
        {
            FillCircle({ (int32_t)agent.Position.x, (int32_t)agent.Position.y }, agent.Radius, olc::WHITE);
        }
    }

    static const uint32_t K_AGENT_COUNT{ 16 };
    static const uint32_t K_MAX_SPAWN_ATTEMPTS{ 4 };
    static const uint32_t K_MIN_RADIUS{ 10 };
    static const uint32_t K_MAX_RADIUS{ 40 };
    static const int32_t K_MIN_VELOCITY{ 100 };
    static const int32_t K_MAX_VELOCITY{ 200 };

    std::vector<Agent> m_Agents;

};

int main()
{
    CollisionsApp collisionsApp;
    if (collisionsApp.Construct(1024, 512, 1, 1))
        collisionsApp.Start();

    return 0;
}