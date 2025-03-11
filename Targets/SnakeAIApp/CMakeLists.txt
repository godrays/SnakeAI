#
#  Copyright © 2023-Present, Arkin Terli. All rights reserved.
#
#  NOTICE:  All information contained herein is, and remains the property of Arkin Terli.
#  The intellectual and technical concepts contained herein are proprietary to Arkin Terli
#  and may be covered by U.S. and Foreign Patents, patents in process, and are protected by
#  trade secret or copyright law. Dissemination of this information or reproduction of this
#  material is strictly forbidden unless prior written permission is obtained from Arkin Terli.

set(TARGET_NAME SnakeAIApp)

add_executable(${TARGET_NAME}
        Main.cpp
        NoAICmd.cpp
        GACmd.cpp
)

add_dependencies(${TARGET_NAME} docopt_cpp freetype_cpp sfml_cpp)

if (APPLE)
    target_link_libraries(${TARGET_NAME} PRIVATE
            "-ObjC"
            "-framework Carbon"
            "-framework CoreFoundation"
            "-framework OpenGL"
            "-framework Cocoa"
            "-framework IOKit"
            "-framework AudioUnit"
            sfml-graphics-s
            sfml-window-s
            sfml-audio-s
            sfml-network-s
            sfml-system-s
            docopt
            freetype
            SnakeGameLib
            )
elseif (LINUX)
    target_link_libraries(${TARGET_NAME}
            sfml-graphics-s
            sfml-window-s
            sfml-audio-s
            sfml-network-s
            sfml-system-s
            dl
            pthread
            GL
            docopt
            freetype
            SnakeGameLib
            )
endif()


install(TARGETS ${TARGET_NAME}
        RUNTIME DESTINATION .
)
