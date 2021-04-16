#! /bin/bash

inpath=/scratchfs/atlas/chenhr/atlaswork/VBF_CP/syst/yield_sys/TheorySys/showerSys

transfer(){
  inf=$1
  sampleID=$2
  d_tilde=$3
  bdtCat=$4

  sampleName=$(echo ${inf} | cut -d _ -f 1)

  sed -i '/shower_/d' ./mag_theory_${sampleID}_${d_tilde}_${bdtCat}_b1.csv
  sed -i '/shower_/d' ./mag_theory_${sampleID}_${d_tilde}_${bdtCat}_b2.csv
  sed -i '/shower_/d' ./mag_theory_${sampleID}_${d_tilde}_${bdtCat}_b3.csv
  sed -i '/shower_/d' ./mag_theory_${sampleID}_${d_tilde}_${bdtCat}_b4.csv
  sed -i '/shower_/d' ./mag_theory_${sampleID}_${d_tilde}_${bdtCat}_b5.csv
  sed -i '/shower_/d' ./mag_theory_${sampleID}_${d_tilde}_${bdtCat}_b6.csv

  lines=$(cat ${inpath}/$inf | sed 's/ //g'); #echo $lines

  for l in $lines
  do

    sysname=$(echo $l | cut -d , -f 1)

    b1=$(echo $l | cut -d , -f 2)
    b2=$(echo $l | cut -d , -f 3)
    b3=$(echo $l | cut -d , -f 4)
    b4=$(echo $l | cut -d , -f 5)
    b5=$(echo $l | cut -d , -f 6)
    b6=$(echo $l | cut -d , -f 7)

    echo "${sysname}_${sampleName},${b1}" >> mag_theory_${sampleID}_${d_tilde}_${bdtCat}_b1.csv
    echo "${sysname}_${sampleName},${b2}" >> mag_theory_${sampleID}_${d_tilde}_${bdtCat}_b2.csv
    echo "${sysname}_${sampleName},${b3}" >> mag_theory_${sampleID}_${d_tilde}_${bdtCat}_b3.csv
    echo "${sysname}_${sampleName},${b4}" >> mag_theory_${sampleID}_${d_tilde}_${bdtCat}_b4.csv
    echo "${sysname}_${sampleName},${b5}" >> mag_theory_${sampleID}_${d_tilde}_${bdtCat}_b5.csv
    echo "${sysname}_${sampleName},${b6}" >> mag_theory_${sampleID}_${d_tilde}_${bdtCat}_b6.csv
  done
}

ds=$(ls mag_theory_346214_*_TT_b1.csv | cut -d _ -f 4)
cats=$(ls mag_theory_346214_m00_*_b1.csv | cut -d _ -f 5)

for c in ${cats};do
  bdtCat=${c}

  #ggF
  inf=ggF_showerSys_${c}_d0.txt
  sampleID=343981
  d_tilde=SM
  echo "$sampleID $d_tilde ${bdtCat}"
  transfer $inf $sampleID $d_tilde ${bdtCat}

  #VBF
  inf=VBF_showerSys_${c}_d0.txt
  sampleID=346214
  for d in ${ds};do
    d_tilde=${d}
    echo "$sampleID $d_tilde ${bdtCat}"
    transfer $inf $sampleID $d_tilde ${bdtCat}
  done
done
