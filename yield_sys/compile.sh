#!/bin/bash

#path of all softwares
#/cvmfs/sft.cern.ch/lcg/releases/LCG_97a/

g++ -I. `root-config --cflags --libs` -I/cvmfs/sft.cern.ch/lcg/releases/LCG_97a/vdt/0.4.3/x86_64-centos7-gcc8-opt/include/ -o getMag.exe getMag.cxx 
