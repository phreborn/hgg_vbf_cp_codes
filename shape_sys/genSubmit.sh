tag=ggF

allJobs=jobsSub.sh
> ${allJobs}

sequence=($(seq 1 1 82))

intvl=0
for init in ${sequence[@]};do
  fin=$((${init} + ${intvl}))
  jobName=Collect_${init}_${fin}; echo ${jobName}
  #if [ ! -d csv/${jobName} ];then mkdir -p csv/${jobName};fi
  if [ ! -d submit_${jobName} ]; then mkdir submit_${jobName}; fi
  executable=exe_${jobName}.sh
  > ${executable}

  echo "#!/bin/bash" >> exe_${jobName}.sh
  echo "" >> exe_${jobName}.sh
  echo "cd /scratchfs/atlas/huirun/atlaswork/VBF_CP/syst/shape_sys" >> exe_${jobName}.sh
  echo "export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase" >> exe_${jobName}.sh
  echo "source \${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh" >> exe_${jobName}.sh
  echo "lsetup \"root 6.20.06-x86_64-centos7-gcc8-opt\"" >> exe_${jobName}.sh
  #echo "root -b -l -q getMag.cxx\(${init},${fin}\) | tee -a submit_${jobName}/log.fitStatus" >> exe_${jobName}.sh
  echo "rm -r /publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/shape/csv/${jobName}" >> exe_${jobName}.sh
  echo "./bin/getMag.exe ${init} ${fin} | tee -a submit_${jobName}/log.fitStatus" >> exe_${jobName}.sh

  chmod +x exe_${jobName}.sh

  echo "hep_sub exe_${jobName}.sh -g atlas -os CentOS7 -wt mid -mem 2048 -o submit_${jobName}/log-0.out -e submit_${jobName}/log-0.err" >> ${allJobs}

  if [ "$(ls submit_${jobName}/)" != "" ];then rm submit_${jobName}/*;fi
done
