INCLUDE_DIRECTORIES(${LIBEASTWOOD_SOURCE_DIR}/src)

SET( LIBEASTWOOD_SRCS
  AdlFile.cpp
  CpsFile.cpp
  Decode.cpp
  Dune2File.cpp
  Exception.cpp
  ExeFile.cpp
  Font.cpp
  IcnFile.cpp
  IniFile.cpp
  IStream.cpp
  MapFile.cpp
  OStream.cpp
  PakFile.cpp
  PalFile.cpp
  ShpFile.cpp
  Sound.cpp
  StringFile.cpp
  Surface.cpp
  VocFile.cpp
  WsaFile.cpp
  adl/adlibemu.c
  adl/analopl.cpp
  adl/diskopl.cpp
  adl/emuopl.cpp
  adl/fmopl.c
  adl/realopl.cpp
  adl/temuopl.cpp
  emc/EmcFileBase.cpp
  emc/EmcFileAssemble.cpp
  emc/EmcFileDisassemble.cpp
  scaler/scale2xARM.s
  scaler/scale2x.cpp
  scaler/scale3x.cpp
  scaler/scalebit.cpp
)

SET( LIBEASTWOOD_HEADERS
  AdlFile.h
  CpsFile.h
  Decode.h
  Dune2File.h
  Exception.h
  ExeFile.h
  Font.h
  IcnFile.h
  IniFile.h
  IStream.h
  MapFile.h
  OStream.h
  Log.h
  PakFile.h
  PalFile.h
  ShpFile.h
  Sound.h
  StringFile.h
  Surface.h
  VocFile.h
  WsaFile.h
)

SET( LIBEASTWOOD_ADLIB_HEADERS
  adl/adlibemu.h
  adl/analopl.h
  adl/diskopl.h
  adl/emuopl.h
  adl/fmopl.h
  adl/kemuopl.h
  adl/opl.h
  adl/realopl.h
  adl/silentopl.h  
  adl/temuopl.h
)

SET( LIBEASTWOOD_EMC_HEADERS
  emc/EmcFileBase.h
  emc/EmcFileAssemble.h
  emc/EmcFileDisassemble.h
)

SET( LIBEASTWOOD_SCALER_HEADERS
  scaler/scalebit.h
)

SET( SDL_LIBEASTWOOD_SRCS
    SDL/MixerPlayer.cpp
    SDL/MixerSound.cpp
    SDL/Surface.cpp
)

SET( SDL_LIBEASTWOOD_HEADERS
    SDL/MixerPlayer.h
    SDL/MixerSound.h
    SDL/Surface.h
)

SET( DUNE2_SCRIPT_TOOLS
    emc/dst.cpp
)

SET( PYEASTWOOD_SRCS
    python/pycpsfile.cpp
    python/pyeastwood.cpp
    python/pyicnfile.cpp
    python/pymapfile.cpp
    python/pypakfile.cpp
    python/pypalfile.cpp
    python/pyshpfile.cpp
    python/pysound.cpp
    python/pystringfile.cpp
    python/pysurface.cpp
    python/pyvocfile.cpp
)

ADD_LIBRARY( eastwood SHARED ${LIBEASTWOOD_SRCS} )
TARGET_LINK_LIBRARIES( eastwood ${LIBSAMPLERATE_LIBRARIES} )
SET_TARGET_PROPERTIES( eastwood PROPERTIES SOVERSION "${LIBEASTWOOD_SOVERSION_INFO}" )
INSTALL(TARGETS eastwood LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
INSTALL( FILES ${LIBEASTWOOD_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eastwood" )
INSTALL( FILES ${LIBEASTWOOD_ADLIB_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eastwood/adl" )
INSTALL( FILES ${LIBEASTWOOD_EMC_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eastwood/emc" )
INSTALL( FILES ${LIBEASTWOOD_SCALER_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eastwood/scaler" )


IF(LIBEASTWOOD_BUILD_SDL)
ADD_LIBRARY( SDL_eastwood SHARED ${SDL_LIBEASTWOOD_SRCS} )
TARGET_LINK_LIBRARIES( SDL_eastwood eastwood )
TARGET_LINK_LIBRARIES( eastwood ${SDL_LIBRARY} )
TARGET_LINK_LIBRARIES( eastwood ${SDLMIXER_LIBRARY} )
SET_TARGET_PROPERTIES( SDL_eastwood PROPERTIES SOVERSION "${LIBEASTWOOD_SOVERSION_INFO}" )
INSTALL(TARGETS SDL_eastwood LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
INSTALL( FILES ${SDL_LIBEASTWOOD_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eastwood/SDL" )
ENDIF(LIBEASTWOOD_BUILD_SDL)

IF(LIBEASTWOOD_BUILD_DST)
ADD_EXECUTABLE( dst ${DUNE2_SCRIPT_TOOLS} )
TARGET_LINK_LIBRARIES( dst eastwood )
ENDIF(LIBEASTWOOD_BUILD_DST)

IF(LIBEASTWOOD_BUILD_PYTHON)
  GET_FILENAME_COMPONENT(PYTHON_LIB_INSTALL "${PYTHON_LIBRARY}" PATH)
  INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_PATH} )
  PYTHON_ADD_MODULE( pyeastwood ${PYEASTWOOD_SRCS})
  TARGET_LINK_LIBRARIES( pyeastwood eastwood )
  SET_TARGET_PROPERTIES(pyeastwood PROPERTIES PREFIX "")
ENDIF(LIBEASTWOOD_BUILD_PYTHON)

