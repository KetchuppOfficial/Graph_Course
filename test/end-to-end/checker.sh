#!/bin/bash

# argv[1]: the number of vertices
# argv[2]: the number of edges

red="\033[1;31m"
green="\033[1;32m"
default="\033[0m"

script_dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
top_dir="${script_dir}/../../"
build_dir="${top_dir}build/"
data="${script_dir}/data/"
bin_dir="${script_dir}/bin/"

test_generator="generator"
test_driver="driver"
rec_test_driver="recursive_driver"

function build_from_sources
{
    local basic_options="-DCMAKE_BUILD_TYPE=Release"

    cmake ${top_dir} -B ${build_dir} ${basic_options}

    echo -en "\n"
    echo -e "${green}Building test generator...${default}"
    cmake --build ${build_dir} --target ${test_generator}
    echo -en "\n"

    echo -e "${green}Building test driver for DFS...${default}"
    cmake --build ${build_dir} --target ${test_driver}
    echo -en "\n"

    echo -e "${green}Building test driver for recursive DFS...${default}"
    cmake --build ${build_dir} --target ${rec_test_driver}
    echo -en "\n"

    echo -e "${green}Installing...${default}"
    cmake --install ${build_dir}
    echo -en "\n"
}

function generate_test
{
    local n_vertices=$1
    local n_edges=$2

    mkdir -p ${data}

    echo -e "${green}Generating test...${default}"
    ${bin_dir}${test_generator} ${n_vertices} ${n_edges} > "${data}${n_vertices}-${n_edges}.test"
    echo -en "\n"
}

function run_test
{
    local n_vertices=$1
    local n_edges=$2

    echo -e "${green}Running test on DFS...${default}"
    ${bin_dir}${test_driver} < "${data}${n_vertices}-${n_edges}.test"
    echo -en "\n"

    echo -e "${green}Running test on recursive DFS..${default}"
    ${bin_dir}${rec_test_driver} < "${data}${n_vertices}-${n_edges}.test"
    echo -en "\n"
}

if [ $# -ne 2 ]
then
    echo -e "${red}Testing script requires exactly 2 arguments${default}"
else
    n_vertices=$1

    if [ $n_vertices -le 0 ]
    then
        echo -e "${red}The number of vertices has to be a positive integer number${default}"
    else
        n_edges=$2

        if [ $n_edges -le 0 ]
        then
            echo -e "${red}The number of vertices has to be a positive integer number${default}"
        else
            build_from_sources
            generate_test $n_vertices $n_edges
            run_test $n_vertices $n_edges
        fi
    fi
fi
