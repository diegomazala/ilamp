cmake_minimum_required (VERSION 2.6)

#cmake_policy(SET CMP0008 NEW)

project (ilamp)


find_package( Eigen3 REQUIRED )
if( NOT EIGEN3_INCLUDE_DIR )
    set( EIGEN3_INCLUDE_DIR "$ENV{EIGEN3_INCLUDE_DIR}" )
endif()



set(FLANN_LIBRARY
	flann_cpp_s.lib
	)
		
set(FLANN_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/3rdparty/flann/lib/Release_x64/)
set(FLANN_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/flann/include/)



add_executable(3d2Nd 
	src/3d2Nd.cpp 
	src/tinyply.cpp 
	src/tinyply.h)
target_compile_features(3d2Nd PRIVATE cxx_std_17)
target_link_libraries(3d2Nd PRIVATE $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>)


add_executable(detail_displace 
	src/detail_displace.cpp 
	src/tinyply.cpp 
	src/tinyply.h)
target_compile_features(detail_displace PRIVATE cxx_std_17)


add_executable(nd2ply 
	src/nd2ply.cpp 
	src/tinyply.cpp 
	src/tinyply.h)
target_compile_features(nd2ply PRIVATE cxx_std_17)
target_link_libraries(nd2ply PRIVATE $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>)
	
	
add_executable(rbf_imp 
	src/rbf_imp_main.cpp 
	src/rbf_imp.h 
	src/tinyply.cpp 
	src/tinyply.h)
target_include_directories (rbf_imp PUBLIC ${EIGEN3_INCLUDE_DIR} ${FLANN_INCLUDE_DIR})
target_compile_features(rbf_imp PRIVATE cxx_std_17)
target_link_libraries(rbf_imp ${FLANN_LIBRARY_DIR}${FLANN_LIBRARY} )
	
	
add_executable(rbf_sample 
	src/rbf_sample.cpp
	src/rbf_interpolation.h 
	src/rbf_interpolation.cpp)
target_include_directories (rbf_sample PUBLIC ${EIGEN3_INCLUDE_DIR} ${FLANN_INCLUDE_DIR})
target_link_libraries(rbf_sample ${FLANN_LIBRARY_DIR}${FLANN_LIBRARY})
	
	
add_executable(replace_vertices 
	src/replace_vertices.cpp
	src/ilamp_utils.h
	src/tinyply.h 
	src/tinyply.cpp)
target_compile_features(replace_vertices PRIVATE cxx_std_17)
target_link_libraries(replace_vertices PRIVATE $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>)

	


#add_library(ilamp_dll SHARED
#	src/ilamp_dll.cpp 
#	src/ilamp.h
#	src/ilamp_project.h
#	src/rbf_interpolation.h
#	src/rbf_interpolation.cpp
#	src/tinyply.cpp
#	src/tinyply.h
#)

#target_include_directories (ilamp_dll PUBLIC $ENV{EIGEN3_INCLUDE_DIR} ${FLANN_INCLUDE_DIR})
#target_link_libraries(ilamp_dll ${FLANN_LIBRARY_DIR}${FLANN_LIBRARY})



set(UNITY_SOURCE_FILES
	src/Unity/IUnityGraphics.h
	src/Unity/IUnityGraphicsD3D11.h
	src/Unity/IUnityGraphicsD3D12.h
	src/Unity/IUnityInterface.h
	src/Unity/PlatformBase.h
	src/Unity/RenderAPI.cpp
	src/Unity/RenderAPI.h
	src/Unity/RenderAPI_D3D11.cpp
	src/Unity/RenderAPI_D3D12.cpp
	src/Unity/RenderingPlugin.cpp
	src/Unity/RenderingPlugin.def
)
set(IMP_HEADER_FILES 
	src/imp_dll.h
	src/ilamp.h
	src/imp_dll.h
	src/rbf_imp.h
)

set(IMP_SOURCE_FILES 
	src/imp_dll.cpp
)

#find_package( OpenCV REQUIRED )

#add_library(imp SHARED
#	${UNITY_SOURCE_FILES}
#	${IMP_HEADER_FILES}
#	${IMP_SOURCE_FILES}
#)
#target_include_directories (imp PUBLIC 
#	${OpenCV_INCLUDE_DIRS} 
#	$ENV{EIGEN3_INCLUDE_DIR} 
#	${FLANN_INCLUDE_DIR}
#)

#target_link_libraries(imp 
#	${FLANN_LIBRARY_DIR}${FLANN_LIBRARY}
#	${OpenCV_LIBS}
#)

source_group("Unity"	FILES ${UNITY_SOURCE_FILES})
