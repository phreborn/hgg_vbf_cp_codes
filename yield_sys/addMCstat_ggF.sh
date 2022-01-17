#! /bin/bash

inpath=csv/
outpath=csv/

transfer(){
  inf=$1
  sampleID=$2
  d_tilde=$3
  bdtCat=$4

  sampleName=ggF

  sed -i '/mcstat_ggF_/d' ${outpath}/mag_yield_${sampleID}_${d_tilde}_${bdtCat}_b1.csv
  sed -i '/mcstat_ggF_/d' ${outpath}/mag_yield_${sampleID}_${d_tilde}_${bdtCat}_b2.csv
  sed -i '/mcstat_ggF_/d' ${outpath}/mag_yield_${sampleID}_${d_tilde}_${bdtCat}_b3.csv
  sed -i '/mcstat_ggF_/d' ${outpath}/mag_yield_${sampleID}_${d_tilde}_${bdtCat}_b4.csv
  sed -i '/mcstat_ggF_/d' ${outpath}/mag_yield_${sampleID}_${d_tilde}_${bdtCat}_b5.csv
  sed -i '/mcstat_ggF_/d' ${outpath}/mag_yield_${sampleID}_${d_tilde}_${bdtCat}_b6.csv

  b1=$(cat ${inpath}/$inf | grep ${bdtCat}_b1 | sed 's/ //g' | cut -d , -f 2)
  b2=$(cat ${inpath}/$inf | grep ${bdtCat}_b2 | sed 's/ //g' | cut -d , -f 2)
  b3=$(cat ${inpath}/$inf | grep ${bdtCat}_b3 | sed 's/ //g' | cut -d , -f 2)
  b4=$(cat ${inpath}/$inf | grep ${bdtCat}_b4 | sed 's/ //g' | cut -d , -f 2)
  b5=$(cat ${inpath}/$inf | grep ${bdtCat}_b5 | sed 's/ //g' | cut -d , -f 2)
  b6=$(cat ${inpath}/$inf | grep ${bdtCat}_b6 | sed 's/ //g' | cut -d , -f 2)

  echo "mcstat_${sampleName},${b1},${b1}" >> ${outpath}/mag_yield_${sampleID}_${d_tilde}_${bdtCat}_b1.csv
  echo "mcstat_${sampleName},${b2},${b2}" >> ${outpath}/mag_yield_${sampleID}_${d_tilde}_${bdtCat}_b2.csv
  echo "mcstat_${sampleName},${b3},${b3}" >> ${outpath}/mag_yield_${sampleID}_${d_tilde}_${bdtCat}_b3.csv
  echo "mcstat_${sampleName},${b4},${b4}" >> ${outpath}/mag_yield_${sampleID}_${d_tilde}_${bdtCat}_b4.csv
  echo "mcstat_${sampleName},${b5},${b5}" >> ${outpath}/mag_yield_${sampleID}_${d_tilde}_${bdtCat}_b5.csv
  echo "mcstat_${sampleName},${b6},${b6}" >> ${outpath}/mag_yield_${sampleID}_${d_tilde}_${bdtCat}_b6.csv
}

ds=$(ls ${inpath}/mag_yield_346214_*_TT_b1.csv | cut -d _ -f 4)
cats=$(ls TheorySys/mag_theory_346214_m00_*_b1.csv | cut -d _ -f 5)

for c in TT TL LT;do
  bdtCat=${c}

  #ggF
  inf=ggF_MCstat.csv
  sampleID=343981
  for d in SM;do
    d_tilde=${d}
    echo "$sampleID $d_tilde ${bdtCat}"
    transfer $inf $sampleID $d_tilde ${bdtCat}
  done
done
