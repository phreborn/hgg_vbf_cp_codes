tag=ggF

allJobs=jobsSub_makeCat.sh
> ${allJobs}

d_tilde=$(cat ../Dtilde | grep -v "#")

for d in ${d_tilde};do
  jobName=Collect_makeCat_${d}; echo ${jobName}
  condorDir=hepsub_${jobName}
  #if [ ! -d csv/${jobName} ];then mkdir -p csv/${jobName};fi
  if [ ! -d ${condorDir} ]; then mkdir ${condorDir}; fi
  executable=exe_${jobName}.sh
  > ${executable}

  echo "#!/bin/bash" >> exe_${jobName}.sh
  echo "" >> exe_${jobName}.sh
  echo "cd /scratchfs/atlas/huirun/atlaswork/VBF_CP/syst/yield_sys" >> exe_${jobName}.sh
  echo "source makeXml_category.sh -d ${d}" >> exe_${jobName}.sh
  echo "sleep 2" >> exe_${jobName}.sh

  chmod +x exe_${jobName}.sh

  echo "hep_sub exe_${jobName}.sh -g atlas -os CentOS7 -wt mid -mem 2048 -o ${condorDir}/log-0.out -e ${condorDir}/log-0.err" >> ${allJobs}

  if [ "$(ls ${condorDir}/)" != "" ];then rm ${condorDir}/*;fi
done
