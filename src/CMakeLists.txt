INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

SET( LIBEASTWOOD_SRCS
  AdlFile.cpp
  CpsFile.cpp
  Decode.cpp
  Font.cpp
  IcnFile.cpp
  IniFile.cpp
  PakFile.cpp
  PalFile.cpp
  ShpFile.cpp
  StringFile.cpp
  VocFile.cpp
  WsaFile.cpp
)

SET( LIBEASTWOOD_HEADERS
  AdlFile.h
  CpsFile.h
  Decode.h
  Font.h
  IcnFile.h
  IniFile.h
  Log.h
  PakFile.h
  PalFile.h
  ShpFile.h
  StringFile.h
  VocFile.h
  WsaFile.h
)

INSTALL( FILES ${LIBEASTWOOD_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eastwood" )

ADD_LIBRARY( eastwood SHARED ${LIBEASTWOOD_SRCS} )

TARGET_LINK_LIBRARIES(eastwood ${SDL_LIBRARY} )
TARGET_LINK_LIBRARIES(eastwood ${SDLMIXER_LIBRARY} )
TARGET_LINK_LIBRARIES(eastwood ${LIBSAMPLERATE_LIBRARY} )


SET_TARGET_PROPERTIES( eastwood PROPERTIES VERSION "${LIBEASTWOOD_VERSION_INFO}" )
SET_TARGET_PROPERTIES( eastwood PROPERTIES SOVERSION "${LIBEASTWOOD_SOVERSION_INFO}" )

INSTALL(TARGETS eastwood LIBRARY DESTINATION ${LIB_INSTALL_DIR} )

