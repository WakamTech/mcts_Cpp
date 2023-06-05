cmake_minimum_required(VERSION 3.0.0)
project(mcts VERSION 0.1.0)

include(CTest)
enable_testing()
set(CMAKE_CXX_STANDARD 23)


set(Torch_DIR "/home/Desktop/opt/libtorch/share/cmake/Torch")

find_package(Torch REQUIRED)
set(SOURCES
        main.cpp
        scrs/Agent.cpp
        scrs/Node.cpp
        scrs/State.cpp
        scrs/Tree.cpp
    )

set(HEADERS 
        include/Agent.hpp
        include/Move.hpp
        include/Node.hpp
        include/State.hpp
        include/Tree.hpp
        include/Load_CSV.hpp
        include/CustomDataset.hpp
        include/Model.hpp
)

include_directories(include)



add_executable(mcts ${SOURCES})

target_link_libraries(mcts ${TORCH_LIBRARIES})


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})


include(CPack)