#!/bin/bash
mkdir gen
python -m grpc_tools.protoc -I proto --python_betterproto_out=gen   public_api.proto