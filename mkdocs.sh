#!/bin/bash

this_dir=$(dirname "${BASH_SOURCE[0]}")

docbuildfolder=${this_dir}/build/docsbuild

(cd doxygen-awesome-css && git pull origin main)
rm -rf ${docbuildfolder}
cmake -S . -B ${docbuildfolder}
cmake --build ${docbuildfolder} --target doc
rm -rf ${this_dir}/docs
cp -a -v ${docbuildfolder}/doc/html docs

