find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Threads)

#
# Protobuf/Grpc source files
#
set(PROTO_FILES
        vopy/admin.proto
        vopy/main.proto
        vopy/common.proto
        vopy/session.proto
)

#
# Add Library target with protobuf sources
#
add_library(vopy ${PROTO_FILES})
target_link_libraries(vopy
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc
        gRPC::grpc++
)
target_include_directories(vopy PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

#
# Compile protobuf and grpc files in vopy target to cpp
#
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(TARGET vopy LANGUAGE cpp)
protobuf_generate(TARGET vopy LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")
