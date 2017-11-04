
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

set(bsynergy_SRCS
    main.cpp
)

set(bsynergy_net_SRCS
    net/blistenserver.cpp
    net/bclient.cpp
)

if(WIN32)
    message("Using win32 platform")
    set(bsynergy_platform_SRCS
        platform/bmswindowsscreen.cpp
    )

    set(bsynergy_platform_windows_hook_SRCS
        platform/bmswindowskeytranslator.cpp
        platform/bmswindowsthread.cpp
    )

    set(bsynergy_platform_windows_hook_INCL
        platform/bmswindowskeytranslator.h
        platform/bmswindowsthread.h
    )
else (WIN32)
    message("Using Unix platform")
    set(bsynergy_platform_SRCS
        platform/bx11screen.cpp
        platform/bx11controlthread.cpp
        platform/bx11keytranslator.cpp
    )
#else(UNIX)
#    message(FATAL_ERROR "Unsupported OS")
endif(WIN32)

set(bsynergy_gui_SRCS
    gui/bscreenlayoutwidget.cpp
    gui/bscreenlistwidget.cpp
    gui/bscreenlayoutconfig.cpp
    gui/bnewscreendialog.cpp
    gui/bsynergywidget.cpp
)

set(bsynergy_server_SRCS
    server/bsynergyserver.cpp
)

set(bsynergy_client_SRCS
    client/bsynergyclient.cpp
)

set(bsynergy_gui_UI
    gui/bnewscreendialog.ui
    gui/bsyngerywidget.ui
)

IF(WIN32)
    ADD_LIBRARY(bsynergyh SHARED ${bsynergy_platform_windows_hook_SRCS} ${bsynergy_platform_windows_hook_INCL})
    target_link_libraries(bsynergyh ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Network_LIBRARIES} ${Qt5Widgets_LIBRARIES})
ENDIF(WIN32)

include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS})

add_executable(bsynergy ${bsynergy_SRCS} ${bsynergy_net_SRCS} ${bsynergy_platform_SRCS} ${bsynergy_gui_SRCS} ${bsynergy_server_SRCS} ${bsynergy_client_SRCS})
target_link_libraries(bsynergy ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Network_LIBRARIES} ${Qt5Widgets_LIBRARIES})

IF(UNIX)
    target_link_libraries(bsynergy ${X11_XTest_LIB} ${X11_X11_LIB})
ENDIF(UNIX)

IF(WIN32)
    target_link_libraries(bsynergy bsynergyh)
ENDIF(WIN32)
