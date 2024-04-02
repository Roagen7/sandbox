## Description
A simple container runtime made as a final project for OS software course.
Features:
- running containerized processes in premade rootfs
- overlayfs
- capabilities dropping
- dns/network setup
- basic seccomp configuration


Dependencies:
- libseccomp


## Building
```
mkdir build

cmake -DCMAKE_BUILD_TYPE=Debug -B./build -G Ninja
 
cmake --build build/ --config Debug --target sandboxer_poc 
```

## Running

example:
```
sudo build/sandboxer_poc --exec /bin/bash --rootfs images/ubuntu --name jammy --container_dir containers/jammy --seccomp
```