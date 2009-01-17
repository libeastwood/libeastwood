SET( libeastwood_SRCS
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

SET( libeastwood_HEADERS
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

ADD_LIBRARY( eastwood SHARED ${libeastwood_SRCS} )

TARGET_LINK_LIBRARIES(eastwood ${SDLMAIN_LIBRARY} )
TARGET_LINK_LIBRARIES(eastwood ${SDLMIXER_LIBRARY} )
TARGET_LINK_LIBRARIES(eastwood ${LIBSAMPLERATE_LIBRARY} )


SET_TARGET_PROPERTIES( eastwood PROPERTIES VERSION "${LIBEASTWOOD_VERSION_INFO}" )
SET_TARGET_PROPERTIES( eastwood PROPERTIES SOVERSION "${LIBEASTWOOD_SOVERSION_INFO}" )
