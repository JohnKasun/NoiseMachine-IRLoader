add_library(RingBuffer 
    INTERFACE 
        include/RingBuffer.h
)

target_include_directories(RingBuffer INTERFACE include/)