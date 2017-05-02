#!/bin/bash

function tag()
{
	ctags -R .
}

function compile_shaders()
{
	./glslangValidator -V shader/shader.frag -o shader/spv/shader.frag.spv
	./glslangValidator -V shader/shader.vert -o shader/spv/shader.vert.spv
}


make_clean=false; compile_shaders=true

# Transform long options to short ones
for arg in "$@"; do
  shift
  case "$arg" in
    "--clean") set -- "$@" "-c" ;;
    "--skip_shaders") set -- "$@" "-s" ;;
    *)        set -- "$@" "$arg"
  esac
done

while getopts "cs" opt 
do
	case "$opt" in
		"c") make_clean=true ;;
		"s") compile_shaders=false ;;
	esac
done

pushd /home/al/code/amvk
pwd
if $compile_shaders; then
	echo "Compile shaders"
	compile_shaders
fi

if $make_clean
then
	echo "Make clean"
	make clean
	compile_shaders
	make
else
	echo "Make"
	make
fi

popd
