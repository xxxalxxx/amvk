#!/bin/bash

function tag()
{
	echo ""
	echo "*********************"
	echo "TAGGING"
	echo "*********************"
	ctags --fields=+iaS --extra=+q -R *
}

function compile_shaders()
{
	echo ""
	echo "*********************"
	echo "Compile shaders"
	echo "*********************"
	pushd /home/al/amvk/shader

	for filename in *.*; do
		../glslangValidator -V $filename -o spv/$filename.spv
	done

	popd
}

make_clean=false; 
compile_shaders=true
only_shaders=false;

# Transform long options to short ones
for arg in "$@"; do
  shift
  case "$arg" in
    "--clean") set -- "$@" "-c" ;;
    "--skip_shaders") set -- "$@" "-s" ;;
    "--only_shaders") set -- "$@" "-o" ;;
    *)        set -- "$@" "$arg"
  esac
done

while getopts "cso" opt 
do
	case "$opt" in
		"c") make_clean=true ;;
		"s") compile_shaders=false ;;
		"o") only_shaders=true ;;
	esac
done



pushd /home/al/amvk

tag

if $only_shaders; then
	compile_shaders
else 
	pwd
	if $compile_shaders; then
		compile_shaders
	fi

	if $make_clean
	then
		echo "*********************"
		echo "Make clean"
		echo "*********************"

		make clean
		compile_shaders
		make
	else
		echo "*********************"
		echo "Make"
		echo "*********************"
		make
	fi
fi
popd
