#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Character.hpp"
#include "AnimatedCharacter.hpp"
#include "UpdatableDrawable.hpp"
#include "Util/Animation.hpp"
#include "App.hpp"
#include <map>
#include <string>

enum class Direction { Left, Right };
enum class EnemyState { Normal, Snowball, Dead };

class Enemy : public UpdatableDrawable, public Character, public AnimatedCharacter {
protected:
    glm::vec2 m_Velocity;
    Direction m_Direction;
    int m_HitCount;
    int m_MaxHits;
    static constexpr float m_GroundLevel = -285.0f;
    EnemyState m_State;
    std::map<std::string, std::shared_ptr<Util::Animation>> m_Animations;
    int m_MeltStage = 0;
    float m_MeltTimer = 0.0f;
    static constexpr float m_MeltDuration = 4.0f;

public:
    Enemy(const std::string& imagePath, const glm::vec2& pos);
    virtual ~Enemy() = default;
    virtual void Update() = 0;
    virtual void OnHit();
    virtual void OnCollision(std::shared_ptr<Util::GameObject> other) = 0;
    void SetAnimation(const std::string& key);
    EnemyState GetState() const { return m_State; }
    std::shared_ptr<Core::Drawable> GetDrawable() const override { return m_Drawable; }
    glm::vec2 GetPosition() const { return m_Transform.translation; }
    int GetMeltStage() const { return m_MeltStage; }
    void SetMeltStage(int stage);
    virtual float GetCharacterWidth() const = 0;
    virtual float GetCharacterHeight() const = 0;
    virtual float GetJumpVelocity() const = 0;
    float GetWidth() const override { return GetCharacterWidth(); }
    float GetHeight() const override { return GetCharacterHeight(); }
};

#endif // ENEMY_HPP