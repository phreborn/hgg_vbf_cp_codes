#! /bin/bash

d_tilde=$(ls ../yield_sys/csv/ | grep b2 | grep -v SM | cut -d '_' -f 4)
bin=$(ls ../yield_sys/csv/ | grep m01 | cut -d '_' -f 5 | cut -d '.' -f 1)

echo $d_tilde
echo $bin

declare -A mapPdf=(["b1"]="Exp" ["b2"]="Exp" ["b3"]="ExpPoly2" ["b4"]="Exp" ["b5"]="Exp" ["b6"]="Exp")
echo ${!mapPdf[*]}

rundir=".."
ifpara="csv/para_bkg.csv"

for d in $d_tilde;do
#for d in m00;do
  for b in $bin;do
  
    out_xml="${rundir}/config/vbf_cp_${d}/model/background_OO_${b}.xml"
    > $out_xml
  
    paras=$(cat ${ifpara} | grep "${b},")
    B=`echo $paras | cut -d ',' -f 2`
    C=`echo $paras | cut -d ',' -f 3`
    yield=`echo $paras | cut -d ',' -f 4`
  
    echo "<!DOCTYPE Model SYSTEM 'AnaWSBuilder.dtd'>" >> $out_xml
    echo "<Model Type=\"UserDef\">" >> $out_xml

    pdfType=${mapPdf[$b]}
    echo "pdf for $b: $pdfType"

    if [ $pdfType = "ExpPoly2" ];then
      #echo "<ModelItem Name=\"EXPR::bkgPdf_${b}('exp(((@0/1000-125)/125)*@1+@2*((@0/1000-125)/125)*((@0/1000-125)/125))',:observable:,p1_${b}[0,-10,10],p2_${b}[0,-10,10])\"/></Model>" >> $out_xml
      echo "<ModelItem Name=\"EXPR::bkgPdf_${b}('exp(((@0/1000-125)/125)*@1+@2*((@0/1000-125)/125)*((@0/1000-125)/125))',:observable:,p1_${b}[${B},-10,10],p2_${b}[${C},-10,10])\"/></Model>" >> $out_xml
      #echo "<ModelItem Name=\"EXPR::bkgPdf_${b}('exp(((@0/1000-125)/125)*@1+@2*((@0/1000-125)/125)*((@0/1000-125)/125))',:observable:,p1_${b}[${B}],p2_${b}[${C}])\"/></Model>" >> $out_xml
    elif [ $pdfType = "Exp" ];then
      echo "<ModelItem Name=\"EXPR::bkgPdf_${b}('exp(((@0/1000-125)/125)*@1',:observable:,p1_${b}[${B},-10,10])\"/></Model>" >> $out_xml
    else
      echo Unrecognized PDF type
    fi
  done
done
