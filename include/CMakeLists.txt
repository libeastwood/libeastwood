INCLUDE_DIRECTORIES(${LIBEASTWOOD_SOURCE_DIR}/include)


SET( LIBEASTWOOD_HEADERS
  eastwood/AdlFile.h
  eastwood/Buffer.h
  eastwood/CpsFile.h
  eastwood/Decode.h
  eastwood/Dune2File.h
  eastwood/EmcFileAssemble.h
  eastwood/EmcFileBase.h
  eastwood/EmcFileDisassemble.h
  eastwood/Exception.h
  eastwood/ExeFile.h
  eastwood/Font.h
  eastwood/IcnFile.h
  eastwood/IniFile.h
  eastwood/IStream.h
  eastwood/MapFile.h
  eastwood/OStream.h
  eastwood/PakFile.h
  eastwood/Palette.h
  eastwood/PalFile.h
  eastwood/ShpFile.h
  eastwood/Sound.h
  eastwood/StringFile.h
  eastwood/Surface.h
  eastwood/VocFile.h
  eastwood/WsaFile.h
)

SET( LIBEASTWOOD_ADLIB_HEADERS
  eastwood/adl/adlibemu.h
  eastwood/adl/analopl.h
  eastwood/adl/diskopl.h
  eastwood/adl/emuopl.h
  eastwood/adl/fmopl.h
  eastwood/adl/kemuopl.h
  eastwood/adl/opl.h
  eastwood/adl/realopl.h
  eastwood/adl/silentopl.h  
  eastwood/adl/temuopl.h
)

SET( LIBEASTWOOD_SCALER_HEADERS
  eastwood/scaler/scalebit.h
)

SET( SDL_LIBEASTWOOD_HEADERS
    eastwood/SDL/Mixer/Player.h
    eastwood/SDL/Mixer/Sound.h
    eastwood/SDL/Palette.h    
    eastwood/SDL/Surface.h
)

INSTALL( FILES ${LIBEASTWOOD_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eastwood" )
INSTALL( FILES ${LIBEASTWOOD_ADLIB_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eastwood/adl" )
INSTALL( FILES ${LIBEASTWOOD_SCALER_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eastwood/scaler" )


IF(LIBEASTWOOD_BUILD_SDL)
INSTALL( FILES ${SDL_LIBEASTWOOD_HEADERS} DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eastwood/SDL" )
ENDIF(LIBEASTWOOD_BUILD_SDL)

