#!/bin/csh
source ~/workspace/logic/set_env.sh
set args = ""
foreach arg ($*)
    set args = "$args $arg"
end
echo "Running simulation with args: $args"
/tools/snps/vcs/current/bin/vcs $args

