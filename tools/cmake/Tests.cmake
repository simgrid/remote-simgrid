
find_program(TESH_COMMAND tesh PATHS /opt/simgrid/bin ENV PATH)

if(NOT TESH_COMMAND)
    message(FATAL_ERROR "Tesh not found! Please reinstall Simgrid with Tesh activated.")
endif()

MACRO(ADD_TESH NAME)
  SET(ARGT ${ARGV})
  LIST(REMOVE_AT ARGT 0)
  IF(WIN32)
    STRING(REPLACE "§" "\;" ARGT "${ARGT}")
  ENDIF()
  ADD_TEST(${NAME} ${TESH_COMMAND} ${TESH_OPTION} ${ARGT})
ENDMACRO()
