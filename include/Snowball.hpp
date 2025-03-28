#ifndef SNOWBALL_HPP
#define SNOWBALL_HPP

#include "UpdatableDrawable.hpp"
#include "Util/Animation.hpp"
#include "App.hpp"
#include "Util/Time.hpp"
#include "Enemy.hpp"

enum class SnowballState { Static, Rolling, Pushed };

class Snowball : public UpdatableDrawable {
private:
    glm::vec2 m_Velocity;
    SnowballState m_State;
    std::shared_ptr<Util::Animation> m_RollAnim;
    std::shared_ptr<Util::Animation> m_MeltAnim;
    float m_MeltTimer;
    const float MELT_DURATION = 5.0f;
    const std::string BASE_PATH = RESOURCE_DIR "/Image/Character/Enemies/";

public:
    Snowball(const glm::vec2& pos);
    void Update() override;
    void Roll(Direction dir);
    void Push(Direction dir);
    bool IsMoving() const;
    void OnCollision(std::shared_ptr<Util::GameObject> other);
    std::shared_ptr<Core::Drawable> GetDrawable() const override { return m_Drawable; }
    glm::vec2 GetPosition() const { return m_Transform.translation; }
};

#endif // SNOWBALL_HPP