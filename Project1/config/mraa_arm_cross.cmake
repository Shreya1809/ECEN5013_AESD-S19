SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)

SET(CMAKE_C_COMPILER /home/shreya1809/buildroot/output/host/usr/bin/arm-linux-gcc)
SET(CMAKE_CXX_COMPILER /home/shreya1809/buildroot/output/host/usr/bin/arm-linux-g++)

SET(CMAKE_FIND_ROOT_PATH /home/shreya1809/buildroot/output/host/usr/arm-buildroot-linux-gnueabihf)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET(SYSTEM_USR_DIR /home/shreya1809/buildroot/output/host/usr/arm-buildroot-linux-gnueabihf)
