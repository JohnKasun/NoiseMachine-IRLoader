set(APP ConvolverPlugin)

juce_add_plugin(${APP}
    PLUGIN_MANUFACTURER_CODE John
    PLUGIN_CODE Conv
    FORMATS AU AUv3 VST3 Standalone
    PRODUCT_NAME "Convolver Plugin"
    COMPANY_NAME JohnKasun)

file(GLOB ${APP}_SOURCES *.cpp)
file(GLOB ${APP}_HEADERS include/*.h)

set(${APP}_DEPEND)
list(APPEND ${APP}_DEPEND ${CMAKE_SOURCE_DIR}/src/Convolver/include/Convolver.h)

target_sources(${APP}
    PRIVATE
        ${${APP}_SOURCES}
        ${${APP}_HEADERS}
        ${${APP}_DEPEND})

target_include_directories(${APP}
    PUBLIC
        include)

target_compile_definitions(${APP}
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(${APP}
    PRIVATE
        juce::juce_audio_utils
        Utility
        Convolver
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)