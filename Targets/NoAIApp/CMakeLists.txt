#
#  Copyright © 2023-Present, Arkin Terli. All rights reserved.
#
#  NOTICE:  All information contained herein is, and remains the property of Arkin Terli.
#  The intellectual and technical concepts contained herein are proprietary to Arkin Terli
#  and may be covered by U.S. and Foreign Patents, patents in process, and are protected by
#  trade secret or copyright law. Dissemination of this information or reproduction of this
#  material is strictly forbidden unless prior written permission is obtained from Arkin Terli.

add_executable(NoAIApp
        main.cpp
        )

if (APPLE)
    target_link_libraries(NoAIApp PRIVATE
            "-ObjC"
            "-framework Carbon"
            "-framework CoreFoundation"
            "-framework OpenGL"
            "-framework Cocoa"
            "-framework IOKit"
            "-framework AudioUnit"
            bz2_static
            z
            freetype
            sfml-audio-s
            sfml-graphics-s
            sfml-network-s
            sfml-system-s
            sfml-window-s
            SnakeGameLib
            )
elseif (LINUX)
    target_link_libraries(NoAIApp
            dl pthread GL
            bz2_static
            z
            freetype
            sfml-audio-s
            sfml-graphics-s
            sfml-network-s
            sfml-system-s
            sfml-window-s
            SnakeGameLib
            )
endif()
