#! /bin/bash
#some bug in if in loop

basedir=/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/shape/

injectTest=0
injectPoint=p01

declare -A mapCtrType=(["RESOLUTION"]="logn" ["SCALE"]="gaus")
echo ${!mapCtrType[*]}

declare -A mapPara=(["RES"]="RESOLUTION" ["SCALE"]="SCALE")
echo ${!mapPara[*]}

nonShapeSys=("EG_RESOLUTION_AF2" "EG_RESOLUTION_ALL")

cats=$(cat ../../nom_WS/cats.cfg | grep -v "#" | grep ":" | cut -d ":" -f 1)
d_tilde=$(ls ${basedir}/../yield/csv/ | grep b3 | grep mag | grep -v SM | grep TT | cut -d '_' -f 4)
d_tilde=$(cat ../Dtilde | grep -v "#")

if [ $1 = "-d" ];then d_tilde=$2;fi

echo $d_tilde
echo $cats

#mcID="346214"
#ifsys="syst_yield_${mcID}.csv"
#ofsys="syst_${mcID}.xml"

fillSys(){

  para=$1
  inf_para=$2
  ofsys=$3
 
#  resp_Prod="<Item Name=\"prod::resp_${para}("
  resp_Prod_RW="<Item Name=\"prod::resp_${para}_RW("
  resp_Prod_Inj="<Item Name=\"prod::resp_${para}_${injectPoint}("
  
  if [ "${para}" == "SCALE" ];then
    echo "<Systematic Name=\"ATLAS_HIGGS_MASS\" Constr=\"gaus\" CentralValue=\"1\" Mag=\"0.0019\" WhereTo=\"shape\"/>" >> $ofsys
#    resp_Prod=${resp_Prod}"response::ATLAS_HIGGS_MASS,"
    resp_Prod_RW=${resp_Prod_RW}"response::ATLAS_HIGGS_MASS,"
    resp_Prod_Inj=${resp_Prod_Inj}"response::ATLAS_HIGGS_MASS,"
  fi

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

#    resp_Prod=${resp_Prod}"response::ATLAS_${sys_name},"
    resp_Prod_RW=${resp_Prod_RW}"response::ATLAS_${sys_name},"
    resp_Prod_Inj=${resp_Prod_Inj}"response::ATLAS_${sys_name},"
 
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
        tmag_u=$(echo ${mag_u} | sed 's/^-//g' | awk '{printf("%.8f",$0)}' | sed 's/0*$//g')
        tmag_d=$(echo ${mag_d} | sed 's/^-//g' | awk '{printf("%.8f",$0)}' | sed 's/0*$//g')
        tmag=
        # bc bug with scientific notation
        if [ `echo "${tmag_u} > ${tmag_d}" | bc` -eq 1 ];then
          tmag=${mag_u}
        else
          tmag=${mag_d}
        fi
        fmag=$(echo ${tmag} | sed 's/^-//g')
        echo "<Systematic Name=\"ATLAS_${sys_name}\" Constr=\"${ConstrType}\" CentralValue=\"1\" Mag=\"${fmag}\" WhereTo=\"shape\"/>" >> $ofsys
      else
        echo "<Systematic Name=\"ATLAS_${sys_name}\" Constr=\"${ConstrType}\" CentralValue=\"1\" Mag=\"${mag_u},${mag_d}\" WhereTo=\"shape\"/>" >> $ofsys
      fi
    elif [ $wNum -eq 2 -a -n $ConstrType -a $ConstrType != "asym" ]; then
      echo "<Systematic Name=\"ATLAS_${sys_name}\" Constr=\"${ConstrType}\" CentralValue=\"1\" Mag=\"${mag_u}\" WhereTo=\"shape\"/>" >> $ofsys
    else
      echo "UNMATCH constraint type and mag number, ${sys_name}"
      echo "need to add by hand"
      echo "to add by hand" >> $ofsys
      echo "<Systematic Name=\"ATLAS_${sys_name}\" Constr=\"\" CentralValue=\"1\" Mag=\",\" WhereTo=\"shape\"/>" >> $ofsys
    fi
  
  done

#  resp_Prod=${resp_Prod}")\"/>"
#  echo ${resp_Prod} >> $ofsys

  resp_Prod_RW=${resp_Prod_RW}")\"/>"
  echo ${resp_Prod_RW} >> $ofsys

  if [ ${injectTest} -eq 1 ];then
    resp_Prod_Inj=${resp_Prod_Inj}")\"/>"
    echo ${resp_Prod_Inj} >> $ofsys
  fi
}

for d in  $d_tilde ; do
  for cat in $cats ; do
#for d in  p01 ; do
#  for cat in TL_b2 ; do
    echo "====== ${d}_${cat} ======"
    ifcsv_mu="${basedir}/csv/mu_${d}_${cat}.csv";
    ifcsv_sigma="${basedir}/csv/sigma_${d}_${cat}.csv";
    ofxml="xml/shape_${d}_${cat}.xml"; #echo $ofxml
    > $ofxml
    fillSys RES $ifcsv_sigma $ofxml
    echo "" >> $ofxml
    fillSys SCALE $ifcsv_mu $ofxml
  done
done

#fillSys $mcID $ifsys $ofsys 
