# Copyright (C) 2015. The SimGrid Team. All rights reserved.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Affero Licence (see in file LICENCE).


find_package(Java QUIET)
if(!Java_FOUND AND EXISTS /usr/share/java/protobuf.jar)

    include(UseJava)
    set(CMAKE_JAVA_COMPILE_FLAGS "-source" "1.7" "-target" "1.7")

    ADD_CUSTOM_COMMAND(
            OUTPUT org/simgrid/rsg/Rsg.java
            DEPENDS ${CMAKE_SOURCE_DIR}/proto/rsg.proto
            COMMAND protoc -I ${CMAKE_SOURCE_DIR}/proto --java_out=. ${CMAKE_SOURCE_DIR}/proto/rsg.proto
    )

    ADD_CUSTOM_COMMAND(
            TARGET jrsg POST_BUILD
            BYPRODUCT dumbClient
            COMMAND CLASSPATH=jrsg.jar:/usr/share/java/protobuf.jar:. javac ${CMAKE_SOURCE_DIR}/java/dumbClient.java
    )
    # 	DEPENDS ${CMAKE_SOURCE_DIR}/java/dumbClient.java

    set(JAVA_SOURCE
        org/simgrid/rsg/Actor.java
        org/simgrid/rsg/Engine.java
        org/simgrid/rsg/Mailbox.java
        org/simgrid/rsg/Rsg.java
    )


    add_jar(jrsg ${JAVA_SOURCE}
            INCLUDE_JARS /usr/share/java/protobuf.jar)

    add_custom_target(jrsg_jar ALL DEPENDS ${JAVA_SOURCE})
    add_custom_target(dumbClient ALL DEPENDS ${JAVA_SOURCE})

    get_target_property(_jarFile jrsg JAR_FILE)
    get_target_property(_classDir jrsg CLASSDIR)

    message(STATUS "Jar file ${_jarFile}")
    message(STATUS "Class compiled to ${_classDir}")
else()
    message(STATUS "Java and /usr/share/java/protobuf.jar not found")
endif()
