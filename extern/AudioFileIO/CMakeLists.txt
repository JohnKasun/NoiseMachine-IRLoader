set(CurrLib AudioFileIO)

set(${CurrLib}_SOURCES AudioFile.cpp AudioFileIf.cpp)
set(${CurrLib}_HEADERS AudioFile.h include/AudioFileIf.h)

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
        sndlib 
    PUBLIC
        Utility)
