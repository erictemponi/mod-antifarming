CU_SET_PATH("CMAKE_ANTIFARMING_DIR" "${CMAKE_CURRENT_LIST_DIR}")

AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/AntiFarming.cpp")
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/AntiFarming.h")
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/cs_antifarming.cpp")
AC_ADD_SCRIPT_LOADER("AntiFarming" "${CMAKE_CURRENT_LIST_DIR}/src/loader.h")

CU_ADD_HOOK(AFTER_WORLDSERVER_CMAKE "${CMAKE_CURRENT_LIST_DIR}/src/cmake/after_ws_install.cmake")