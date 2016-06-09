# Copyright (C) 2015. The SimGrid Team. All rights reserved.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Affero Licence (see in file LICENCE).


find_package(Java)
if(Java_FOUND)

    include(UseJava)
    set(CMAKE_JAVA_COMPILE_FLAGS "-source" "1.8" "-target" "1.8")

    # ADD_CUSTOM_COMMAND(
    #         OUTPUT org/simgrid/rsg/Rsg.java
    #         DEPENDS ${CMAKE_SOURCE_DIR}/proto/rsg.proto
    #         COMMAND protoc -I ${CMAKE_SOURCE_DIR}/proto --java_out=. ${CMAKE_SOURCE_DIR}/proto/rsg.proto
    # )
    

    # ADD_CUSTOM_COMMAND(
    #         TARGET jrsg POST_BUILD
    #         BYPRODUCT dumbClient
    #         COMMAND CLASSPATH=jrsg.jar:/usr/share/java/protobuf.jar:. javac ${CMAKE_SOURCE_DIR}/java/dumbClient.java
    # )
    # 	DEPENDS ${CMAKE_SOURCE_DIR}/java/dumbClient.java

    set(JAVA_SOURCE
        "${THRIFT_JAVA_FILES_LIST}"
        org/simgrid/rsg/Main.java
        org/simgrid/rsg/Actor.java
        org/simgrid/rsg/Engine.java
        # org/simgrid/rsg/Mailbox.java
        # org/simgrid/rsg/Rsg.java
    )
    
    set(CLIENT_SOURCE
      dumbClient.java
    )

    add_jar(jrsg_jar ${JAVA_SOURCE} INCLUDE_JARS  "${CMAKE_CURRENT_SOURCE_DIR}/*")

    add_jar(client ${CLIENT_SOURCE} INCLUDE_JARS "${CMAKE_CURRENT_SOURCE_DIR}/*" jrsg_jar)
    
    get_target_property(_jarFile jrsg_jar JAR_FILE)
    get_target_property(_classDir jrsg_jar CLASSDIR)
    
    message(STATUS "Jar file ${_jarFile}")
    message(STATUS "Class compiled to ${_classDir}")
    
    # add_test(NAME TestHelloWorld COMMAND ${Java_JAVA_EXECUTABLE} -cp ${_jarFile} Main "Hello world !")
    
else()
    message(STATUS "Java and /usr/share/java/protobuf.jar not found")
endif()
