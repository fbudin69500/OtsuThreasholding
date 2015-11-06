CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
CMAKE_POLICY(VERSION 2.6)
PROJECT(OtsuThresholding)


#####################     ITK    ############################################
  FIND_PACKAGE(ITK REQUIRED)
  IF(USE_ITK_FILE)
    INCLUDE(${USE_ITK_FILE})
  ENDIF(USE_ITK_FILE)


set(Filter OtsuThresholding)

set(${Filter}_SOURCE ${Filter}.cxx)
add_executable( ${Filter} ${${Filter}_SOURCE})
target_link_libraries( ${Filter}  ${ITK_LIBRARIES})

