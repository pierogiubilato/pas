# CMakeList.txt : CMake project for miniroot, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Chek for root presence.
find_package(ROOT REQUIRED COMPONENTS RIO Net)

# Set project name.
project ("pas")

# Add main target executable.
add_executable (pas)

# Set language version. 
set_property(TARGET pas PROPERTY CXX_STANDARD 17)
set_property(TARGET pas PROPERTY CXX_STANDARD_REQUIRED YES)
#set_property(TARGET pas PROPERTY COMPILE_FLAGS -Zc:__cplusplus) # This is required to full root compatibility.
set_property(TARGET pas PROPERTY COMPILE_FLAGS ${ROOT_CXX_FLAGS}) # Ensures full root libraries compatibility.

# Add ROOT includes.
target_include_directories(pas PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/include	
	${ROOT_INCLUDE_DIRS}
)

# Add ROOT libraries.
target_link_libraries (pas PRIVATE 
    ${ROOT_LIBRARIES}
)

# Add target own code.
target_sources(pas PRIVATE 
	 "src/pasMain.cpp"
	 "src/pasMisc.cpp"
	 
	 "src/pasObjAna.cpp"
	 "src/pasObjCluster.cpp"
	 "src/pasObjLayer.cpp"
	 
	 "src/pasAnaInfo.cpp"
	 "src/pasAnaReco.cpp"
	 "src/pasAnaSetup.cpp"
	 "src/pasAnaStat.cpp"
	 #"src/pasAnaTest.cpp"
)



# TODO: Add tests and install targets if needed.
