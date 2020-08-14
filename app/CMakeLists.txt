cmake_minimum_required(VERSION 3.4.1)

#C++编译
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(jnilibs "${CMAKE_SOURCE_DIR}/src/main/jniLibs")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${jnilibs}/${ANDROID_ABI})

#第三方so存放的根目录
SET(
    third_path
    ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}
    )
#引入源文件.h目录
include_directories( ${CMAKE_SOURCE_DIR}/src/main/cpp/include)


#FFMpeg库位置根目录
SET(
    ffmpeg_path
    ${third_path}
)
#设置libavcodec.so
add_library(
              libavcodec
              SHARED
              IMPORTED
)

set_target_properties(libavcodec
                      PROPERTIES IMPORTED_LOCATION
                      ${ffmpeg_path}/libavcodec.so
)
#设置libavdevice.so
add_library(libavdevice
              SHARED
              IMPORTED
)

set_target_properties(libavdevice
                      PROPERTIES IMPORTED_LOCATION
                      ${ffmpeg_path}/libavdevice.so
)

#设置libavfilter.so
add_library(libavfilter
               SHARED
               IMPORTED
 )

set_target_properties(libavfilter
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_path}/libavfilter.so
 )

#设置libavformat.so
add_library(libavformat
              SHARED
              IMPORTED
)

set_target_properties(libavformat
                      PROPERTIES IMPORTED_LOCATION
                      ${ffmpeg_path}/libavformat.so
)

#设置libavutil.so
add_library(libavutil
              SHARED
              IMPORTED
)

set_target_properties(libavutil
                      PROPERTIES IMPORTED_LOCATION
                      ${ffmpeg_path}/libavutil.so
)

#设置libpostproc.so
add_library(libpostproc
              SHARED
              IMPORTED
)

set_target_properties(libpostproc
                      PROPERTIES IMPORTED_LOCATION
                      ${ffmpeg_path}/libpostproc.so
)

#设置libswresample.so
add_library(libswresample
               SHARED
               IMPORTED
 )

set_target_properties(libswresample
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_path}/libswresample.so
 )

#设置libswscale.so
add_library(libswscale
               SHARED
               IMPORTED
 )

set_target_properties(libswscale
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_path}/libswscale.so
 )


add_library( # Sets the name of the library.
              basicffmpeg
              # Sets the library as a shared library.
              SHARED
             # Provides a relative path to your source file(s).
            src/main/cpp/BasicFFMpeg.cpp)

find_library( # Sets the name of the path variable.
               log-lib

               # Specifies the name of the NDK library that
               # you want CMake to locate.
               log )

target_link_libraries(
                       basicffmpeg
                       libavcodec
                       libavdevice
                       libavfilter
                       libavformat
                       libavutil
                       libpostproc
                       libswresample
                       libswscale
                       ${log-lib})

add_library( # Sets the name of the library.
        basicdatatype
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        src/main/cpp/BasicDataType.cpp
        src/main/cpp/FLVData.cpp)
target_link_libraries(
        basicdatatype
        ${log-lib})