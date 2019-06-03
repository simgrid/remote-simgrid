#pragma once

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

#define ANSI_COLOR_RED     "\x0b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[94m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_HL_ON         "\E[7m"
#define ANSI_HL_OFF        "\E[27m"
#define DEBUG_PREFIX_STR   "DBG—"

#ifndef DEBUG_TRACE
    #define DEBUG_TRACE          0
#endif
#define DEBUG_SPAWN_CLIENT  (1 && DEBUG_TRACE)
#define DEBUG_SPAWN_SERVER  (1 && DEBUG_TRACE)
#define DEBUG_CLIENT        (1 && DEBUG_TRACE)
#define DEBUG_CLIENT_NET    (1 && DEBUG_CLIENT)
#define DEBUG_SERVER        (1 && DEBUG_TRACE)
#define DEBUG_SERVER_NET    (1 && DEBUG_SERVER)

#define rsg_die(fmt, ...) \
    do { \
        fflush(stdout); fflush(stderr); \
        fprintf(stderr, "Critical error: " fmt "\n", ##__VA_ARGS__); \
        fflush(stdout); fflush(stderr); \
        abort(); \
    } while(0)

/*!
 * \brief Print a debug message in blue. Useful for debugging!
 */
#define debug_client_print(fmt, ...) \
        do { if (DEBUG_CLIENT) { \
            fflush(stdout); fflush(stderr); \
            fprintf(stderr, DEBUG_PREFIX_STR ANSI_COLOR_BLUE "<:> %s:%d:%s(): " fmt ANSI_COLOR_RESET "\n", \
            __FILE__, __LINE__, __func__,  ##__VA_ARGS__); \
            fflush(stdout); fflush(stderr); }} while (0)

#define debug_process(fmt, ...) \
        do { if (DEBUG_CLIENT) { \
            fflush(stdout); fflush(stderr); \
            fprintf(stderr, DEBUG_PREFIX_STR ANSI_COLOR_MAGENTA "<:> %s:%d:%s(): " fmt ANSI_COLOR_RESET "\n", \
            __FILE__, __LINE__, __func__,  ##__VA_ARGS__); \
            fflush(stdout); fflush(stderr); }} while (0)

#define debug_spawn_client(fmt, ...) \
        do { if (DEBUG_SPAWN_CLIENT) { \
            fflush(stdout); fflush(stderr); \
            fprintf(stderr, DEBUG_PREFIX_STR ANSI_HL_ON "" ANSI_COLOR_MAGENTA "<:> %s:%d:%s(): " fmt  ANSI_HL_OFF "" ANSI_COLOR_RESET "\n", \
            __FILE__, __LINE__, __func__,  ##__VA_ARGS__); \
            fflush(stdout); fflush(stderr); }} while (0)

#define debug_spawn_server(fmt, ...) \
        do { if (DEBUG_SPAWN_SERVER) { \
            fflush(stdout); fflush(stderr); \
            fprintf(stderr, DEBUG_PREFIX_STR ANSI_HL_ON "" ANSI_COLOR_MAGENTA "<:> %s:%d:%s(): " fmt ANSI_HL_OFF "" ANSI_COLOR_RESET "\n", \
            __FILE__, __LINE__, __func__,  ##__VA_ARGS__); \
            fflush(stdout); fflush(stderr); }} while (0)



/*!
 * \brief Print a debug message in green. Useful for debugging!
 */
#include <mutex>
extern std::mutex print;
#define debug_server_print(fmt, ...) \
        do { if (DEBUG_SERVER) { print.lock(); \
            fflush(stdout); fflush(stderr); \
            fprintf(stderr, DEBUG_PREFIX_STR ANSI_COLOR_GREEN "<%f:%lu> %s:%d:%s(): " fmt ANSI_COLOR_RESET "\n", \
            SIMIX_get_clock(), simgrid::s4u::this_actor::get_pid(), \
            __FILE__, __LINE__, __func__,  ##__VA_ARGS__); \
            fflush(stdout); fflush(stderr); print.unlock(); }} while (0)
