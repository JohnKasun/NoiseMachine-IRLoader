set(CurrLib sndlib)

file(GLOB ${CurrLib}_SOURCES *.c;*.cpp;*.txt)
file(GLOB ${CurrLib}_HEADERS *.h)

# remove the files with main functions
file(GLOB to_remove RELATIVE ${CMAKE_SOURCE_DIR}/extern/${CurrLib} *sndplay.c;*sndinfo.c)
list(REMOVE_ITEM ${CurrLib}_SOURCES ${to_remove})

add_library(${CurrLib} STATIC ${${CurrLib}_SOURCES} ${${CurrLib}_HEADERS})
target_include_directories(${CurrLib} PUBLIC .)

set_target_properties(${CurrLib} PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(${CurrLib} PROPERTIES OUTPUT_NAME "${CurrLib}")
set_target_properties(${CurrLib} PROPERTIES PREFIX "lib")

if(MSVC)
    set_target_properties(${CurrLib} PROPERTIES COMPILE_FLAGS "/W1 /DMUS_WINDOZE")
endif(MSVC)

if (APPLE)
	set_target_properties(${CurrLib} PROPERTIES COMPILE_FLAGS "-w")
endif(APPLE)
