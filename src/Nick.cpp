#include "Nick.hpp"
#include "App.hpp"
#include "Bullet.hpp"
#include "Snowball.hpp"
// #include "GameWorld.hpp"

Nick::Nick()
    : Character(RESOURCE_DIR "/Image/Character/Tom&Nick/spawn1.png")
    , AnimatedCharacter(std::vector<std::string>{})
{
    LoadAnimations();
    m_Drawable = m_SpawnAnimation;
    SetPosition({0.0f, -285.0f}); // 出生在第 29 行地面
    m_SpawnAnimation->Play();
    // Update(); // 初始校正
}

void Nick::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    glm::vec2 position = GetPosition();
    float moveSpeed = m_Speed * deltaTime; // m_Speed = 150.0f
    float moveDistance = 0.0f;
    bool isOnPlatform = false;
    bool isMoving = Util::Input::IsKeyPressed(Util::Keycode::A) || Util::Input::IsKeyPressed(Util::Keycode::D);

    if (m_IsInvincible) {
        m_InvincibleTimer -= deltaTime;
        m_BlinkTimer += deltaTime;
        if (m_BlinkTimer >= m_BlinkInterval) {
            SetVisible(!GetVisibility());
            m_BlinkTimer = 0.0f;
        }
        if (m_InvincibleTimer <= 0.0f) {
            SetInvincible(false);
        }
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::A)) {
        moveDistance = -moveSpeed;
    } else if (Util::Input::IsKeyPressed(Util::Keycode::D)) {
        moveDistance = moveSpeed;
    }

    glm::vec2 newPosition = GameWorld::map_collision_judgement(characterWidth, characterWidth, position, m_JumpVelocity, m_Gravity, moveDistance, isOnPlatform);

    // 設置方向（保持動畫）
    if(moveDistance != 0.0f) SetDirection(moveDistance > 0);

    // 使用 GetPosition() 而非 GetTransform()
    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) { // 改為 IsKeyDown
        bool nearSnowball = false;
        for (auto& obj : GameWorld::GetObjects()) {
            if (auto snowball = std::dynamic_pointer_cast<Snowball>(obj)) {
                if (glm::distance(position, snowball->GetPosition()) < 50.0f) {
                    SetState(State::KICK);
                    snowball->Push(m_FacingRight ? Direction::Right : Direction::Left);
                    nearSnowball = true;
                    break;
                }
            }
        }
        if (!nearSnowball) {
            SetState(State::ATTACK);
            GameWorld::AddObject(std::make_shared<Bullet>(position, m_FacingRight ? Direction::Right : Direction::Left));
            App::GetRoot().AddChild(GameWorld::GetObjects().back());
        }
    }

    if (isMoving) {
        for (auto& obj : GameWorld::GetObjects()) {
            if (auto snowball = std::dynamic_pointer_cast<Snowball>(obj)) {
                if (glm::distance(position, snowball->GetPosition()) < 50.0f) {
                    SetState(State::PUSH);
                    snowball->Roll(m_FacingRight ? Direction::Right : Direction::Left);
                    break;
                }
            }
        }
    }

    // 狀態機
    switch (m_State) {
        case State::SPAWN:
            if (IsAnimationFinished()) {
                SetState(State::IDLE);
                SetInvincible(true);
            }
            break;
        case State::IDLE:
            if (Util::Input::IsKeyPressed(Util::Keycode::MOUSE_LB)) {
                SetState(State::ATTACK);
            } else if (Util::Input::IsKeyPressed(Util::Keycode::SPACE) && isOnPlatform) {
                SetState(State::JUMP);
            } else if (isMoving) {
                SetState(State::WALK);
            }
            break;
        case State::WALK:
            if (Util::Input::IsKeyPressed(Util::Keycode::MOUSE_LB)) {
                SetState(State::ATTACK);
            } else if (Util::Input::IsKeyPressed(Util::Keycode::SPACE) && isOnPlatform) {
                SetState(State::JUMP);
            } else if (!isMoving) {
                SetState(State::IDLE);
            }
            break;
        case State::ATTACK:
            if (Util::Input::IsKeyPressed(Util::Keycode::SPACE) && isOnPlatform) {
                SetState(State::JUMP);
            } else if (std::dynamic_pointer_cast<Util::Animation>(m_Drawable)->GetState() == Util::Animation::State::ENDED) {
                SetState(isMoving ? State::WALK : State::IDLE);
            }
            break;
        case State::JUMP:
            if (isOnPlatform && m_JumpVelocity == 0.0f) {
                SetState(isMoving ? State::WALK : State::IDLE);
            }
            break;
        case State::DIE:
            if (IsAnimationFinished()) {
                SetState(State::SPAWN);
                SetPosition({0.0f, -285.0f});
            }
            break;
        case State::PUSH:
            if (!isMoving) SetState(State::IDLE);
            break;
        case State::KICK:
            if (IsAnimationFinished()) SetState(isMoving ? State::WALK : State::IDLE);
            break;
    }
    SetPosition(newPosition);
}

void Nick::SetState(State state) {
    if (m_State == state) return;
    m_State = state;

    if (state == State::JUMP) {
        m_JumpVelocity = m_JumpInitialVelocity;
    }

    SwitchAnimation(state, (state == State::IDLE || state == State::WALK || state == State::PUSH));
}

void Nick::SetInvincible(bool invincible) {
    if (m_IsInvincible == invincible) return;
    m_IsInvincible = invincible;
    if (invincible) {
        m_InvincibleTimer = m_InvincibleDuration;
        m_BlinkTimer = 0.0f;
    } else {
        SetVisible(true);
    }
}

void Nick::SetDirection(bool facingRight) {
    if (m_FacingRight == facingRight) return;
    m_FacingRight = facingRight;

    if (m_State == State::JUMP || m_State == State::PUSH || m_State == State::KICK) {
        auto currentAnimation = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
        int currentFrame = currentAnimation->GetCurrentFrameIndex();
        SwitchAnimation(m_State, m_State == State::PUSH);
        std::dynamic_pointer_cast<Util::Animation>(m_Drawable)->SetCurrentFrame(currentFrame);
    } else if (m_State == State::IDLE || m_State == State::WALK) {
        SwitchAnimation(m_State, true);
    }
}

void Nick::SwitchAnimation(State state, bool looping) {
    std::shared_ptr<Util::Animation> animation;
    switch (state) {
        case State::SPAWN:
            animation = m_SpawnAnimation;
            break;
        case State::IDLE:
            animation = m_FacingRight ? m_IdleRightAnimation : m_IdleLeftAnimation;
            break;
        case State::WALK:
            animation = m_FacingRight ? m_WalkRightAnimation : m_WalkLeftAnimation;
            break;
        case State::ATTACK:
            animation = m_FacingRight ? m_AttackRightAnimation : m_AttackLeftAnimation;
            break;
        case State::JUMP:
            animation = m_FacingRight ? m_JumpRightAnimation : m_JumpLeftAnimation;
            break;
        case State::DIE:
            animation = m_DieAnimation;
            break;
        case State::PUSH: animation = m_FacingRight ? m_PushRightAnimation : m_PushLeftAnimation;
            break;
        case State::KICK: animation = m_FacingRight ? m_KickRightAnimation : m_KickLeftAnimation;
            break;
    }
    if (animation) {
        m_Drawable = animation;
        SetLooping(looping);
        animation->SetCurrentFrame(0);
        animation->Play();
    }
}

void Nick::Die() {
    if (m_Lives > 0) {
        m_Lives--;
        SetState(State::DIE);
        SetInvincible(false);
    }
}

void Nick::LoadAnimations() {
    const std::string basePath = RESOURCE_DIR "/Image/Character/Tom&Nick/";

    m_SpawnAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{basePath + "spawn1.png", basePath + "spawn2.png", basePath + "spawn3.png", basePath + "spawn4.png"},
        false, 200, false, 0);

    m_IdleLeftAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{basePath + "nick_stand_left.png"}, false, 500, true, 0);
    m_IdleRightAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{basePath + "nick_stand_right.png"}, false, 500, true, 0);

    m_WalkLeftAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{basePath + "nick_walk_left_1.png", basePath + "nick_walk_left_2.png", basePath + "nick_walk_left_3.png"},
        false, 200, true, 0);
    m_WalkRightAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{basePath + "nick_walk_right_1.png", basePath + "nick_walk_right_2.png", basePath + "nick_walk_right_3.png"},
        false, 200, true, 0);

    m_AttackLeftAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{basePath + "nick_att_left_1.png", basePath + "nick_att_left_2.png"},
        false, 200, false, 200);
    m_AttackRightAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{basePath + "nick_att_right_1.png", basePath + "nick_att_right_2.png"},
        false, 200, false, 200);

    m_JumpLeftAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{basePath + "nick_jump_left_1.png", basePath + "nick_jump_left_2.png", basePath + "nick_jump_left_3.png", basePath + "nick_jump_left_4.png"},
        false, 250, false, 0);
    m_JumpRightAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{basePath + "nick_jump_right_1.png", basePath + "nick_jump_right_2.png", basePath + "nick_jump_right_3.png", basePath + "nick_jump_right_4.png"},
        false, 250, false, 0);

    m_DieAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{basePath + "nick_die_1.png", basePath + "nick_die_2.png", basePath + "nick_die_3.png"},
        false, 300, false, 0);
    m_PushLeftAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{basePath + "nick_push_left_1.png", basePath + "nick_push_left_2.png", basePath + "nick_push_left_3.png"},
        false, 200, true, 0);
    m_PushRightAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{basePath + "nick_push_right_1.png", basePath + "nick_push_right_2.png", basePath + "nick_push_right_3.png"},
        false, 200, true, 0);
    m_KickLeftAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{basePath + "nick_kick_left.png"}, false, 200, false, 0);
    m_KickRightAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{basePath + "nick_kick_right.png"}, false, 200, false, 0);
}