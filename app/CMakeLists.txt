cmake_minimum_required(VERSION 3.4.1)


#定義源文件目錄位置變量
set(ROOT_SRC_DIR ${PROJECT_SOURCE_DIR}/src/main/cpp)
set(BASE ${ROOT_SRC_DIR}/base)
set(JNI_LIBS "${CMAKE_SOURCE_DIR}/src/main/jniLibs")
set(FFMPEG_INC ${ROOT_SRC_DIR}/ffmpeg)
set(BASIC_DATA_TYPE_SRC ${ROOT_SRC_DIR}/basicdatatype/)
set(BASIC_FFMPEG_SRC ${ROOT_SRC_DIR}/basicffmpeg/)


#設置自定義lib輸出位置
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${JNI_LIBS}/${ANDROID_ABI})


#設置cpp源文件目錄
aux_source_directory(${BASE} base)
aux_source_directory(${BASIC_DATA_TYPE_SRC} basicdatatypesrc)
aux_source_directory(${BASIC_FFMPEG_SRC} basicffmpegsrc)


#引入源文件.h目录
include_directories(${FFMPEG_INC})
include_directories(${BASE})
include_directories(${BASIC_DATA_TYPE_SRC})
include_directories(${BASIC_FFMPEG_SRC})


#添加共享庫搜索路徑
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI})


#import android log
find_library(
        # Sets the name of the path variable.
        log-lib
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log )

#file(GLOB native_srcs "src/main/cpp/basicffmpeg/*.cpp")
#添加libbasicffmpeg.so庫
add_library(
        # Sets the name of the library.
        basicffmpeg
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        ${base}
        ${basicffmpegsrc})

target_link_libraries(
        basicffmpeg
        # for dynamic
        avcodec avdevice avformat avfilter avutil swresample swscale postproc
        # for static
        #        libavformat.a libavcodec.a libavfilter.a  libavutil.a libswresample.a libswscale.a libass.a libfontconfig.a libexpat.a
        #        libfreetype.a libfribidi.a libmp3lame.a libx264.a
        ${log-lib})

#添加libbasicdatatype.so庫
add_library( # Sets the name of the library.
        basicdatatype
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        ${base}
        ${basicdatatypesrc})

target_link_libraries(
        basicdatatype
        ${log-lib})