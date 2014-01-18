file(GLOB sources_localdir *.cpp *.h)
file(GLOB_RECURSE sources_Product product/*.cpp product/*.h)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(HTUPDATE_STAT_SRCS 
	${sources_Product}
	${sources_localdir})
include_directories(${CMAKE_BINARY_DIR}
  		    ${CMAKE_CURRENT_SOURCE_DIR}
		    ${CMAKE_SOURCE_DIR}/src/shared/log
		    ${CMAKE_SOURCE_DIR}/src/shared/thread
		    ${CMAKE_SOURCE_DIR}/src/shared/config
		    ${CMAKE_SOURCE_DIR}/src/shared/Utilities
		    ${sources_Product})
ADD_EXECUTABLE("htupdate" ${HTUPDATE_STAT_SRCS})
