# - Try to find OpenGL, glut includes and libraries
# Once done this will define:
#  GL_FOUND        - system has OpenGL and glut
#  GL_INCLUDE_DIR  - incude paths to use OpenGL and glut
#  GL_LIBRARIES    - Link these to use OpenGL and glut

IF(UNIX)
    FIND_PATH(GL_INCLUDE_DIR
        gl.h
        /usr/local/include/GL
        /usr/include/GL
        )

    FIND_LIBRARY(GL_LIBRARIES
        GL
        /usr/local/lib
        /usr/lib
        )

    FIND_PATH(GLU_INCLUDE_DIR
        glu.h
        /usr/local/include/GL
        /usr/include/GL
        )

    FIND_LIBRARY(GLU_LIBRARIES
        GLU
        /usr/local/lib
        /usr/lib
        )

    FIND_PATH(GLUT_INCLUDE_DIR
        glut.h
        /usr/local/include/GL
        /usr/include/GL
        )

    FIND_LIBRARY(GLUT_LIBRARIES
        glut
        /usr/local/lib
        /usr/lib
        )
        
    IF(GL_LIBRARIES AND GL_INCLUDE_DIR AND GLU_LIBRARIES AND GLU_INCLUDE_DIR AND GLUT_LIBRARIES AND GLUT_INCLUDE_DIR)
        SET(GL_FOUND 1 CACHE STRING "Set to 1 if GL is found, 0 otherwise")
        INCLUDE_DIRECTORIES(BEFORE ${GL_INCLUDE_DIR})
        INCLUDE_DIRECTORIES(BEFORE ${GLU_INCLUDE_DIR})
        INCLUDE_DIRECTORIES(BEFORE ${GLUT_INCLUDE_DIR})
    ELSE(GL_LIBRARIES AND GL_INCLUDE_DIR AND GLU_LIBRARIES AND GLU_INCLUDE_DIR AND GLUT_LIBRARIES AND GLUT_INCLUDE_DIR)
        SET (GL_FOUND 0 CACHE STRING "Set to 1 if GL is found, 0 otherwise")
    ENDIF(GL_LIBRARIES AND GL_INCLUDE_DIR AND GLU_LIBRARIES AND GLU_INCLUDE_DIR AND GLUT_LIBRARIES AND GLUT_INCLUDE_DIR)
ENDIF(UNIX)

SET(GL_LIBRARIES ${GL_LIBRARIES} ${GLU_LIBRARIES} ${GLUT_LIBRARIES})

MARK_AS_ADVANCED( GL_FOUND )
