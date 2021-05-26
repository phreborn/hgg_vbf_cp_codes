tag=ggF

allJobs=jobsSub.sh
> ${allJobs}

d_tilde=$(cat Dtilde | grep -v "#")

for d in ${d_tilde};do
  jobName=Collect_${d}; echo ${jobName}
  #if [ ! -d csv/${jobName} ];then mkdir -p csv/${jobName};fi
  if [ ! -d submit_${jobName} ]; then mkdir submit_${jobName}; fi
  executable=exe_${jobName}.sh
  > ${executable}

  echo "#!/bin/bash" >> exe_${jobName}.sh
  echo "" >> exe_${jobName}.sh
  echo "cd /scratchfs/atlas/huirun/atlaswork/VBF_CP/syst" >> exe_${jobName}.sh
  echo "source makeXml_cat.sh -d ${d}" >> exe_${jobName}.sh
  echo "sleep 2" >> exe_${jobName}.sh

  chmod +x exe_${jobName}.sh

  echo "hep_sub exe_${jobName}.sh -g atlas -os CentOS7 -wt mid -mem 2048 -o submit_${jobName}/log-0.out -e submit_${jobName}/log-0.err" >> ${allJobs}

  if [ "$(ls submit_${jobName}/)" != "" ];then rm submit_${jobName}/*;fi
done
