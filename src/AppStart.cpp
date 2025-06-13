#include "App.hpp"
#include "Util/Logger.hpp"

App::App() {
    // 初始化關卡配置（確保正確）
    m_LevelConfigs = {
        { -1, false, {} }, // 初始畫面
        { 0, false, {} },  // 開始畫面
        { 1, false, { {"RedDemon", {-260.0f, 160.0f}}, {"RedDemon", {260.0f, 160.0f}}, {"RedDemon", {0.0f, -40.0f}}, {"Fat", {0.0f, 250.0f}} } },
        { 2, false, { {"Frog", {180.0f, 150.0f}}, {"Monkey", {-150.0f, 100.0f}}, {"Frog", {-110.0f, -185.0f}}, {"Monkey", {65.0f, -80.0f}} }},
        { 3, false, {{"Frog", {-370.0f, 140.0f}}, {"Frog", {370.0f, 140.0f}}, {"RedDemon", {-200.0f, 20.0f}}, {"RedDemon", {200.0f, 20.0f}}, {"RedDemon", {-350.0f, -200.0f}}, {"RedDemon", {350.0f, -200.0f}} }},
        { 4, false, {{"Fat", {0.0f, 240.0f}},{"Frog", {-270.0f, -200.0f}},{"Frog", {270.0f, -200.0f}},{"Monkey", {210.0f, 30.0f}},{"Monkey", {-225.0f, 100.0f}},{"Monkey", {100.0f, 250.0f}} } },
        { 5, false, {{"Fat", {0.0f, 240.0f}},{"Frog", {-130.0f, 30.0f}},{"Frog", {120.0f, 30.0f}},{"RedDemon", {-370.0f, -90.0f}},{"RedDemon", {370.0f, -90.0f}},{"RedDemon", {-180.0f, -200.0f}}} },
        { 6, false, {{"Fat", {0.0f, 240.0f}},{"Frog", {350.0f, 80.0f}},{"Frog", {130.0f, 40.0f}},{"Frog", {-130.0f, 40.0f}},{"RedDemon", {-380.0f, 30.0f}}} },
        { 7, false, {{"RedDemon", {-230.0f, 230.0f}},{"RedDemon", {150.0f, 230.0f}},{"RedDemon", {-260.0f, 15.0f}},{"RedDemon", {-140.0f, -200.0f}},{"RedDemon", {160.0f, -200.0f}},{"RedDemon", {330.0f, -140.0f}}} },
        { 8, false, {{"Monkey", {0.0f, 230.0f}},{"Monkey", {-200.0f, 100.0f}},{"Monkey", {200.0f, 100.0f}},{"Frog", {-290.0f, 10.0f}},{"Frog", {290.0f, 10.0f}}} },
        { 9, false, {{"Monkey", {0.0f, 200.0f}},{"Monkey", {-300.0f, 0.0f}},{"Monkey", {300.0f, 0.0f}},{"Frog", {-260.0f, -200.0f}},{"Frog", {260.0f, -200.0f}}} },
        { 10, true, { {"Boss", {300.0f, 80.0f}}, } }, // 第10關: 1隻 Boss
        { 11, false, { {"RedDemon", {-360.0f, 240.0f}},{"RedDemon", {360.0f, 240.0f}},{"Frog", {-200.0f, 140.0f}},{"Monkey", {300.0f, 140.0f}}, } },
        { 12, false, { {"Monkey", {-260.0f, -200.0f}},{"Monkey", {260.0f, -200.0f}},{"RedDemon", {-180.0f, 190.0f}},{"RedDemon", {180.0f, 190.0f}} } },
        { 13, false, { {"Monkey", {-370.0f, 240.0f}}, {"Monkey", {350.0f, -320.0f}},{"RedDemon", {370.0f, 240.0f}},{"RedDemon", {-320.0f, -110.0f}} } },
        { 14, false, { {"Monkey", {0.0f, 140.0f}},{"Monkey", {-370.0f, -10.0f}},{"Monkey", {370.0f, 10.0f}},{"Frog", {0.0f, -80.0f}}} },
        { 15, false, { {"Monkey", {-240.0f, 100.0f}},{"Monkey", {240.0f, 100.0f}},{"Frog", {-250.0f, -65.0f}}, {"Frog", {250.0f, -65.0f}}} },
        { 16, false, { {"Fat", {0.0f, 220.0f}},{"Monkey", {-200.0f, 70.0f}},{"Monkey", {200.0f, 130.0f}},{"Monkey", {0.0f, -20.0f}}, } },
        { 17, false, { {"Monkey", {0.0f, 20.0f}},{"Monkey", {-370.0f, 230.0f}},{"RedDemon", {0.0f, -140.0f}},{"RedDemon", {0.0f, 170.0f}},{"RedDemon", {370.0f, 230.0f}}, } },
        { 18, false, { {"Frog", {-150.0f, -200.0f}},{"Frog", {120.0f, 220.0f}},{"Monkey", {-250.0f, 60.0f}},{"Monkey", {300.0f, 60.0f}}, } },
        { 19, false, { {"Monkey", {-180.0f, -80.0f}},{"Monkey", {0.0f, -80.0f}},{"Monkey", {180.0f, -80.0f}},{"RedDemon", {-350.0f, 230.0f}},{"RedDemon", {350.0f, 230.0f}},{"RedDemon", {-310.0f, -290.0f}},{"RedDemon", {310.0f, -290.0f}}, } },
        { 20, true, { {"Boss2", {0.0f, 95.0f}}, } }, // 第20關: 1隻 Boss2
    };
    m_IntroBGM = std::make_shared<Util::BGM>(RESOURCE_DIR "/Audio/intro.mp3");
    m_Stage1BGM = std::make_shared<Util::BGM>(RESOURCE_DIR "/Audio/stage1_bgm.mp3");
    m_Stage2BGM = std::make_shared<Util::BGM>(RESOURCE_DIR "/Audio/stage2_bgm.mp3");
    m_BossBGM = std::make_shared<Util::BGM>(RESOURCE_DIR "/Audio/boss_bgm.mp3");
    m_GameOverBGM = std::make_shared<Util::SFX>(RESOURCE_DIR "/Audio/game_over.mp3");

    // Set BGM volumes
    m_IntroBGM->SetVolume(10);
    m_Stage1BGM->SetVolume(10);
    m_Stage2BGM->SetVolume(10);
    m_BossBGM->SetVolume(10);
    m_GameOverBGM->SetVolume(10);

    // 初始化數字圖片快取
    m_DigitImages.resize(10);
    for (int i = 0; i <= 9; ++i) {
        std::string imagePath = RESOURCE_DIR "/Image/Text/red_" + std::to_string(i) + ".png";
        m_DigitImages[i] = std::make_shared<Util::Image>(imagePath);
        if (!m_DigitImages[i]) {
            LOG_INFO("Failed to load digit image: {}", imagePath);
        }
    }
}

void App::Start() {
    LOG_TRACE("Start");
    m_PRM = std::make_shared<PhaseResourceManger>();
    m_Root.AddChildren(m_PRM->GetChildren());

    // 初始化覆蓋層
    m_Overlay = std::make_shared<BackgroundImage>();
    m_Overlay->SetImage(RESOURCE_DIR "/Image/Background/black/black255.png");
    m_Overlay->m_Transform.scale = {1.0f, 1.0f}; // 設置縮放
    m_Overlay->SetZIndex(5); // 設置 Z 軸順序
    m_Root.AddChild(m_Overlay);

    // 初始化生命值 UI
    m_nickLives = std::make_shared<BackgroundImage>();
    m_nickLives->SetImage(RESOURCE_DIR "/Image/Text/red_3.png");
    m_nickLives->m_Transform.translation = {-320.0f, 360.0f};
    m_nickLives->SetZIndex(5);
    m_nickLives->SetVisible(false);
    m_Root.AddChild(m_nickLives);

    // 初始化分數 UI
    m_ScoreDigits.resize(MAX_SCORE_DIGITS);
    for (int i = 0; i < MAX_SCORE_DIGITS; ++i) {
        m_ScoreDigits[i] = std::make_shared<BackgroundImage>();
        m_ScoreDigits[i]->SetImage(RESOURCE_DIR "/Image/Text/red_0.png");
        m_ScoreDigits[i]->m_Transform.translation = {m_ScorePosition.x - i * m_ScoreDigitSpacing, m_ScorePosition.y};
        m_ScoreDigits[i]->m_Transform.scale = {1.0f, 1.0f};
        m_ScoreDigits[i]->SetZIndex(5);
        m_ScoreDigits[i]->SetVisible(false);
        m_Root.AddChild(m_ScoreDigits[i]);
    }

    // 初始化為初始畫面
    InitializeLevel(-1);
    m_CurrentState = State::UPDATE;
    m_IntroBGM->Play(-1);  // Play intro music and loop indefinitely
    LOG_INFO("App started with initial level -1");
}