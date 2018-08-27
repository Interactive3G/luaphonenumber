# luaphonenumber
Lua bindings for libphonenumber

To compile it, first clone Google's libphonenumber report @ https://github.com/googlei18n/libphonenumber.git

You only need to compile the CPP libraries (see cpp/README file)

sudo apt-get install cmake cmake-curses-gui libprotobuf-dev libgtest-dev libre2-dev libicu-dev libboost-dev libboost-thread-dev libboost-system-dev protobuf-compiler

cd cpp
mkdir build
cmake ..
make install
