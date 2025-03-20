// main.cpp
#include "App.hpp"
#include "Core/Context.hpp"

int main(int, char**) {
    auto context = Core::Context::GetInstance();
    App& app = App::GetInstance();

    while (!context->GetExit()) {
        switch (app.GetCurrentState()) {
            case App::State::START:
                app.Start();
            break;
            case App::State::UPDATE:
            case App::State::GAMEOVER:
                app.Update();
            break;
            case App::State::END:
                app.End();
            context->SetExit(true);
            break;
        }
        context->Update();
    }
    return 0;
}