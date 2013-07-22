## Public Domain

project (Chipmunk)
    set(CHIPMUNK_SOURCE  
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/chipmunk.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/cpArray.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/cpArbiter.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/cpBB.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/cpBBTree.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/cpBody.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/cpCollision.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/cpHashSet.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/cpPolyShape.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/cpShape.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/cpSpace.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/cpSpaceComponent.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/cpSpaceQuery.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/cpSpaceStep.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/cpSpatialIndex.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/cpSweep1D.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/cpPolyShape.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/cpVect.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/constraints/cpConstraint.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/constraints/cpDampedRotarySpring.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/constraints/cpDampedSpring.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/constraints/cpGearJoint.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/constraints/cpGrooveJoint.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/constraints/cpPinJoint.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/constraints/cpPivotJoint.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/constraints/cpRatchetJoint.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/constraints/cpRotaryLimitJoint.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/constraints/cpSimpleMotor.c
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk/constraints/cpSlideJoint.c
        )
    list(APPEND CHIPMUNK_SOURCE ${CHIPMUNK_SOURCE})

    list(APPEND CHIPMUNK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND CHIPMUNK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/howlingmoon)
    list(APPEND CHIPMUNK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk)
    
    include_directories(${CHIPMUNK_INCLUDE_DIRS} )
    add_library (Chipmunk SHARED ${CHIPMUNK_SOURCE})
    
if (${CMAKE_C_COMPILER_ID} EQUAL "MSVC")
	set_source_files_properties(${CHIPMUNK_SOURCE} PROPERTIES LANGUAGE CXX)
	set_target_properties(Chipmunk PROPERTIES LINKER_LANGUAGE CXX)
endif()

    install (TARGETS Chipmunk
        DESTINATION ${CMAKE_INSTALL_PREFIX}/release 
        CONFIGURATIONS Release RelWithDebInfo)  
    install (TARGETS Chipmunk
        DESTINATION ${CMAKE_INSTALL_PREFIX}/debug 
        CONFIGURATIONS Debug)  


project (Chipmunk++)
    set(CHIPMUNK_SOURCE    
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunkpp/bb.cpp
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunkpp/body.cpp
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunkpp/chipmunk.cpp
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunkpp/circleshape.cpp
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunkpp/polyshape.cpp
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunkpp/segmentshape.cpp
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunkpp/shape.cpp
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunkpp/space.cpp
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunkpp/types.cpp
            ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunkpp/vect.cpp
        )
    list(APPEND CHIPMUNK_SOURCE ${CHIPMUNK_SOURCE})

    list(APPEND CHIPMUNK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR})
    list(APPEND CHIPMUNK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/howlingmoon)
    list(APPEND CHIPMUNK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunk)
    list(APPEND CHIPMUNK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/external/howlingmoon/chipmunkpp)
    
    list(APPEND CHIPMUNK_LIBRARIES Chipmunk)   
    
    include_directories(${CHIPMUNK_INCLUDE_DIRS} )
    add_library (Chipmunk++ SHARED ${CHIPMUNK_SOURCE})
    target_link_libraries(Chipmunk++ ${CHIPMUNK_LIBRARIES})
    
    install (TARGETS Chipmunk++
        DESTINATION ${CMAKE_INSTALL_PREFIX}/release 
        CONFIGURATIONS Release RelWithDebInfo)  
    install (TARGETS Chipmunk++
        DESTINATION ${CMAKE_INSTALL_PREFIX}/debug 
        CONFIGURATIONS Debug)  
