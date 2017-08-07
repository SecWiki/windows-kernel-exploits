project (BeaEngine)
cmake_minimum_required (VERSION 2.6)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

#set (CMAKE_VERBOSE_MAKEFILE ON)

option (optHAS_OPTIMIZED "Turn Optimizations ON"    OFF)
option (optHAS_SYMBOLS   "Build with debug Symbols" ON)
option (optBUILD_64BIT   "Build 64 bits executable" OFF)
option (optBUILD_DLL     "Build Shared Objects"     OFF)
option (optBUILD_STDCALL "Build using stdcall"      OFF)
option (optBUILD_LITE	 "Build without text disassembly"      OFF)

if (optHAS_OPTIMIZED)
  if (optHAS_SYMBOLS)
    set (CMAKE_BUILD_TYPE relwithdebinfo)
  else (optHAS_SYMBOLS)
    set (CMAKE_BUILD_TYPE release)
  endif (optHAS_SYMBOLS)
else (optHAS_OPTIMIZED)
  if (optHAS_SYMBOLS)
    set (CMAKE_BUILD_TYPE Debug)
  else (optHAS_SYMBOLS)
    set (CMAKE_BUILD_TYPE Debug)
  endif (optHAS_SYMBOLS)
endif (optHAS_OPTIMIZED)

# determine BEA_COMPILER
if (NOT BEA_COMPILER)
  if (${CMAKE_SYSTEM_NAME} STREQUAL Linux)
    if (WATCOM)
      set (BEA_COMPILER watcom)
    else ()
      set (BEA_COMPILER gnu)
    endif ()
  endif ()
  if (${CMAKE_SYSTEM_NAME} STREQUAL FreeBSD)
    set (BEA_COMPILER gnu)
  endif ()
  if (${CMAKE_SYSTEM_NAME} STREQUAL SunOS)
    set (BEA_COMPILER suncc)
  endif ()
  if (${CMAKE_SYSTEM_NAME} STREQUAL Windows)
    if (MINGW OR MSYS)
      set (BEA_COMPILER gnu)
    else ()
      if (CYGWIN)
	set (BEA_COMPILER gnu)
      else ()
	if (BORLAND)
	  set (BEA_COMPILER borland)
	else ()
	  if (MSVC)
	    set (BEA_COMPILER msvc)
	  else ()
	    if (WATCOM)
	      set (BEA_COMPILER watcom)
	    endif ()
	  endif ()
	endif ()
      endif ()
    endif ()
  endif ()
endif ()

# =========================================
# gcc configuration
# =========================================
if (BEA_COMPILER STREQUAL gnu)
  set (CMAKE_C_COMPILER   gcc)
  set (CMAKE_CXX_COMPILER g++)
  set (BEA_WARNINGS -Wall -W -Wextra -Wconversion -Wno-long-long 
    -Wshadow  -Wpointer-arith -Wcast-qual  -Wcast-align -Wwrite-strings)
  set (BEA_FLAGS    -pedantic -ansi -pipe -fno-common -fshort-enums )
  set (BEA_DEFINITIONS "")
  if (optHAS_SYMBOLS)
    list (APPEND BEA_FLAGS -g)
  endif ()
  if (optHAS_OPTIMIZED)
    list (APPEND BEA_FLAGS -fomit-frame-pointer -O2)
  endif ()
  if (optBUILD_64BIT)
    list (APPEND BEA_FLAGS -m64)
  endif ()
  if (optBUILD_STDCALL)
    list (APPEND BEA_DEFINITIONS "-DBEA_USE_STDCALL")
  endif ()
  if (optBUILD_LITE)
    list (APPEND BEA_DEFINITIONS "-DBEA_LIGHT_DISASSEMBLY")
  endif ()  
endif ()
# =========================================
# SunStudio configuration
# =========================================
if (BEA_COMPILER STREQUAL suncc)
  set (CMAKE_C_COMPILER   cc)
  set (CMAKE_CXX_COMPILER CC)
  list (APPEND BEA_FLAGS "-xmemalign=ab")
endif ()
# =========================================
# Visual Studio configuration
# =========================================
if (BEA_COMPILER STREQUAL msvc)
  set (CMAKE_C_COMPILER   cl)
  set (CMAKE_CXX_COMPILER cl)
  set (BEA_DEFINITIONS "/DBEA_LACKS_SNPRINTF /D_CRT_SECURE_NO_WARNINGS")
  
  if (optBUILD_STDCALL)
    set (BEA_DEFINITIONS "${BEA_DEFINITIONS} /DBEA_USE_STDCALL")
  endif ()
  if (optBUILD_LITE)
    set (BEA_DEFINITIONS "${BEA_DEFINITIONS} /DBEA_LIGHT_DISASSEMBLY")
  endif ()    
  if (MSVC60)
	set (BEA_WARNINGS /W3)
  else ()
	if (NOT MSVC90)
		set (BEA_WARNINGS /W3 /Wp64)
	else ()
		set (BEA_WARNINGS /W4)
	endif ()
  endif ()
  if (optHAS_SYMBOLS)
    list (APPEND BEA_FLAGS /ZI)
	list (APPEND BEA_DEFINITIONS "/D_DEBUG=1")
  endif ()
  if (optHAS_OPTIMIZED)
    list (APPEND BEA_FLAGS /O2)
    list (APPEND BEA_DEFINITIONS "/DNDEBUG")
  endif ()

  # determine code generation model
  if (optHAS_OPTIMIZED)
    if (optBUILD_DLL)
      list (APPEND BEA_FLAGS /MD)
    else ()
      list (APPEND BEA_FLAGS /MT)
    endif ()
  else ()
    if (optBUILD_DLL)
      list (APPEND BEA_FLAGS /MDd)
    else ()
      list (APPEND BEA_FLAGS /MTd)
    endif ()
  endif ()
endif ()
# =========================================
# Intel Compiler configuration
# =========================================
if (BEA_COMPILER STREQUAL intel)
  set (CMAKE_C_COMPILER   icc)
  set (CMAKE_CXX_COMPILER icpc)
  set (BEA_WARNINGS -Wall -Wcheck -Wp64 -wd981 -wd1419 -wd1418)
  set (BEA_FLAGS    -ansi -pipe)
  set (BEA_DEFINITIONS "")
  if (optHAS_SYMBOLS)
    list (APPEND BEA_FLAGS -g)
  endif ()
  if (optHAS_OPTIMIZED)
    list (APPEND BEA_FLAGS -fomit-frame-pointer -O3 -ip)
  endif ()
  if (optBUILD_64BIT)
    list (APPEND BEA_FLAGS -m64)
  endif ()
  if (optBUILD_STDCALL)
    list (APPEND BEA_DEFINITIONS "-DBEA_USE_STDCALL")
  endif ()
  if (optBUILD_LITE)
    list (APPEND BEA_DEFINITIONS "-DBEA_LIGHT_DISASSEMBLY")
  endif ()    
endif ()

# =========================================
# Borland C configuration
# =========================================
if (BEA_COMPILER STREQUAL borland)
  set (CMAKE_C_COMPILER   bcc32)
  set (CMAKE_CXX_COMPILER bcc32)
  
  set (BEA_WARNINGS -w -wamb -wdef -wnod -wnak -wcln -wsig -wucp)
  set (BEA_FLAGS    -pc -p- -H- -b -d -Hu-)
  set (BEA_DEFINITIONS "")  
  if (optHAS_SYMBOLS)
    list (APPEND BEA_FLAGS -v -y -R)
  endif ()
  if (optHAS_OPTIMIZED)
    list (APPEND BEA_FLAGS -O2)
  endif ()
  if (optBUILD_64BIT)
	set (BEA_DEFINITIONS "-D_WIN64")
  else ()
	set (BEA_DEFINITIONS "-D_WIN32")
  endif ()	
  if (optBUILD_STDCALL)
    list (APPEND BEA_DEFINITIONS "-DBEA_USE_STDCALL")
  endif ()
  if (optBUILD_LITE)
    list (APPEND BEA_DEFINITIONS "-DBEA_LIGHT_DISASSEMBLY")
  endif ()    
endif ()
# =========================================
# Watcom C configuration
# =========================================
if (BEA_COMPILER STREQUAL watcom)
  set (CMAKE_C_COMPILER   wcl386)
  set (CMAKE_CXX_COMPILER wcl386)
  set (BEA_DEFINITIONS "-DBEA_STL_CONTAINER_REQUIRES_DEFAULT_CTOR")
  set (BEA_WARNINGS -w2 -wx )
  set (BEA_FLAGS    -q -fpi -fpi87 "-bt=nt" -zq -6r -mf)
  if (optHAS_SYMBOLS)
    list (APPEND BEA_FLAGS -db -d2 )
  endif ()
  if (optHAS_OPTIMIZED)
    list (APPEND BEA_FLAGS -ox -s -ors )
  endif ()
  if (optBUILD_64BIT)
#    list (APPEND BEA_FLAGS -m64)
  endif ()
  
  if (optBUILD_STDCALL)
    set (BEA_DEFINITIONS "${BEA_DEFINITIONS} -DBEA_USE_STDCALL")
  endif ()
  if (optBUILD_LITE)
    set (BEA_DEFINITIONS "${BEA_DEFINITIONS} -DBEA_LIGHT_DISASSEMBLY")
  endif ()    
  set (CMAKE_EXE_LINKER_FLAGS "-l=nt")
  set (CMAKE_SHARED_LINKER_FLAGS "-l=nt")
  

endif ()

# =========================================
# Pelles C configuration
# =========================================
if (BEA_COMPILER STREQUAL pelles)
  set (CMAKE_C_COMPILER   pocc)
  #set (CMAKE_CXX_COMPILER pocc)
  
  set (BEA_WARNINGS /W2)
  set (BEA_FLAGS    /Gm /Gn)
  set (BEA_DEFINITIONS "")
  if (optHAS_SYMBOLS)
    list (APPEND BEA_FLAGS /Zi )
  endif ()
  if (optHAS_OPTIMIZED)
    list (APPEND BEA_FLAGS /Ox )
  endif ()
  if (optBUILD_64BIT)
#    list (APPEND BEA_FLAGS -m64)
  endif ()
  if (optBUILD_STDCALL)
    list (APPEND BEA_DEFINITIONS "/DBEA_USE_STDCALL")
  endif ()
  if (optBUILD_LITE)
    list (APPEND BEA_DEFINITIONS "/DBEA_LIGHT_DISASSEMBLY")
  endif ()    
endif ()




# ============================================
# construct compiler flags
# ============================================
set (myC_FLAGS "")
set (myCXX_FLAGS "")

foreach (flag ${BEA_FLAGS})
  set (myC_FLAGS "${myC_FLAGS} ${flag}")
endforeach ()

foreach (flag ${BEA_WARNINGS})
  set (myC_FLAGS "${myC_FLAGS} ${flag}")
endforeach ()

foreach (flag ${BEA_FLAGS})
  set (myCXX_FLAGS "${myCXX_FLAGS} ${flag}")
endforeach ()

foreach (flag ${BEA_WARNINGS})
  set (myCXX_FLAGS "${myCXX_FLAGS} ${flag}")
endforeach ()

# ================================================
# pass compiler flags to cmake
# ================================================

if (${CMAKE_BUILD_TYPE} STREQUAL RelWithDebInfo)
  set (CMAKE_C_FLAGS_RELWITHDEBINFO   ${myC_FLAGS})
  set (CMAKE_CXX_FLAGS_RELWITHDEBINFO ${myCXX_FLAGS})
endif ()

if (${CMAKE_BUILD_TYPE} STREQUAL Release)
  set (CMAKE_C_FLAGS_RELEASE   ${myC_FLAGS})
  set (CMAKE_CXX_FLAGS_RELEASE ${myCXX_FLAGS})
endif ()

if (${CMAKE_BUILD_TYPE} STREQUAL DebugFull)
  set (CMAKE_C_FLAGS_DEBUGFULL   ${myC_FLAGS})
  set (CMAKE_CXX_FLAGS_DEBUGFULL ${myCXX_FLAGS})
endif ()

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
  set (CMAKE_C_FLAGS_DEBUG   ${myC_FLAGS})
  set (CMAKE_CXX_FLAGS_DEBUG ${myCXX_FLAGS})
endif ()


set (BEA_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/include)
set (BEA_SRC_ROOT ${CMAKE_SOURCE_DIR}/beaengineSources)



# ======================================
# set output dirs
# ======================================

set (myQualification "${CMAKE_SYSTEM_NAME}.${BEA_COMPILER}.${CMAKE_BUILD_TYPE}")
set (myLIB_OUTPUT "${CMAKE_SOURCE_DIR}/lib/${myQualification}" )
set (myBIN_OUTPUT "${CMAKE_SOURCE_DIR}/bin/${myQualification}")
set (myOBJ_OUTPUT obj)

if (optBUILD_64BIT)
  set (myLIB_OUTPUT "${myLIB_OUTPUT}.64" )
  set (myBIN_OUTPUT "${myBIN_OUTPUT}.64")
  set (myOBJ_OUTPUT "${myOBJ_OUTPUT}.64")
endif ()

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${myLIB_OUTPUT})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${myLIB_OUTPUT})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${myBIN_OUTPUT})



FIND_PACKAGE (ZLIB)


set (build_modules 
  beaengineSources
  #examples
  #unittest
)



if (NOT ZLIB_FOUND)
  set (ZLIB_INCLUDE_DIR unittest/thirdparty/zlib)
else ()
  list (APPEND BEA_LIBS_PATH ${ZLIB_LIBRARIES})
endif ()
  

add_definitions     (${BEA_DEFINITIONS})
include_directories (${BEA_SRC_ROOT}  ${BEA_INCLUDE_PATH} 
                     ${CMAKE_SOURCE_DIR} ${ZLIB_INCLUDE_DIR})
link_directories    (${BEA_LIBS_PATH})


set (BEA_TARGET "BeaEngine")

if (NOT optBUILD_DLL)
  set (BEA_TARGET "${BEA_TARGET}_s")
endif ()

if (NOT optHAS_OPTIMIZED)
  set (BEA_TARGET "${BEA_TARGET}_d")
endif ()

if (optBUILD_STDCALL)
  set (BEA_TARGET "${BEA_TARGET}_stdcall")
endif ()

if (optBUILD_64BIT)
  set (BEA_TARGET "${BEA_TARGET}_64")
endif ()

 

foreach (mdl ${build_modules})
  set (MDL_SRC "${mdl}")
  set (MDL_OBJ "${myOBJ_OUTPUT}/${myQualification}/${mdl}")
  add_subdirectory (${MDL_SRC} ${MDL_OBJ})
endforeach () 
