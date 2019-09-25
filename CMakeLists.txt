cmake_minimum_required(VERSION 3.12)
project(FindFriendBot)

set(CMAKE_CXX_STANDARD 14)

set(INC_DIR /usr/local/include)
set(LINK_DIR /usr/local/lib)
set(LIB_TOXCORE toxcore)

if (CMAKE_HOST_WIN32)
    set(WINDOWS 1)
elseif(CMAKE_HOST_APPLE)
    set(MACOS 1)
elseif(CMAKE_HOST_UNIX)
    set(LINUX 1)
endif()

set(CMAKE_BUILD_TYPE Release)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

if(MACOS)
    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
elseif(LINUX)
    set(FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR /usr/local/mysql-connector-c++)
endif()

include_directories("src")
include_directories(${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/include/jdbc/)
include_directories(${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/include/jdbc/cppconn)
link_directories(${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/lib64)
include_directories(${INC_DIR})
link_directories(${LINK_DIR})

ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)

find_package(Boost 1.68.0 COMPONENTS program_options regex)
find_package(Boost COMPONENTS regex date_time system filesystem thread graph program_options)
find_package(Boost REQUIRED regex date_time system filesystem thread graph program_options)
find_package(Boost COMPONENTS program_options REQUIRED)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(${PROJECT_NAME}
            src/main.cpp
            src/Bot.cpp
            src/Bot.h
            src/Logger.h
            src/Logger.cpp
            src/Conf.h
            src/Conf.cpp
            src/Assisant.h
            src/Assisant.cpp
            src/DBConn.cpp
            src/DBConn.h
            src/Util.cpp
            src/Util.h
			src/Member.h
			src/Member.cpp
			src/MemberMgr.h
			src/MemberMgr.cpp
			src/Lock.h
			src/LockedQueue.h
            src/MemberModel.cpp
            src/MemberModel.h)
    if(MACOS)
        target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} boost_log-mt boost_log_setup-mt boost_thread-mt boost_system-mt)
    elseif(LINUX)
        target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} boost_log boost_log_setup boost_thread boost_system)
    endif()
endif()

if(MACOS)
    target_link_libraries(${PROJECT_NAME}  libmysqlcppconn.dylib)
    target_link_libraries(${PROJECT_NAME} libtoxcore.dylib)
    target_link_libraries(${PROJECT_NAME} libsodium.dylib)
	target_link_libraries(${PROJECT_NAME} libtcmalloc.dylib)
	target_link_libraries(${PROJECT_NAME} libtcmalloc.dylib)
	target_link_libraries(${PROJECT_NAME} libpthread.dylib)
elseif(LINUX)
    target_link_libraries(${PROJECT_NAME}  libmysqlcppconn.so)
    target_link_libraries(${PROJECT_NAME} libtoxcore.so)
    target_link_libraries(${PROJECT_NAME} libsodium.so)
	target_link_libraries(${PROJECT_NAME} libtcmalloc.so)
	target_link_libraries(${PROJECT_NAME} libpthread.so)
endif()
