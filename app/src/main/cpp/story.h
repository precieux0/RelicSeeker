#pragma once
#include <string>

class Story {
public:
    static Story& get();
    void trigger(const std::string& event);
    void update(float dt);
    void render(class Renderer& r);
    bool isActive() const;
private:
    std::string mCurrentText;
    float mTimer;
};
