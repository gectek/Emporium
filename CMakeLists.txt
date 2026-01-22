cmake_minimum_required(VERSION 3.10)

project(ClientApp C)

# Build an executable called "client" using both .c files
add_executable(
    client
    client.c
    sockutil.c
)
#Second executable
add_executable(
    server
    server.c
    sockutil.c
)

# Optional but recommended: show warnings
target_compile_options(client PRIVATE -Wall -Wextra -Werror)

