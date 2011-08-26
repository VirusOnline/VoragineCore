# Copyright (C) 2005-2011 MaNGOS <http://www.getmangos.com/>
#
# Copyright (C) 2008-2011 Trinity <http://www.trinitycore.org/>
#
# Copyright (C) 2010-2011 Project Voragine <http://www.projectvoragine.com/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

file(GLOB_RECURSE sources_authentication Authentication/*.cpp Authentication/*.h)
file(GLOB_RECURSE sources_realms Realms/*.cpp Realms/*.h)
file(GLOB_RECURSE sources_server Server/*.cpp Server/*.h)
file(GLOB sources_localdir *.cpp *.h)

if( USE_COREPCH )
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
endif()

if ( USE_COREPCH AND MSVC )
  set(logonserver_SRCS
    PrecompiledHeaders/authPCH.cpp
    PrecompiledHeaders/authPCH.h
  )
endif()

set(logonserver_SRCS
  ${logonserver_SRCS}
  ${sources_authentication}
  ${sources_realms}
  ${sources_server}
  ${sources_localdir}
)

if( WIN32 )
set(logonserver_SRCS
    ${logonserver_SRCS}
    ${sources_Debugging}
    logonserver.rc
)
endif()

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/src/server/shared
  ${CMAKE_SOURCE_DIR}/src/server/shared/Database
  ${CMAKE_SOURCE_DIR}/src/server/shared/Debugging
  ${CMAKE_SOURCE_DIR}/src/server/shared/Packets
  ${CMAKE_SOURCE_DIR}/src/server/shared/Cryptography
  ${CMAKE_SOURCE_DIR}/src/server/shared/Cryptography/Authentication
  ${CMAKE_SOURCE_DIR}/src/server/shared/Logging
  ${CMAKE_SOURCE_DIR}/src/server/shared/Threading
  ${CMAKE_SOURCE_DIR}/src/server/shared/Utilities
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Authentication
  ${CMAKE_CURRENT_SOURCE_DIR}/Realms
  ${CMAKE_CURRENT_SOURCE_DIR}/Server
  ${ACE_INCLUDE_DIR}
  ${MYSQL_INCLUDE_DIR}
  ${OPENSSL_INCLUDE_DIR}
)

set(logonserver_LINK_FLAGS "")

add_executable(logonserver
  ${logonserver_SRCS}
)

add_dependencies(logonserver revision.h)

if( NOT WIN32 )
  add_definitions(-D_VORAGINE_REALM_CONFIG='"${CONF_DIR}/logonserver.conf"')
endif()

if( UNIX )
  set(logonserver_LINK_FLAGS "-pthread ${logonserver_LINK_FLAGS}")
endif()

if( CMAKE_SYSTEM_NAME MATCHES "Darwin" )
  set(logonserver_LINK_FLAGS "-framework Carbon ${logonserver_LINK_FLAGS}")
endif()

set_target_properties(logonserver PROPERTIES LINK_FLAGS "${logonserver_LINK_FLAGS}")

target_link_libraries(logonserver
  shared
  ${MYSQL_LIBRARY}
  ${OPENSSL_LIBRARIES}
  ${OPENSSL_EXTRA_LIBRARIES}
  ${OSX_LIBS}
)

if( WIN32 )
  add_custom_command(TARGET logonserver
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/logonserver.conf.dist ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/
  )
endif()

if( UNIX )
  install(TARGETS logonserver DESTINATION bin)
  install(FILES  logonserver.conf.dist DESTINATION etc)
elseif( WIN32 )
  install(TARGETS logonserver DESTINATION "${CMAKE_INSTALL_PREFIX}")
  install(FILES logonserver.conf.dist DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()

# Generate precompiled header
if( USE_COREPCH )
  if(CMAKE_COMPILER_IS_GNUCXX)
    add_precompiled_header(logonserver ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders/authPCH.h)
  elseif(MSVC)
    add_native_precompiled_header(logonserver ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders/authPCH)
  endif()
endif()

