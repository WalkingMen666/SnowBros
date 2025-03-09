#include "Nick.hpp"

Nick::Nick()
    : Character(RESOURCE_DIR "/Image/Character/Tom&Nick/spawn1.png")
    , AnimatedCharacter(std::vector<std::string>{})
{
    LoadAnimations();
    m_Drawable = m_SpawnAnimation;
    SetPosition({0.0f, 0.0f});
    m_SpawnAnimation->Play();
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
        false, 200, false, 200); // cooldown 200ms
    m_AttackRightAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{basePath + "nick_att_right_1.png", basePath + "nick_att_right_2.png"},
        false, 200, false, 200); // cooldown 200ms

    m_JumpLeftAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{basePath + "nick_jump_left_1.png", basePath + "nick_jump_left_2.png", basePath + "nick_jump_left_3.png", basePath + "nick_jump_left_4.png"},
        false, 250, false, 0);
    m_JumpRightAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{basePath + "nick_jump_right_1.png", basePath + "nick_jump_right_2.png", basePath + "nick_jump_right_3.png", basePath + "nick_jump_right_4.png"},
        false, 250, false, 0);

    m_DieAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{basePath + "nick_die_1.png", basePath + "nick_die_2.png", basePath + "nick_die_3.png"},
        false, 300, false, 0);
}

void Nick::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    glm::vec2 position = GetPosition();
    bool isMoving = Util::Input::IsKeyPressed(Util::Keycode::A) || Util::Input::IsKeyPressed(Util::Keycode::D);

    // 處理無敵狀態的計時和閃爍
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

    switch (m_State) {
        case State::SPAWN:
            if (IsAnimationFinished()) {
                SetState(State::IDLE);
                SetInvincible(true);
            }
            break;

        case State::IDLE:
            HandleMovement(deltaTime, position);
            if (Util::Input::IsKeyPressed(Util::Keycode::MOUSE_LB)) {
                SetState(State::ATTACK);
            } else if (Util::Input::IsKeyPressed(Util::Keycode::SPACE)) {
                SetState(State::JUMP);
            } else if (isMoving) {
                SetState(State::WALK);
            }
            break;

        case State::WALK:
            HandleMovement(deltaTime, position);
            if (Util::Input::IsKeyPressed(Util::Keycode::MOUSE_LB)) {
                SetState(State::ATTACK);
            } else if (Util::Input::IsKeyPressed(Util::Keycode::SPACE)) {
                SetState(State::JUMP);
            } else if (!isMoving) {
                SetState(State::IDLE);
            }
            break;

        case State::ATTACK:
            HandleMovement(deltaTime, position); // 攻擊時保持移動
            if (Util::Input::IsKeyPressed(Util::Keycode::SPACE)) {
                SetState(State::JUMP);
            } else {
                auto animation = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
                if (animation->GetState() == Util::Animation::State::ENDED) {
                    SetState(isMoving ? State::WALK : State::IDLE); // 結束後根據移動狀態切換
                }
            }
            break;

        case State::JUMP:
            HandleMovement(deltaTime, position);
            m_JumpVelocity += m_Gravity * deltaTime;
            position.y += m_JumpVelocity * deltaTime;
            if (position.y <= m_GroundLevel) {
                position.y = m_GroundLevel;
                m_JumpVelocity = 0.0f;
                SetState(isMoving ? State::WALK : State::IDLE); // 落地後根據移動狀態切換
            }
            break;

        case State::DIE:
            if (IsAnimationFinished()) {
                SetState(State::SPAWN);
                SetPosition({0.0f, 0.0f});
            }
            break;
    }

    SetPosition(position);
}

void Nick::SetState(State state) {
    if (m_State == state) return;
    m_State = state;

    if (state == State::JUMP) {
        m_JumpVelocity = m_JumpInitialVelocity;
    }

    SwitchAnimation(state, (state == State::IDLE || state == State::WALK));
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

    if (m_State == State::JUMP) {
        auto currentAnimation = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
        int currentFrame = currentAnimation->GetCurrentFrameIndex();
        SwitchAnimation(m_State, false);
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
    }
    if (animation) {
        m_Drawable = animation;
        SetLooping(looping);
        animation->SetCurrentFrame(0);
        animation->Play();
    }
}

void Nick::HandleMovement(float deltaTime, glm::vec2& position) {
    if (Util::Input::IsKeyPressed(Util::Keycode::A)) {
        position.x -= m_Speed * deltaTime;
        SetDirection(false);
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::D)) {
        position.x += m_Speed * deltaTime;
        SetDirection(true);
    }
}

void Nick::Die() {
    SetState(State::DIE);
    SetInvincible(false);
}