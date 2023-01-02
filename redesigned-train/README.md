# redesigned-train
test led manipulator server. spagetti code

compiled and tested for Windows with: 
g++ server.cpp -o -g -Wall server1
g++ client.cpp -o -g -Wall client1

tested with valgrind:
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind-out.txt \
         ./server1.exe 8088
