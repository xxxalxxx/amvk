#!/bin/bash
#ctags -R .

./glslangValidator -V shader/shader.frag -o shader/spv/shader.frag.spv
./glslangValidator -V shader/shader.vert -o shader/spv/shader.vert.spv
make

