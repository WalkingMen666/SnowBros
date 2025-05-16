#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Character.hpp"
#include "AnimatedCharacter.hpp"
#include "UpdatableDrawable.hpp"
#include "Util/Animation.hpp"
#include "Snowball.hpp"
#include "App.hpp"
#include <map>
#include <string>

// enum class Direction { Left, Right };
enum class EnemyState { Normal, Snowball, Dead };

class Snowball;

class Enemy : public UpdatableDrawable, public Character, public AnimatedCharacter {
public:

    Enemy(const std::string& imagePath, const glm::vec2& pos);
    virtual ~Enemy() = default;

    virtual void Update() = 0;
    virtual void OnHit();
    virtual void OnCollision(std::shared_ptr<Util::GameObject> other) = 0;
    virtual bool IsBoss() const { return false; }
    virtual void Die() { m_State = EnemyState::Dead; }
    void SetAnimation(const std::string& key);
    void SetMeltStage(int stage);
    void GetAttacked(){m_MaxHealth--;}
    void GetSnowballCollision(){m_MaxHealth-=15;}

    [[nodiscard]] EnemyState GetState() const { return m_State; }
    [[nodiscard]] std::shared_ptr<Core::Drawable> GetDrawable() const override { return m_Drawable; }
    [[nodiscard]] glm::vec2 GetPosition() const { return m_Transform.translation; }
    [[nodiscard]] int GetMeltStage() const { return m_MeltStage; }
    [[nodiscard]] virtual std::shared_ptr<Snowball> GetSnowball() const{return  nullptr;}
    [[nodiscard]] virtual float GetCharacterWidth() const = 0;
    [[nodiscard]] virtual float GetCharacterHeight() const = 0;
    [[nodiscard]] virtual float GetJumpVelocity() const = 0;
    [[nodiscard]] float GetWidth() const override { return GetCharacterWidth(); }
    [[nodiscard]] float GetHeight() const override { return GetCharacterHeight(); }

protected:
    // Movement and State
    glm::vec2 m_Velocity;
    Direction m_Direction;
    int       m_HitCount;
    int       m_MaxHits;
    EnemyState m_State;
    int       m_MeltStage = 0;
    float     m_MeltTimer = 0.0f;
    int m_MaxHealth = 1;

    // Constants
    static constexpr float m_GroundLevel = -325.0f;
    static constexpr float m_MeltDuration = 4.0f;

    // Animations
    std::map<std::string, std::shared_ptr<Util::Animation>> m_Animations;
};

#endif // ENEMY_HPP