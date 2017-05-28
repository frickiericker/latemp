#!/bin/sh -efu
python test05-dump_temperature.py
python test09-temphole.py < test05-temp.tsv > test09-temp.tsv
python test08-temp_to_lat.py < test09-temp.tsv > test09-lat.tsv

: ${CXX:=c++}
: ${CXXFLAGS:=}
: ${LDFLAGS:=}
${CXX} ${CXXFLAGS} -std=c++14 -O2 -o test07 test07-optimization.cc ${LDFLAGS}
./test07 > test07-lat.tsv
