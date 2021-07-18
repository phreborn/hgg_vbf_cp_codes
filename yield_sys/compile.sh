#!/bin/bash

#path of all softwares
#/cvmfs/sft.cern.ch/lcg/releases/LCG_97a/

#compile under the same env as condor run in
#export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
#source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh
#lsetup "root 6.20.06-x86_64-centos7-gcc8-opt"

#include lib in x86_64-centos7-gcc8-opt, better choose OS CentOS7 in condor mode
g++ -I. `root-config --cflags --libs` -I/cvmfs/sft.cern.ch/lcg/releases/LCG_97a/vdt/0.4.3/x86_64-centos7-gcc8-opt/include/ -o getMag.exe getMag.cxx
chmod +x getMag.exe

g++ -I. `root-config --cflags --libs` -I/cvmfs/sft.cern.ch/lcg/releases/LCG_97a/vdt/0.4.3/x86_64-centos7-gcc8-opt/include/ -o getMag_jd.exe getMag_jd.cxx
chmod +x getMag_jd.exe
