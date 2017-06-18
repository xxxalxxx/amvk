#!/bin/bash

function tag()
{
	echo "*********************"
	echo "TAGGING"
	echo "*********************"
	ctags --fields=+iaS --extra=+q -R *
}

function compile_shaders()
{
	./glslangValidator -V shader/quad.frag -o shader/spv/quad.frag.spv
	./glslangValidator -V shader/quad.vert -o shader/spv/quad.vert.spv
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



pushd /home/al/amvk

tag

pwd
if $compile_shaders; then
	echo "*********************"
	echo "Compile shaders"
	echo "*********************"
	compile_shaders
fi

if $make_clean
then
	echo "*********************"
	echo "Make clean"
	echo "*********************"

	make -j3 clean
	compile_shaders
	make
else
	echo "*********************"
	echo "Make"
	echo "*********************"
	make -j3
fi

popd
