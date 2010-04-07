project(kwin4_effect_pulse)

cmake_minimum_required(VERSION 2.4)
add_definitions( ${QT_DEFINITIONS} )
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )
find_package(KDE4)
add_definitions( ${KDE4_DEFINITIONS} )

find_package(X11)
include(KDE4Defaults)
include(MacroLibrary)
include_directories (${KDE4_INCLUDES})


kde4_add_plugin(kwin4_effect_pulse pulse.cpp)


target_link_libraries(kwin4_effect_pulse ${KDE4_KDEUI_LIBS} kwineffects ${X11_LIBRARIES})

install(TARGETS kwin4_effect_pulse DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES pulse.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kwin )
