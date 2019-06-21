#pragma once

namespace rsg
{
class Host;

class Engine
{
private:
    Engine() = delete;
    Engine(const Engine &) = delete;

public:
    static double get_clock();
};

}
