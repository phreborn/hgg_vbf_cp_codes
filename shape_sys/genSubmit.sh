tag=ggF

allJobs=jobsSub.sh
> ${allJobs}

sequence=($(seq 1 1 82))

intvl=0
for sysSet in photonallsys1 photonallsys2
do
for init in ${sequence[@]};do
  fin=$((${init} + ${intvl}))
  jobName=Collect_${sysSet}_${init}_${fin}; echo ${jobName}
  condorDir=hep_sub_${jobName}
  #if [ ! -d csv/${jobName} ];then mkdir -p csv/${jobName};fi
  if [ ! -d ${condorDir} ]; then mkdir ${condorDir}; fi
  executable=exe_${jobName}.sh
  > ${executable}

  echo "#!/bin/bash" >> exe_${jobName}.sh
  echo "" >> exe_${jobName}.sh
  echo "cd /scratchfs/atlas/huirun/atlaswork/VBF_CP/syst/shape_sys" >> exe_${jobName}.sh
  echo "export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase" >> exe_${jobName}.sh
  echo "source \${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh" >> exe_${jobName}.sh
  echo "lsetup \"root 6.20.06-x86_64-centos7-gcc8-opt\"" >> exe_${jobName}.sh
  #echo "root -b -l -q getMag.cxx\(${init},${fin}\) | tee -a ${condorDir}/log.fitStatus" >> exe_${jobName}.sh
  echo "rm -r /publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/shape/csv/${jobName}" >> exe_${jobName}.sh
  echo "./bin/getMag.exe ${init} ${fin} ${sysSet} | tee -a ${condorDir}/log.fitStatus" >> exe_${jobName}.sh

  chmod +x exe_${jobName}.sh

  echo "hep_sub exe_${jobName}.sh -g atlas -os CentOS7 -wt mid -mem 2048 -o ${condorDir}/log-0.out -e ${condorDir}/log-0.err" >> ${allJobs}

  if [ "$(ls ${condorDir}/)" != "" ];then rm ${condorDir}/*;fi
done
done
