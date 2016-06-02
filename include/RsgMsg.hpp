#pragma once

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// #define DEBUG_MUTEX
// #define DEBUG 1
#define debug_print(COLOR, fmt, ...) \
        do { if (DEBUG) fprintf(stderr, #COLOR "<DEBUG> %s:%d:%s():" fmt ANSI_COLOR_RESET "\n", __FILE__, \
                                __LINE__, __func__,  ##__VA_ARGS__); } while (0)

#define debug_process(fmt, ...) \
        do { if (DEBUG) fprintf(stderr, ANSI_COLOR_MAGENTA "<PROCESS> %s:%d:%s():\t" fmt ANSI_COLOR_RESET "\n", __FILE__, \
                                __LINE__, __func__,  ##__VA_ARGS__); } while (0)
            
#define debug_server(fmt, ...) \
        do { if (DEBUG) fprintf(stderr, ANSI_COLOR_CYAN "<SERVER> %s:%d:%s():\t" fmt ANSI_COLOR_RESET "\n", __FILE__, \
                                __LINE__, __func__,  ##__VA_ARGS__); } while (0)

#define debug_actor(fmt, ...) \
        do { if (DEBUG) fprintf(stderr, ANSI_COLOR_BLUE "<ACTOR> %s:%d:%s():\t" fmt ANSI_COLOR_RESET "\n", __FILE__, \
                                __LINE__, __func__,  ##__VA_ARGS__); } while (0)


#define DEBUG2 1
#define debug2_print(fmt, ...) \
        do { if (DEBUG2) fprintf(stderr, ANSI_COLOR_CYAN "<DEBUG> %s:%d:%s():\t" fmt ANSI_COLOR_RESET, __FILE__, \
                                __LINE__, __func__,  ##__VA_ARGS__); } while (0)
        