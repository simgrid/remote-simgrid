# - Find Thrift (a cross platform RPC lib/tool)
# This module defines
#  THRIFT_VERSION_STRING, version string of ant if found
#  THRIFT_LIBRARIES, libraries to link
#  THRIFT_INCLUDE_DIR, where to find THRIFT headers
#  THRIFT_COMPILER, thrift compiler executable
#  THRIFT_FOUND, If false, do not try to use ant
# Function
#  thrift_gen_cpp(<path to thrift file> <output variable with file list>)
#
# Initial work was done by Cloudera https://github.com/cloudera/Impala
# 2014 - modified by snikulov

# prefer the thrift version supplied in THRIFT_HOME (cmake -DTHRIFT_HOME then environment)
find_path(THRIFT_INCLUDE_DIR
    NAMES
        thrift/Thrift.h
    HINTS
        ${THRIFT_HOME}
        ENV THRIFT_HOME
        /usr/local
        /opt/local
    PATH_SUFFIXES
        include
)

# prefer the thrift version supplied in THRIFT_HOME
find_library(THRIFT_LIBRARIES
    NAMES
        thrift libthrift
    HINTS
        ${THRIFT_HOME}
        ENV THRIFT_HOME
        /usr/local
        /opt/local
    PATH_SUFFIXES
        lib lib64
)

find_program(THRIFT_COMPILER
    NAMES
        thrift
    HINTS
        ${THRIFT_HOME}
        ENV THRIFT_HOME
        /usr/local
        /opt/local
    PATH_SUFFIXES
        bin bin64
)





if (THRIFT_COMPILER)
    exec_program(${THRIFT_COMPILER}
        ARGS -version OUTPUT_VARIABLE __thrift_OUT RETURN_VALUE THRIFT_RETURN)
    string(REGEX MATCH "[0-9]+.[0-9]+.[0-9]+(-[a-z]+)*$" THRIFT_VERSION_STRING ${__thrift_OUT})

    # define utility function to generate cpp files
    function(thrift_gen_cpp thrift_file THRIFT_CPP_FILES_LIST THRIFT_GEN_INCLUDE_DIR)
        set(_res)
        set(_res_inc_path)
        if(EXISTS "${CMAKE_SOURCE_DIR}/${thrift_file}")
            get_filename_component(_target_dir "${CMAKE_SOURCE_DIR}/${thrift_file}" NAME_WE)
            message("thrift_gen_cpp: ${CMAKE_SOURCE_DIR}/${thrift_file}")

            if(NOT EXISTS ${CMAKE_BINARY_DIR}/${_target_dir})
                file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${_target_dir})
            endif()
            message("Executing ${THRIFT_COMPILER} -r -o ${CMAKE_BINARY_DIR}/${_target_dir} --gen cpp ${CMAKE_SOURCE_DIR}/${thrift_file}")
            exec_program(${THRIFT_COMPILER}
                ARGS -r -o "${CMAKE_BINARY_DIR}/${_target_dir}" --gen cpp "${CMAKE_SOURCE_DIR}/${thrift_file}"
                OUTPUT_VARIABLE __thrift_OUT
                RETURN_VALUE THRIFT_RETURN)
            if(NOT "${THRIFT_RETURN}" EQUAL "0")
                message(FATAL_ERROR "Thrift exited with a value of ${THRIFT_RETURN}: ${__thrift_OUT}")
            endif()
            
            file(GLOB_RECURSE __result_src "${CMAKE_BINARY_DIR}/${_target_dir}/*.cpp")

            file(GLOB_RECURSE __skeletons "${CMAKE_BINARY_DIR}/${_target_dir}/*.skeleton.cpp")

            message("Thrift: You can find skeleton(s) for your server(s) here: ${__skeletons}")
            list(REMOVE_ITEM __result_src ${__skeletons})

            file(GLOB_RECURSE __result_hdr "${CMAKE_BINARY_DIR}/${_target_dir}/*.h")
            list(APPEND _res ${__result_src})
            list(APPEND _res ${__result_hdr})
            if(__result_hdr)
                list(GET __result_hdr 0 _res_inc_path)
                get_filename_component(_res_inc_path ${_res_inc_path} DIRECTORY)
            endif()



            add_library(${thrift_file} STATIC ${_res})
target_link_libraries(${thrift_file} ${THRIFT_LIBRARIES})

configure_file(${thrift_file} "${CMAKE_BINARY_DIR}/${_target_dir}" COPYONLY)



        else()
            message("thrift_gen_cpp: file ${CMAKE_SOURCE_DIR}/${thrift_file} does not exists")
        endif()
        set(${THRIFT_CPP_FILES_LIST} "${_res}" PARENT_SCOPE)
        set(${THRIFT_GEN_INCLUDE_DIR} "${_res_inc_path}" PARENT_SCOPE)
    endfunction()
    
    # define utility function to generate cpp files
    function(thrift_gen_java thrift_file THRIFT_JAVA_FILES_LIST THRIFT_GEN_JAVA_INCLUDE_DIR)
        set(_res)
        set(_res_inc_path)
        if(EXISTS "${CMAKE_SOURCE_DIR}/${thrift_file}")
            get_filename_component(_target_dir "${CMAKE_SOURCE_DIR}/${thrift_file}" NAME_WE)
            message("thrift_gen_java: ${CMAKE_SOURCE_DIR}/${thrift_file}")

            if(NOT EXISTS ${CMAKE_BINARY_DIR}/${_target_dir})
                file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${_target_dir})
            endif()
            exec_program(${THRIFT_COMPILER}
                ARGS -r -o "${CMAKE_BINARY_DIR}/${_target_dir}" --gen java "${CMAKE_SOURCE_DIR}/${thrift_file}"
                OUTPUT_VARIABLE __thrift_OUT
                RETURN_VALUE THRIFT_RETURN)
            file(GLOB_RECURSE __result_src "${CMAKE_BINARY_DIR}/${_target_dir}/*.java")

            set(${THRIFT_JAVA_FILES_LIST} "${__result_src}" PARENT_SCOPE)
            set(${THRIFT_GEN_JAVA_INCLUDE_DIR} "${_res_inc_path}" PARENT_SCOPE)
          endif ()

    endfunction()
    
endif ()






include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(THRIFT DEFAULT_MSG THRIFT_LIBRARIES THRIFT_INCLUDE_DIR THRIFT_COMPILER)
mark_as_advanced(THRIFT_LIBRARIES THRIFT_INCLUDE_DIR THRIFT_COMPILER THRIFT_VERSION_STRING)
