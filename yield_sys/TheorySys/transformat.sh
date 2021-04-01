#! /bin/bash

#mv VBF_theorySys_d2.63e-08.txt VBF_theorySys_d0.00.txt
#mv ggF_theorySys_d2.63e-08.txt ggF_theorySys_d0.00.txt

#declare -A mapD_tilde=(["d2.63e-08"]="m00" ["d-0.01"]="m01" ["d-0.02"]="m02" ["d-0.03"]="m03" ["d-0.04"]="m04") 
declare -A mapID=(["ggF"]="343981" ["VBF"]="346214")
echo ${!mapD_tilde[*]}
echo ${!mapID[*]}

transfer(){
  inf=$1
  sampleID=$2
  d_tilde=$3

  > ./mag_theory_${sampleID}_${d_tilde}_b1.csv
  > ./mag_theory_${sampleID}_${d_tilde}_b2.csv
  > ./mag_theory_${sampleID}_${d_tilde}_b3.csv
  > ./mag_theory_${sampleID}_${d_tilde}_b4.csv
  > ./mag_theory_${sampleID}_${d_tilde}_b5.csv
  > ./mag_theory_${sampleID}_${d_tilde}_b6.csv

  lines=$(cat $inf | sed 's/ //g'); #echo $lines

  for l in $lines
  do

    sysname=$(echo $l | cut -d , -f 1)

    b1=$(echo $l | cut -d , -f 2)
    b2=$(echo $l | cut -d , -f 3)
    b3=$(echo $l | cut -d , -f 4)
    b4=$(echo $l | cut -d , -f 5)
    b5=$(echo $l | cut -d , -f 6)
    b6=$(echo $l | cut -d , -f 7)

    echo "${sysname},${b1}" >> mag_theory_${sampleID}_${d_tilde}_b1.csv 
    echo "${sysname},${b2}" >> mag_theory_${sampleID}_${d_tilde}_b2.csv 
    echo "${sysname},${b3}" >> mag_theory_${sampleID}_${d_tilde}_b3.csv 
    echo "${sysname},${b4}" >> mag_theory_${sampleID}_${d_tilde}_b4.csv 
    echo "${sysname},${b5}" >> mag_theory_${sampleID}_${d_tilde}_b5.csv 
    echo "${sysname},${b6}" >> mag_theory_${sampleID}_${d_tilde}_b6.csv 
  done
}

infiles=`ls /scratchfs/atlas/chenhr/atlaswork/VBF_CP/syst/yield_sys/TheorySys | grep txt`

for inf in $infiles
do

  if [[ ! ($inf =~ ".txt") ]];then
    continue
  fi

  if [[ $inf =~ "ggF" ]] && [[ ! ($inf =~ "d0.00") ]];then
    continue
  fi

  d_tilde=""

  #for d in "${!mapD_tilde[@]}";do
  #  if [[ $inf =~ $d ]];then
  #    d_tilde=${mapD_tilde[$d]}
  #  fi
  #done

  d_tilde=$(echo $inf | cut -d . -f 2)
  if [ ${#d_tilde} -eq 1 ];then
    d_tilde="${d_tilde}0"
  elif [ ${#d_tilde} -ne 2 ];then
    echo "warning length of d_tilde"
  fi

  if [[ $inf =~ "d-0" ]];then
    d_tilde="m${d_tilde}"
  elif [[ $inf =~ "d0" ]];then
    d_tilde="p${d_tilde}"
  fi

  if [[ $inf =~ "ggF" ]]; then
    d_tilde="SM"
  fi

  echo $inf
  echo $d_tilde

  sampleID=""
  for id in "${!mapID[@]}";do
    if [[ $inf =~ $id ]];then
      sampleID=${mapID[$id]}
    fi
  done

  echo "${sampleID}_${d_tilde}"

  transfer $inf $sampleID $d_tilde

done

#transfer VBF_theorySys_d-0.00.txt 346214 m00
