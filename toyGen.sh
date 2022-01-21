#!/bin/bash

allJobs=jobsSub.sh
> ${allJobs}

for i in $(seq 0 49)
do
  jobName=Collect_${i}; echo ${jobName}
  if [ ! -d hepsub_${jobName} ]; then mkdir hepsub_${jobName}; fi
  executable=exe_${jobName}.sh
  > ${executable}

  echo "#!/bin/bash" >> ${executable}
  echo "" >> ${executable}
  echo "cd /scratchfs/atlas/huirun/atlaswork/VBF_CP/syst" >> ${executable}
  echo "source duplicToy.sh -i ${i}" >> ${executable}

  chmod +x ${executable}

  echo "hep_sub ${executable} -g atlas -os CentOS7 -wt mid -mem 2048 -o hepsub_${jobName}/log-0.out -e hepsub_${jobName}/log-0.err" >> ${allJobs}

  if [ "$(ls hepsub_${jobName}/)" != "" ];then rm hepsub_${jobName}/*;fi
done
