#!/bin/bash

pushd /home/al/code/amvk
ctags --fields=+iaS --extra=+q --language-force=C++ -R *
popd
