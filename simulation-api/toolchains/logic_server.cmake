message("Creating logic server target")
# Set the system name
set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_C_COMPILER /tools/snps/vcs/S-2021.09-SP2-9-T/gnu/linux/gcc-9.2.0_64-shared/bin/gcc)
set(CMAKE_CXX_COMPILER /tools/snps/vcs/S-2021.09-SP2-9-T/gnu/linux/gcc-9.2.0_64-shared/bin/g++)
set(Boost_ROOT "/tools/misc/boost/1.83.0")
set(Boost_INCLUDEDIR "${Boost_ROOT}/include")
set(Boost_LIBRARYDIR "${Boost_ROOT}/lib")
set(Boost_NO_SYSTEM_PATHS ON)
set(SVDPI_INCLUDE_DIR /tools/cdnc/xcelium/23.09.004/tools.lnx86/include)


set(Protobuf_INCLUDE_DIR /tools/misc/protocomp/2.5.0/usr/include/)
set(Protobuf_LIBRARY /tools/misc/protocomp/2.5.0/usr/lib64/libprotobuf.so)