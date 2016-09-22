
#ifndef __COMMON_HPP__
#define __COMMON_HPP__


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


#define DEBUG_TRACE         0
#define DEBUG_SPAWN_CLIENT  (1 && DEBUG_TRACE)
#define DEBUG_SPAWN_SERVER  (1 && DEBUG_TRACE)
#define DEBUG_CLIENT        (0 && DEBUG_TRACE)
#define DEBUG_CLIENT_NET    (0 && DEBUG_CLIENT)
#define DEBUG_SERVER        (0 && DEBUG_TRACE)
#define DEBUG_SERVER_NET    (0 && DEBUG_SERVER)


/*!
 * \brief Print a debug message in blue. Useful for debugging!
 */
#define debug_client_print(fmt, ...) \
        do { if (DEBUG_CLIENT) { \
            fflush(stdout); fflush(stderr); \
            fprintf(stderr, ANSI_COLOR_BLUE "<:> %s:%d:%s(): " fmt ANSI_COLOR_RESET "\n", \
            __FILE__, __LINE__, __func__,  ##__VA_ARGS__); \
            fflush(stdout); fflush(stderr); }} while (0)

#define debug_process(fmt, ...) \
        do { if (DEBUG_CLIENT) { \
            fflush(stdout); fflush(stderr); \
            fprintf(stderr, ANSI_COLOR_MAGENTA "<:> %s:%d:%s(): " fmt ANSI_COLOR_RESET "\n", \
            __FILE__, __LINE__, __func__,  ##__VA_ARGS__); \
            fflush(stdout); fflush(stderr); }} while (0)

#define debug_spawn_client(fmt, ...) \
        do { if (DEBUG_SPAWN_CLIENT) { \
            fflush(stdout); fflush(stderr); \
            fprintf(stderr, ANSI_HL_ON "" ANSI_COLOR_MAGENTA "<:> %s:%d:%s(): " fmt  ANSI_HL_OFF "" ANSI_COLOR_RESET "\n", \
            __FILE__, __LINE__, __func__,  ##__VA_ARGS__); \
            fflush(stdout); fflush(stderr); }} while (0)

#define debug_spawn_server(fmt, ...) \
        do { if (DEBUG_SPAWN_SERVER) { \
            fflush(stdout); fflush(stderr); \
            fprintf(stderr, ANSI_HL_ON "" ANSI_COLOR_MAGENTA "<:> %s:%d:%s(): " fmt ANSI_HL_OFF "" ANSI_COLOR_RESET "\n", \
            __FILE__, __LINE__, __func__,  ##__VA_ARGS__); \
            fflush(stdout); fflush(stderr); }} while (0)



/*!
 * \brief Print a debug message in green. Useful for debugging!
 */
#define debug_server_print(fmt, ...) \
        do { if (DEBUG_SERVER) { \
            fflush(stdout); fflush(stderr); \
            fprintf(stderr, ANSI_COLOR_GREEN "<%f:%i> %s:%d:%s(): " fmt ANSI_COLOR_RESET "\n", \
            SIMIX_get_clock(), s4u::this_actor::getPid(), \
            __FILE__, __LINE__, __func__,  ##__VA_ARGS__); \
            fflush(stdout); fflush(stderr); }} while (0)


#endif // __COMMON_HPP__
