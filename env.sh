#!/bin/sh 
export PYTHONPATH=~/mestrado/lib/:~/mestrado/submodules/pymobility/src/

rm cscope.files cscope.out
find . -name '*.py' >> cscope.files
find . -name '*.p4' >> cscope.files
find . -name '*.h' >> cscope.files
cscope -pkR
