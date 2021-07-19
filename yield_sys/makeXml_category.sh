#! /bin/bash

declare -A mapCtrType=(["EG"]="logn" ["PH"]="logn" ["FT"]="asym" ["JET"]="asym" ["MET"]="asym" ["PRW"]="asym" ["pdf"]="logn" ["qcd"]="logn" ["aS"]="logn" ["shower"]="gaus")
echo ${!mapCtrType[*]}

cats=$(cat ../../nom_WS/cats.cfg | grep -v "#" | grep ":" | cut -d ":" -f 1)
d_tilde=$(ls csv/ | grep b3 | grep -v SM | grep TT | cut -d '_' -f 4)
d_tilde=$(cat ../Dtilde | grep -v "#")
d_tilde=$(cat ../cHW_fine | grep -v "#")

echo $d_tilde
echo $cats

#mcID="346214"
#ifsys="syst_yield_${mcID}.csv"
#ofsys="syst_${mcID}.xml"

fillSys(){

mcID=$1
ifsys=$2
ofsys=$3

> $ofsys

lines=`cat $ifsys`
for line in $lines
do

  sys_name=`echo $line | cut -d ',' -f 1`
  mag_u=$(echo $line | cut -d ',' -f 2)
  mag_d=$(echo $line | cut -d ',' -f 3)

  echo $sys_name

  wNum=$(echo $line | sed 's/,/ /g' | wc -w)

  ConstrType=""
  for key in "${!mapCtrType[@]}";do
    if [[ $sys_name =~ $key ]];then
      ConstrType=${mapCtrType[$key]}
    fi
  done

  if [ -z $ConstrType ];then
    echo "no match found in constraint type mapping, ${sys_name}"
  fi

  #echo "${sys_name}, ${ConstrType}"

  if [ $wNum -eq 3 ];then
    if [ -n $ConstrType -a $ConstrType != "asym" ];then
      echo "UNMATCH constraint type and mag number, ${sys_name}"
      echo "origin type ${ConstrType}, force asym"
      ConstrType="asym"
    fi
    echo "<Systematic Name=\"ATLAS_${sys_name}\" Constr=\"${ConstrType}\" CentralValue=\"1\" Mag=\"${mag_u},${mag_d}\" WhereTo=\"yield\"/>" >> $ofsys
  elif [ $wNum -eq 2 -a -n $ConstrType -a $ConstrType != "asym" ]; then
    echo "<Systematic Name=\"ATLAS_${sys_name}\" Constr=\"${ConstrType}\" CentralValue=\"1\" Mag=\"${mag_u}\" WhereTo=\"yield\"/>" >> $ofsys
  else
    echo "UNMATCH constraint type and mag number, ${sys_name}"
    echo "need to add by hand"
    echo "to add by hand" >> $ofsys
    echo "<Systematic Name=\"ATLAS_${sys_name}\" Constr=\"\" CentralValue=\"1\" Mag=\",\" WhereTo=\"yield\"/>" >> $ofsys
  fi

done
}

basepath=/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/yield/

for id in 346214 ; do
  for d in  $d_tilde ; do
    for cat in $cats ; do
      echo ""
      echo ======= ${d}_${cat} =======
      echo ""
      ifcsv_theo="TheorySys/mag_theory_${id}_m00_${cat}.csv";
      ifcsv_exp="${basepath}/csv/mag_yield_${id}_${d}_${cat}.csv";
      ifcsv_exp_jd="${basepath}/csv_jd/mag_yield_${id}_${d}_${cat}.csv";
      ifcsv_tmp="csv/tmp_${id}_${d}_${cat}.csv"
      > $ifcsv_tmp
      cat $ifcsv_exp | grep -v "JER_EffectiveNP" >> $ifcsv_tmp
      cat $ifcsv_exp_jd | grep "JER_EffectiveNP" >> $ifcsv_tmp
      cat $ifcsv_theo >> $ifcsv_tmp
      ofxml="xml/sample_${id}_${d}_${cat}.xml"; #echo $ofxml
      fillSys $id $ifcsv_tmp $ofxml
      rm $ifcsv_tmp
    done
  done
done

for id in 343981; do
  for cat in $cats ; do
    echo ""
    echo ======= ${id}_${cat} =======
    echo ""
    ifcsv_theo="TheorySys/mag_theory_${id}_SM_${cat}.csv";
    ifcsv_exp="${basepath}/csv/mag_yield_${id}_SM_${cat}.csv";
    ifcsv_exp_jd="${basepath}/csv_jd/mag_yield_${id}_SM_${cat}.csv";
    ifcsv_tmp="csv/tmp_${id}_SM_${cat}.csv"
    > $ifcsv_tmp
    cat $ifcsv_exp | grep -v "JER_EffectiveNP" >> $ifcsv_tmp
    cat $ifcsv_exp_jd | grep "JER_EffectiveNP" >> $ifcsv_tmp
    cat $ifcsv_theo >> $ifcsv_tmp
    ofxml="xml/sample_${id}_SM_${cat}.xml";
    fillSys $id $ifcsv_tmp $ofxml
    rm $ifcsv_tmp
  done
done

#fillSys $mcID $ifsys $ofsys 
