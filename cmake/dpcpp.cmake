

set(CMAKE_DEFENCE_FLAGS "${CMAKE_DEFENCE_FLAGS} -fPIC -Wformat -Wformat-security -Wall -Werror")
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -fsycl")
set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
