tag=ggF

basedir=/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/yield/

allJobs=jobsSub.sh
> ${allJobs}

sequence=($(seq 1 3 52))

intvl=2
for sysSet in photonsys jetsys
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
  echo "cd /scratchfs/atlas/huirun/atlaswork/VBF_CP/syst/yield_sys" >> exe_${jobName}.sh
  #echo "export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase" >> exe_${jobName}.sh
  #echo "source \${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh" >> exe_${jobName}.sh
  #echo "lsetup \"root 6.20.06-x86_64-centos7-gcc8-opt\"" >> exe_${jobName}.sh
  echo "rm -r ${basedir}/csv/${jobName}" >> exe_${jobName}.sh
  for id in 346214 343981;do
    echo "./getMag.exe ${id} ${init} ${fin} ${sysSet}" >> exe_${jobName}.sh
    echo "sleep 2" >> exe_${jobName}.sh
  done

  chmod +x exe_${jobName}.sh

  echo "hep_sub exe_${jobName}.sh -g atlas -os CentOS7 -wt mid -mem 2048 -o ${condorDir}/log-0.out -e ${condorDir}/log-0.err" >> ${allJobs}

  if [ "$(ls ${condorDir}/)" != "" ];then rm ${condorDir}/*;fi
done
done
