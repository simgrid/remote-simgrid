#pragma once

namespace rsg
{
class Host;

class Engine
{
private:
    Engine() = delete;
    Engine(const Engine &) = delete;
    Engine & operator=(const Engine &) = delete;

public:
    static double get_clock();
};

}
