set(APP Tests)

add_executable(${APP})

file(GLOB ${APP}_SOURCES *.cpp)
file(GLOB ${APP}_HEADERS include/*.h; include/*.hpp)

set(${APP}_DEPEND)
list(APPEND ${APP}_DEPEND ${CMAKE_SOURCE_DIR}/src/Convolver/include/Convolver.h)

target_include_directories(${APP} PUBLIC include)
target_sources(${APP} 
    PRIVATE
        ${${APP}_SOURCES}
        ${${APP}_HEADERS}
        ${${APP}_DEPEND}
)
target_link_libraries(${APP}
    PRIVATE
        Utility
        Convolver
)