# CMake generated Testfile for 
# Source directory: E:/spdlog/spdlog-master/tests
# Build directory: E:/spdlog/spdlog-master-build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(spdlog-utests "E:/spdlog/spdlog-master-build/tests/Debug/spdlog-utests.exe")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(spdlog-utests "E:/spdlog/spdlog-master-build/tests/Release/spdlog-utests.exe")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(spdlog-utests "E:/spdlog/spdlog-master-build/tests/MinSizeRel/spdlog-utests.exe")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(spdlog-utests "E:/spdlog/spdlog-master-build/tests/RelWithDebInfo/spdlog-utests.exe")
else()
  add_test(spdlog-utests NOT_AVAILABLE)
endif()
