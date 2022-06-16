set(CurrLib Convolver)

file(GLOB ${CurrLib}_SOURCES *.cpp;*.h)
file(GLOB ${CurrLib}_HEADERS include/*.h)

set(${CurrLib}_DEPENDS)
list(APPEND ${CurrLib}_DEPENDS ${CMAKE_SOURCE_DIR}/extern/RingBuffer/include/RingBuffer.h)

add_library(${CurrLib}
    STATIC
        ${${CurrLib}_SOURCES}
        ${${CurrLib}_HEADERS}
        ${${CurrLib}_DEPENDS})


target_include_directories(${CurrLib} PUBLIC include)

target_link_libraries(${CurrLib}
    PUBLIC
        RingBuffer
        Utility
        Fft)