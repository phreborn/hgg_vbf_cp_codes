#!/bin/bash

#cHWs=$(cat ../cHW_fine | grep -v "#")
cHWs=$(ls /publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/theory/kfactor_SMEFT_fine/LL_PH_EFF_ISO_Uncertainty_cHW_*txt | cut -d _ -f 9 | sed 's/\.txt//g')
cHWs=$(ls /publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/theory/kfactor_SMEFT_fine_updatedBDT/LL_PH_EFF_ISO_Uncertainty_cHW_*txt | cut -d _ -f 10 | sed 's/\.txt//g')

fcHW="../cHW_fine"
> ${fcHW}
for cHW in ${cHWs};do
  scHW=${cHW}
  cHW="p${cHW}"
  cHW=${cHW/p-/m}
  cHW=${cHW/./d}
  decimal=${cHW#*d}
  if [[ "${cHW}" =~ "e" ]];then cHW="m0d00"
  elif [ ${#cHW} -eq 2 ];then cHW="${cHW}d00"
  elif [ ${#decimal} -eq 1 ]; then cHW="${cHW}0"
  fi
  echo "cHWs[\"${cHW}\"] = \"${scHW}\";"
  echo ${cHW} >> ${fcHW}
done
