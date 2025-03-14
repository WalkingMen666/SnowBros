// Nick.cpp
#include "Nick.hpp"
#include "App.hpp"
#include "PhaseResourceManger.hpp"

Nick::Nick()
    : Character(RESOURCE_DIR "/Image/Character/Tom&Nick/spawn1.png")
    , AnimatedCharacter(std::vector<std::string>{})
{
    LoadAnimations();
    m_Drawable = m_SpawnAnimation;
    SetPosition({0.0f, -285.0f});
    m_SpawnAnimation->Play();
    Update(); // 初始校正
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
}

void Nick::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    glm::vec2 position = GetPosition();
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

    auto prm = App::GetPRM();
    if (!prm) {
        LOG_ERROR("PRM is null!");
        return;
    }
    const Map& map = prm->GetMap();

    float characterWidth = 40.0f; // 匹配新瓦片大小
    float characterHeight = 40.0f;
    float characterBottom = position.y - characterHeight / 2;
    float characterTop = position.y + characterHeight / 2;
    float characterLeft = position.x - characterWidth / 2;
    float characterRight = position.x + characterWidth / 2;

    // 計算移動
    glm::vec2 newPosition = position;
    float moveSpeed = m_Speed * deltaTime;

    // 水平移動與牆壁碰撞（瓦片值 2）及邊界限制
    if (Util::Input::IsKeyPressed(Util::Keycode::A)) {
        newPosition.x -= moveSpeed;
        int nextLeftTileX = std::max(0, std::min(static_cast<int>((newPosition.x - characterWidth / 2 + 410.0f) / TILE_SIZE), 164 - 1));
        int tileYBottom = std::max(0, std::min(static_cast<int>((360.0f - characterBottom) / TILE_SIZE), 144 - 1));
        int tileYTop = std::max(0, std::min(static_cast<int>((360.0f - characterTop) / TILE_SIZE), 144 - 1));
        bool blocked = false;
        for (int y = tileYBottom; y <= tileYTop; ++y) {
            if (map.GetTile(nextLeftTileX, y) == 2) {
                blocked = true;
                newPosition.x = (nextLeftTileX + 1) * TILE_SIZE - 410.0f + characterWidth / 2;
                break;
            }
        }
        if (!blocked && newPosition.x < -410.0f) {
            newPosition.x = -410.0f; // 限制左邊界
        } else if (!blocked) {
            SetDirection(false);
        }
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::D)) {
        newPosition.x += moveSpeed;
        int nextRightTileX = std::max(0, std::min(static_cast<int>((newPosition.x + characterWidth / 2 + 410.0f) / TILE_SIZE), 164 - 1));
        int tileYBottom = std::max(0, std::min(static_cast<int>((360.0f - characterBottom) / TILE_SIZE), 144 - 1));
        int tileYTop = std::max(0, std::min(static_cast<int>((360.0f - characterTop) / TILE_SIZE), 144 - 1));
        bool blocked = false;
        for (int y = tileYBottom; y <= tileYTop; ++y) {
            if (map.GetTile(nextRightTileX, y) == 2) {
                blocked = true;
                newPosition.x = nextRightTileX * TILE_SIZE - 410.0f - characterWidth / 2;
                break;
            }
        }
        if (!blocked && newPosition.x > 410.0f) {
            newPosition.x = 410.0f; // 限制右邊界
        } else if (!blocked) {
            SetDirection(true);
        }
    }

    // 垂直移動
    m_JumpVelocity += m_Gravity * deltaTime;
    float nextY = newPosition.y + m_JumpVelocity * deltaTime;

    // 平台碰撞檢測（僅在下落時）
    bool isOnPlatform = false;
    float platformY = m_GroundLevel;

    if (m_JumpVelocity <= 0) { // 下落或靜止時檢查
        int leftTileX = std::max(0, std::min(static_cast<int>((newPosition.x - characterWidth / 2 + 410.0f) / TILE_SIZE), 164 - 1));
        int rightTileX = std::max(0, std::min(static_cast<int>((newPosition.x + characterWidth / 2 + 410.0f) / TILE_SIZE), 164 - 1));
        float nextBottom = nextY - characterHeight / 2;
        int tileYStart = std::max(0, std::min(static_cast<int>((360.0f - characterBottom) / TILE_SIZE), 144 - 1));
        int tileYEnd = std::max(0, std::min(static_cast<int>((360.0f - nextBottom) / TILE_SIZE), 144 - 1));

        for (int tileX = leftTileX; tileX <= rightTileX; ++tileX) {
            for (int tileY = std::min(tileYStart, tileYEnd); tileY <= std::max(tileYStart, tileYEnd); ++tileY) {
                if (tileY >= 0 && tileY < 144 && map.GetTile(tileX, tileY) == 1) {
                    float platformTop = 360.0f - tileY * TILE_SIZE;
                    // 檢查是否從上方穿過平台
                    if (characterBottom >= platformTop && nextBottom <= platformTop) {
                        isOnPlatform = true;
                        platformY = platformTop + characterHeight / 2;
                        m_JumpVelocity = 0.0f;
                        break;
                    }
                }
            }
            if (isOnPlatform) break;
        }
    }

    // 更新位置
    if (isOnPlatform) {
        newPosition.y = platformY;
    } else {
        newPosition.y = nextY;
        if (newPosition.y <= m_GroundLevel) {
            newPosition.y = m_GroundLevel;
            m_JumpVelocity = 0.0f;
            isOnPlatform = true;
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
                SetPosition({0.0f, -280.0f});
            }
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
    auto prm = App::GetPRM();
    if (!prm) {
        LOG_ERROR("PRM is null!");
        return;
    }
    const Map& map = prm->GetMap();

    float characterWidth = 20.0f;
    float characterHeight = 20.0f;
    float moveSpeed = m_Speed * deltaTime;

    if (Util::Input::IsKeyPressed(Util::Keycode::A)) {
        float nextX = position.x - moveSpeed;
        int tileX = std::max(0, std::min(static_cast<int>((nextX - characterWidth / 2) / Map::TILE_SIZE), Map::MAP_WIDTH - 1));
        int tileYBottom = std::max(0, std::min(static_cast<int>((position.y - characterHeight / 2) / Map::TILE_SIZE), Map::MAP_HEIGHT - 1));
        int tileYTop = std::max(0, std::min(static_cast<int>((position.y + characterHeight / 2) / Map::TILE_SIZE), Map::MAP_HEIGHT - 1));

        bool canMove = true;
        for (int y = tileYBottom; y <= tileYTop; ++y) {
            if (map.GetTile(tileX, y) == 2) {
                canMove = false;
                break;
            }
        }
        if (canMove) {
            position.x = nextX;
            SetDirection(false);
        }
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::D)) {
        float nextX = position.x + moveSpeed;
        int tileX = std::max(0, std::min(static_cast<int>((nextX + characterWidth / 2) / Map::TILE_SIZE), Map::MAP_WIDTH - 1));
        int tileYBottom = std::max(0, std::min(static_cast<int>((position.y - characterHeight / 2) / Map::TILE_SIZE), Map::MAP_HEIGHT - 1));
        int tileYTop = std::max(0, std::min(static_cast<int>((position.y + characterHeight / 2) / Map::TILE_SIZE), Map::MAP_HEIGHT - 1));

        bool canMove = true;
        for (int y = tileYBottom; y <= tileYTop; ++y) {
            if (map.GetTile(tileX, y) == 2) {
                canMove = false;
                break;
            }
        }
        if (canMove) {
            position.x = nextX;
            SetDirection(true);
        }
    }
}

void Nick::Die() {
    SetState(State::DIE);
    SetInvincible(false);
}