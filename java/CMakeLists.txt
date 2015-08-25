# Copyright (C) 2015. The SimGrid Team. All rights reserved.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Affero Licence (see in file LICENCE).


find_package(Java)
include(UseJava)
set(CMAKE_JAVA_COMPILE_FLAGS "-source" "1.7" "-target" "1.7")

ADD_CUSTOM_COMMAND(
	OUTPUT org/simgrid/rsg/Rsg.java
	DEPENDS ${CMAKE_SOURCE_DIR}/src/rsg.proto
	COMMAND protoc -I ${CMAKE_SOURCE_DIR}/src --java_out=. ${CMAKE_SOURCE_DIR}/src/rsg.proto
)

ADD_CUSTOM_COMMAND(
	TARGET jrsg POST_BUILD
	BYPRODUCT dumbClient
	COMMAND CLASSPATH=jrsg.jar:/usr/share/java/protobuf.jar:. javac ${CMAKE_SOURCE_DIR}/java/dumbClient.java
)
# 	DEPENDS ${CMAKE_SOURCE_DIR}/java/dumbClient.java

set(JAVA_SOURCE
    org/simgrid/rsg/Rsg.java
    org/simgrid/rsg/Engine.java)
    

add_jar(jrsg ${JAVA_SOURCE}
        INCLUDE_JARS /usr/share/java/protobuf.jar)
add_custom_target(jrsg_jar ALL DEPENDS ${JAVA_SOURCE})

get_target_property(_jarFile jrsg JAR_FILE)
get_target_property(_classDir jrsg CLASSDIR)

message(STATUS "Jar file ${_jarFile}")
message(STATUS "Class compiled to ${_classDir}")