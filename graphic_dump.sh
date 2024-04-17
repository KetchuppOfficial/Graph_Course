#!/bin/bash

function usage()
{
    local message=$1
    echo "Error: $message"
    echo "Usage: /path/to/run.sh [OPTIONS]"
    echo ""
    echo "-v <number-of-vertices>"
    echo "-e <number-of-edges>"
    exit 1
}

N_ARGS=$#
if [ $N_ARGS -ne 6 ]; then # each option takes 2 command line arguments
    usage "Script requires 3 options"
fi

NUM="^[0-9]+$"

while getopts :v:e:o: OPT; do
    case $OPT in
    v) # -v <number-of-vertices>
        n_vertices=$OPTARG
        [[ $n_vertices =~ $NUM ]] || usage "-v is followed not by an integer number"
        [[ $n_vertices -gt 0 ]] || usage "-v is followed by a non-positive integer number"
	    ;;
	e) # -e <number-of-edges>
	    n_edges=$OPTARG
        [[ $n_edges =~ $NUM ]] || usage "-e is followed not by an integer number"
        [[ $n_edges -gt 0 ]] || usage "-e is followed by a non-positive integer number"
	    ;;
    o) # -o <output-name>
        output_name=$OPTARG
        ;;
	*)
	    usage "Invalid command line argument $OPTARG"
	    ;;
  esac
done

extension=${output_name##*.}

script_dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
build_dir=$script_dir/build

cmake -B $build_dir -DCMAKE_BUILD_TYPE=Release
cmake --build $build_dir --target generator graphic_dump

$build_dir/test/end-to-end/generator --n-vertices $n_vertices --n-edges $n_edges --no-self-loops | \
$build_dir/test/end-to-end/graphic_dump | \
dot -T$extension -o $output_name
