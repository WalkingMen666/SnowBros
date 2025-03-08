#include "Nick.hpp"

Nick::Nick() {
    LoadAnimations();
    m_Drawable = m_SpawnAnimation;
    m_Transform.translation = m_Position;
    m_SpawnAnimation->Play(); // 手動觸發播放
    // LOG_DEBUG("Nick initialized with Spawn animation, State: {}", m_SpawnAnimation->GetState());
}

void Nick::LoadAnimations() {
    const std::string basePath = RESOURCE_DIR "/Image/Character/Tom&Nick/";

    m_SpawnAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{
            basePath + "spawn1.png",
            basePath + "spawn2.png",
            basePath + "spawn3.png",
            basePath + "spawn4.png"
        }, false, 500, true, 0); // play = true

    m_IdleLeftAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{basePath + "nick_stand_left.png"}, true, 500, true, 0);
    m_IdleRightAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{basePath + "nick_stand_right.png"}, true, 500, true, 0);

    m_WalkLeftAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{
            basePath + "nick_walk_left_1.png",
            basePath + "nick_walk_left_2.png",
            basePath + "nick_walk_left_3.png"
        }, true, 200, true, 0);
    m_WalkRightAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{
            basePath + "nick_walk_right_1.png",
            basePath + "nick_walk_right_2.png",
            basePath + "nick_walk_right_3.png"
        }, true, 200, true, 0);

    m_AttackLeftAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{
            basePath + "nick_att_left_1.png",
            basePath + "nick_att_left_2.png"
        }, false, 300, true, 0);
    m_AttackRightAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{
            basePath + "nick_att_right_1.png",
            basePath + "nick_att_right_2.png"
        }, false, 300, true, 0);

    m_JumpLeftAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{
            basePath + "nick_jump_left_1.png",
            basePath + "nick_jump_left_2.png",
            basePath + "nick_jump_left_3.png",
            basePath + "nick_jump_left_4.png"
        }, false, 400, true, 0);
    m_JumpRightAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{
            basePath + "nick_jump_right_1.png",
            basePath + "nick_jump_right_2.png",
            basePath + "nick_jump_right_3.png",
            basePath + "nick_jump_right_4.png"
        }, false, 400, true, 0);

    m_DieAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{
            basePath + "nick_die_1.png",
            basePath + "nick_die_2.png",
            basePath + "nick_die_3.png"
        }, false, 500, true, 0);

    m_PushLeftAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{
            basePath + "nick_push_left_1.png",
            basePath + "nick_push_left_2.png",
            basePath + "nick_push_left_3.png"
        }, true, 300, true, 0);
    m_PushRightAnimation = std::make_shared<Util::Animation>(
        std::vector<std::string>{
            basePath + "nick_push_right_1.png",
            basePath + "nick_push_right_2.png",
            basePath + "nick_push_right_3.png"
        }, true, 300, true, 0);
}

void Nick::Update() {
    LOG_DEBUG("Nick::Update called, State: {}, Animation State: {}",
              static_cast<int>(m_State),
              static_cast<int>(std::dynamic_pointer_cast<Util::Animation>(m_Drawable)->GetState()));
    switch (m_State) {
        case State::SPAWN:
            if (IsAnimationFinished()) {
                LOG_DEBUG("Spawn animation finished, switching to INVINCIBLE");
                SetState(State::INVINCIBLE);
                m_InvincibleTimer = m_InvincibleDuration;
                m_BlinkTimer = 0.0f;
            }
            break;

        case State::INVINCIBLE:
            m_InvincibleTimer -= Util::Time::GetDeltaTime();
            m_BlinkTimer += Util::Time::GetDeltaTime();
            if (m_BlinkTimer >= m_BlinkInterval) {
                m_IsVisible = !m_IsVisible;
                m_BlinkTimer = 0.0f;
            }
            SetVisible(m_IsVisible);
            if (m_InvincibleTimer <= 0.0f) {
                LOG_DEBUG("Invincible state ended, switching to IDLE");
                SetState(State::IDLE);
                SetVisible(true);
            }
            break;

        case State::IDLE:
            if (Util::Input::IsKeyDown(Util::Keycode::A)) {
                SetDirection(false);
                SetState(State::WALK);
            } else if (Util::Input::IsKeyDown(Util::Keycode::D)) {
                SetDirection(true);
                SetState(State::WALK);
            } else if (Util::Input::IsKeyDown(Util::Keycode::SPACE)) {
                SetState(State::JUMP);
            } else if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
                SetState(State::ATTACK);
            }
            break;

        case State::WALK:
            if (Util::Input::IsKeyDown(Util::Keycode::A)) {
                m_Position.x -= m_Speed * Util::Time::GetDeltaTime();
                SetDirection(false);
            } else if (Util::Input::IsKeyDown(Util::Keycode::D)) {
                m_Position.x += m_Speed * Util::Time::GetDeltaTime();
                SetDirection(true);
            } else {
                SetState(State::IDLE);
            }
            break;

        case State::ATTACK:
            if (IsAnimationFinished()) {
                SetState(State::IDLE);
            }
            break;

        case State::JUMP:
            if (IsAnimationFinished()) {
                SetState(State::IDLE);
            }
            break;

        case State::DIE:
            if (IsAnimationFinished()) {
                SetState(State::SPAWN);
                m_Position = {0.0f, 0.0f};
            }
            break;

        case State::PUSH:
            if (!Util::Input::IsKeyDown(Util::Keycode::A) && !Util::Input::IsKeyDown(Util::Keycode::D)) {
                SetState(State::IDLE);
            }
            break;
    }

    m_Transform.translation = m_Position;
}

void Nick::SetState(State state) {
    if (m_State == state) return;
    // LOG_DEBUG("Switching state from {} to {}, Animation State: {}",
    //           static_cast<int>(m_State), static_cast<int>(state),
    //           std::dynamic_pointer_cast<Util::Animation>(m_Drawable)->GetState());
    m_State = state;
    SwitchAnimation(state, (state == State::IDLE || state == State::WALK || state == State::PUSH));
}

void Nick::SetDirection(bool facingRight) {
    if (m_FacingRight != facingRight) {
        m_FacingRight = facingRight;
        SwitchAnimation(m_State, (m_State == State::IDLE || m_State == State::WALK || m_State == State::PUSH));
    }
}

void Nick::SwitchAnimation(State state, bool looping) {
    std::shared_ptr<Util::Animation> animation;
    switch (state) {
        case State::SPAWN:
            animation = m_SpawnAnimation;
            break;
        case State::INVINCIBLE:
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
        case State::PUSH:
            animation = m_FacingRight ? m_PushRightAnimation : m_PushLeftAnimation;
            break;
    }
    m_Drawable = animation;
    animation->SetLooping(looping);
    animation->SetCurrentFrame(0);
    animation->Play(); // 確保動畫播放
    // LOG_DEBUG("Switched animation for state {}, looping: {}, State: {}",
    //           static_cast<int>(state), looping, animation->GetState());
}

bool Nick::IsAnimationFinished() const {
    auto animation = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
    bool finished = animation->GetState() == Util::Animation::State::ENDED;
    // LOG_DEBUG("IsAnimationFinished: State={}, CurrentFrame={}, FrameCount={}",
    //           static_cast<int>(animation->GetState()),
    //           animation->GetCurrentFrameIndex(),
    //           animation->GetFrameCount());
    return finished;
}