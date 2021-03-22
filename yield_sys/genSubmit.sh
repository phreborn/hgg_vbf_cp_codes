tag=ggF

allJobs=jobsSub.sh
> ${allJobs}

sequence=($(seq 1 3 52))

intvl=2
for init in ${sequence[@]};do
  fin=$((${init} + ${intvl}))
  jobName=Collect_${init}_${fin}; echo ${jobName}
  #if [ ! -d csv/${jobName} ];then mkdir -p csv/${jobName};fi
  if [ ! -d submit_${jobName} ]; then mkdir submit_${jobName}; fi
  executable=exe_${jobName}.sh
  > ${executable}

  echo "#!/bin/bash" >> exe_${jobName}.sh
  echo "" >> exe_${jobName}.sh
  echo "cd /scratchfs/atlas/chenhr/atlaswork/VBF_CP/syst/yield_sys" >> exe_${jobName}.sh
  #echo "export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase" >> exe_${jobName}.sh
  #echo "source \${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh" >> exe_${jobName}.sh
  #echo "lsetup \"root 6.20.06-x86_64-centos7-gcc8-opt\"" >> exe_${jobName}.sh
  for id in 346214 343981;do
    echo "./getMag.exe ${id} ${init} ${fin}" >> exe_${jobName}.sh
    echo "sleep 2" >> exe_${jobName}.sh
  done

  chmod +x exe_${jobName}.sh

  echo "hep_sub exe_${jobName}.sh -g atlas -os CentOS7 -wt mid -mem 2048 -o submit_${jobName}/log-0.out -e submit_${jobName}/log-0.err" >> ${allJobs}

  if [ "$(ls submit_${jobName}/)" != "" ];then rm submit_${jobName}/*;fi
done
