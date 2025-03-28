#include "Snowball.hpp"
#include "Enemy.hpp"
#include "GameWorld.hpp"
#include "Util/Logger.hpp"

Snowball::Snowball(const glm::vec2& pos) : m_State(SnowballState::Static), m_MeltTimer(0.0f) {
    m_Transform.translation = pos;
    m_RollAnim = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "snowball_roll_1.png", BASE_PATH + "snowball_roll_2.png",
                                 BASE_PATH + "snowball_roll_3.png", BASE_PATH + "snowball_roll_4.png"}, false, 200, true, 0);
    m_MeltAnim = std::make_shared<Util::Animation>(
        std::vector<std::string>{BASE_PATH + "snowball_melt_1.png", BASE_PATH + "snowball_melt_2.png",
                                 BASE_PATH + "snowball_melt_3.png"}, false, 500, false, 0);
    m_Drawable = m_RollAnim;
    m_RollAnim->Play();
}

void Snowball::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    if (m_State == SnowballState::Static) {
        m_MeltTimer += deltaTime;
        if (m_MeltTimer >= MELT_DURATION) {
            GameWorld::RemoveObject(std::shared_ptr<Snowball>(this));
        }
    } else if (m_State == SnowballState::Rolling || m_State == SnowballState::Pushed) {
        m_Transform.translation += m_Velocity * deltaTime;
    }
}

void Snowball::Roll(Direction dir) {
    m_State = SnowballState::Rolling;
    m_Velocity.x = (dir == Direction::Right) ? 100.0f : -100.0f;
}

void Snowball::Push(Direction dir) {
    m_State = SnowballState::Pushed;
    m_Velocity.x = (dir == Direction::Right) ? 300.0f : -300.0f;
}

bool Snowball::IsMoving() const { return m_State == SnowballState::Pushed; }

void Snowball::OnCollision(std::shared_ptr<Util::GameObject> other) {
    if (m_State == SnowballState::Pushed) {
        if (auto enemy = std::dynamic_pointer_cast<Enemy>(other)) {
            enemy->OnCollision(std::shared_ptr<Snowball>(this));
        }
    }
}