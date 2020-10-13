#pragma once

#include <stdio.h>

#define RSG_INFO(fmt, ...) \
    do { \
        auto me = rsg::Actor::self(); \
        fflush(stdout); \
        fprintf(stdout, "[%s:%s:(%d) %.6lf] " fmt "\n", \
            me->get_host()->get_cname(), me->get_name().c_str(), me->get_pid(), rsg::Engine::get_clock(), ##__VA_ARGS__); \
        fflush(stdout); \
    } while (0)
