cmake_minimum_required(VERSION 3.15)
project(lab1 C)

set(CMAKE_C_STANDARD 99)


add_executable(main src/pal/main.c)

add_executable(server-queue src/client-server/Part_1/message_queue_server.c)
add_executable(client-queue src/client-server/Part_1/message_queue_client.c)

add_executable(server-mmap src/client-server/Part_1/mmap_server.c)
add_executable(client-mmap src/client-server/Part_1/mmap_client.c)

add_executable(server-shared-memory src/client-server/Part_1/shared_memory_server.c)
add_executable(client-shared-memory src/client-server/Part_1/shared_memory_client.c)

add_executable(mutex src/client-server/Part_2/lock_mutex.c)

add_executable(unnamedPipe src/client-server/Part_3/unnamed_pipe.c)

add_executable(loba src/pal/main.c src/pal/logs.c src/pal/pipes.c src/pal/ipc.c)

add_executable(test src/learnC/testProg.c)

add_executable(twoPipes src/pal/twoPipes.c)