set(APP ConvolverExec)

add_executable(${APP})

file(GLOB ${APP}_SOURCES *.cpp)
file(GLOB ${APP}_HEADERS include/*.h)

set(${APP}_DEPEND)
list(APPEND ${APP}_DEPEND ${CMAKE_SOURCE_DIR}/src/Convolver/include/ConvolverIf.h)
list(APPEND ${APP}_DEPEND ${CMAKE_SOURCE_DIR}/extern/AudioFileIO/include/AudioFileIf.h)

target_sources(${APP} 
        PRIVATE
                ${${APP}_SOURCES}
                ${${APP}_HEADERS}
                ${${APP}_DEPEND})

target_link_libraries(${APP}
        PRIVATE
                Utility
                AudioFileIO
                RingBuffer
                Convolver
)