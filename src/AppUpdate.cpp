#include "App.hpp"
#include "Nick.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "GameWorld.hpp"
#include "RedDemon.hpp"
#include "Enemy.hpp"
#include "Bullet.hpp"

void App::Update() {
    float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
    if (m_Phase == Phase::Phase_1) {
        m_FadeTimer += deltaTime;
        float alpha = 0.0f;
        if (m_FadingIn) {
            alpha = std::max(0.0f, 180.0f - (255.0f * (m_FadeTimer / (m_FadeDuration / 2))));
            if (m_FadeTimer >= m_FadeDuration / 2) {
                m_FadingIn = false;
                m_FadeTimer = 0.0f;
            }
        } else {
            alpha = std::min(180.0f, 200.0f * (m_FadeTimer / (m_FadeDuration / 2)));
            if (m_FadeTimer >= m_FadeDuration / 2) {
                m_FadingIn = true;
                m_FadeTimer = 0.0f;
            }
        }
        int alphaInt = static_cast<int>(alpha);
        std::string overlayPath = RESOURCE_DIR "/Image/Background/black/black" + std::to_string(alphaInt) + ".png";
        m_Overlay->SetImage(overlayPath);
        m_Overlay->SetVisible(true);

        if (Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
            m_Phase = Phase::Phase0;
            m_PRM->NextPhase();
            m_FadeTimer = 0.0f;
            m_FadingIn = true;
            m_Overlay->SetVisible(false);
            LOG_DEBUG("Entered Phase 0");
        }
    }
    else if (m_Phase == Phase::Phase0) {
        if (Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
            m_Phase = Phase::Phase1;
            m_PRM->NextPhase();
            if (!m_Nick) { // 僅在 m_Nick 未創建時分配
                m_Nick = std::make_shared<Nick>();
                m_Root.AddChild(std::static_pointer_cast<Util::GameObject>(m_Nick));
                m_Nick->SetState(Nick::State::SPAWN);
            }
            GameWorld::AddObject(std::make_shared<RedDemon>(glm::vec2(100.0f, -285.0f)));
            m_Root.AddChild(std::static_pointer_cast<Util::GameObject>(GameWorld::GetObjects().back()));
            LOG_DEBUG("Entered Phase 1, Nick and RedDemon initialized");
        }
    }
    else if (m_Phase == Phase::Phase1) {
        auto& objects = GameWorld::GetObjects();
        std::vector<std::shared_ptr<UpdatableDrawable>> toRemove;
        for (auto& obj : objects) {
            obj->Update();
            if (auto bullet = std::dynamic_pointer_cast<Bullet>(obj)) {
                if (bullet->IsMarkedForRemoval()) toRemove.push_back(obj);
            }
            if (auto enemy = std::dynamic_pointer_cast<Enemy>(obj)) {
                if (enemy->GetState() == EnemyState::Dead &&
                    std::dynamic_pointer_cast<Util::Animation>(enemy->GetDrawable())->GetState() == Util::Animation::State::ENDED) {
                    toRemove.push_back(obj);
                }
            }
        }
        for (const auto& obj : toRemove) {
            GameWorld::RemoveObject(obj);
            m_Root.RemoveChild(obj);
        }
    }

    if (m_Nick) m_Nick->Update();
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
    m_Root.Update();
}