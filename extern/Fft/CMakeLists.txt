set(CurrLib Fft)

set(${CurrLib}_SOURCES Fft.cpp rvfft.cpp)
set(${CurrLib}_HEADERS include/Fft.h rvfft.h)

add_library(${CurrLib} 
    STATIC 
        ${${CurrLib}_SOURCES} 
        ${${CurrLib}_HEADERS})

target_include_directories(${CurrLib} PUBLIC include)

set_target_properties(${CurrLib} PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(${CurrLib} PROPERTIES OUTPUT_NAME "${CurrLib}")
set_target_properties(${CurrLib} PROPERTIES PREFIX "lib")

target_link_libraries(${CurrLib}
    PRIVATE
        Utility)