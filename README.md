# redesigned-train folder:
test led manipulator server. spagetti code

compiled and tested for Windows with: 
g++ server.cpp -o -g -Wall server
g++ client.cpp -o -g -Wall client

tested with valgrind:
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind-out.txt \
         ./server1.exe 8088

# new version build (need boost_asio head library):

mkdir build && cd build 
cmake ..
cmake --build .
