#!/bin/bash

# argv[1]: the number of vertices
# argv[2]: the number of edges

red="\033[1;31m"
default="\033[0m"

top_dir="../../"
build_dir="${top_dir}build/"
data="data/"
bin_dir="bin/"

test_generator="generator"
test_driver="driver"
rec_test_driver="recursive_driver"

function build_from_sources
{
    local basic_options="-DCMAKE_BUILD_TYPE=Release"

    cmake ${top_dir} -B ${build_dir} ${basic_options}

    echo -en "\n"
    echo "Building test generator..."
    cmake --build ${build_dir} --target ${test_generator}
    echo -en "\n"

    echo "Building test driver for DFS..."
    cmake --build ${build_dir} --target ${test_driver}
    echo -en "\n"

    echo "Building test driver for recursive DFS..."
    cmake --build ${build_dir} --target ${rec_test_driver}
    echo -en "\n"

    echo "Installing..."
    cmake --install ${build_dir}
    echo -en "\n"
}

function generate_test
{
    local n_vertices=$1
    local n_edges=$2

    mkdir -p ${data}

    echo "Generating test..."
    ${bin_dir}${test_generator} ${n_vertices} ${n_edges} > "${data}${n_vertices}-${n_edges}.test"
    echo -en "\n"
}

function run_test
{
    local n_vertices=$1
    local n_edges=$2

    echo "Running test on DFS..."
    ${bin_dir}${test_driver} < "${data}${n_vertices}-${n_edges}.test"
    echo -en "\n"

    echo "Running test on recursive DFS..."
    ${bin_dir}${rec_test_driver} < "${data}${n_vertices}-${n_edges}.test"
    echo -en "\n"
}

if [ $# -ne 2 ]
then
    echo "${red}Testing script requires exactly 2 arguments${default}"
else
    n_vertices=$1

    if [ $n_vertices -le 0 ]
    then
        echo "${red}The number of vertices has to be a positive integer number${default}"
    else
        n_edges=$2

        if [ $n_edges -le 0 ]
        then
            echo "${red}The number of vertices has to be a positive integer number${default}"
        else
            build_from_sources
            generate_test $n_vertices $n_edges
            run_test $n_vertices $n_edges
        fi
    fi
fi
