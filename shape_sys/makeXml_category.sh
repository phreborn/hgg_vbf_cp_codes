#! /bin/bash
#some bug in if in loop

declare -A mapCtrType=(["RESOLUTION"]="logn" ["SCALE"]="gaus")
echo ${!mapCtrType[*]}

declare -A mapPara=(["RES"]="RESOLUTION" ["SCALE"]="SCALE")
echo ${!mapPara[*]}

nonShapeSys=("EG_RESOLUTION_AF2" "EG_RESOLUTION_ALL")

d_tilde=$(ls ../yield_sys/csv/ | grep b3 | grep -v SM | cut -d '_' -f 4)
bin=$(ls ../yield_sys/csv/ | grep m01 | cut -d '_' -f 5 | cut -d '.' -f 1)

echo $d_tilde
echo $bin

#mcID="346214"
#ifsys="syst_yield_${mcID}.csv"
#ofsys="syst_${mcID}.xml"

fillSys(){

  para=$1
  inf_para=$2
  ofsys=$3
 
  resp_Prod="<Item Name=\"prod::resp_${para}("
  
  lines=`cat $inf_para`
  for line in $lines
  do
  
    sys_name=`echo $line | cut -d ',' -f 1`
  
    if [[ ! ($sys_name =~ "${mapPara[${para}]}") ]];then
      continue
    fi
  
    if [[ ${nonShapeSys[@]/${sys_name}/} != ${nonShapeSys[@]} ]];then #list substitution
      continue
    fi

    resp_Prod=${resp_Prod}"response::ATLAS_${sys_name},"
 
    mag_u=$(echo $line | cut -d ',' -f 2)
    mag_d=$(echo $line | cut -d ',' -f 3)
  
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
      echo "<Systematic Name=\"ATLAS_${sys_name}\" Constr=\"${ConstrType}\" CentralValue=\"1\" Mag=\"${mag_u},${mag_d}\" WhereTo=\"shape\"/>" >> $ofsys
    elif [ $wNum -eq 2 -a -n $ConstrType -a $ConstrType != "asym" ]; then
      echo "<Systematic Name=\"ATLAS_${sys_name}\" Constr=\"${ConstrType}\" CentralValue=\"1\" Mag=\"${mag_u}\" WhereTo=\"shape\"/>" >> $ofsys
    else
      echo "UNMATCH constraint type and mag number, ${sys_name}"
      echo "need to add by hand"
      echo "to add by hand" >> $ofsys
      echo "<Systematic Name=\"ATLAS_${sys_name}\" Constr=\"\" CentralValue=\"1\" Mag=\",\" WhereTo=\"shape\"/>" >> $ofsys
    fi
  
  done

  resp_Prod=${resp_Prod}")\"/>"
  echo ${resp_Prod} >> $ofsys
}


for d in  $d_tilde ; do
  for b in $bin ; do
    echo "======d value ${d}, OO in ${b}======"
    ifcsv_mu="csv/mu_${d}_${b}.csv";
    ifcsv_sigma="csv/sigma_${d}_${b}.csv";
    ofxml="xml/shape_${d}_${b}.xml"; #echo $ofxml
    > $ofxml
    fillSys RES $ifcsv_sigma $ofxml
    echo "" >> $ofsys
    fillSys SCALE $ifcsv_mu $ofxml
  done
done

#fillSys $mcID $ifsys $ofsys 
