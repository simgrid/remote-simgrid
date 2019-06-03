#pragma once

#include <stdio.h>

#define RSG_INFO(fmt, ...) \
    do { \
        fflush(stdout); \
        fprintf(stdout, "[%s] " fmt "\n", simgrid::rsg::Actor::self()->getName(),  ##__VA_ARGS__); \
        fflush(stdout); \
    } while (0)
