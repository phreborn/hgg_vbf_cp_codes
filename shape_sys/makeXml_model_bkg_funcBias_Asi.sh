#! /bin/bash

#cats=$(ls /scratchfs/atlas/chenhr/atlaswork/VBF_CP/calcBDT/outputs/mc16a/ | grep  343981_ggF_Nominal | cut -d _ -f 4 | cut -d . -f 1)
cats=$(cat ../../nom_WS/cats.cfg | grep -v "#" | grep ":" | cut -d ":" -f 1)

d_tilde=$(ls ../yield_sys/csv/ | grep TT_b2 | grep -v SM | cut -d '_' -f 4)
d_tilde=$(cat ../Dtilde | grep -v "#")
bin=$(ls ../yield_sys/csv/ | grep m01 | cut -d '_' -f 5 | cut -d '.' -f 1)

echo $d_tilde
echo $bin

echo $cats

#declare -A mapPdf=(["b1"]="ExpPoly2" ["b2"]="ExpPoly2" ["b3"]="ExpPoly2" ["b4"]="ExpPoly2" ["b5"]="ExpPoly2" ["b6"]="ExpPoly2") # need to extend to ["cat_bin"]="type"
#echo ${!mapPdf[*]}

rundir=".."
ifpara="csv/para_bkg.csv"

for d in $d_tilde;do
#for d in m00;do

  if [ ! -d ${rundir}/config/vbf_cp_${d}/model/ ];then
    mkdir -p ${rundir}/config/vbf_cp_${d}/model/
  fi

  for cat in $cats;do
  
    out_xml="${rundir}/config/vbf_cp_${d}/model/background_OO_${cat}_funcBias_Asi.xml"
    > $out_xml
  
    paras=$(cat ${ifpara} | grep -v "#" | grep "${cat},")
    B=`echo $paras | cut -d ',' -f 2`
    C=`echo $paras | cut -d ',' -f 3`
    POWA=`echo $paras | cut -d ',' -f 4`
    yield=`echo $paras | cut -d ',' -f 5`
  
    echo "<!DOCTYPE Model SYSTEM 'AnaWSBuilder.dtd'>" >> $out_xml
    echo "<Model Type=\"UserDef\">" >> $out_xml

    #pdfType=${mapPdf[$b]}
    pdfType=$(cat funcBkg | grep ${cat} | cut -d ":" -f 2)
    echo "pdf for ${cat}: $pdfType"

    if [[ "${cat}" =~ "TT" || "${cat}" =~ "TL" ]];then
      echo "<ModelItem Name=\"EXPR::bkgPdf_${cat}_bfb('exp(((@0/1000-125)/125)*@1+@2*((@0/1000-125)/125)*((@0/1000-125)/125))',:observable:,p1_${cat}_bfb[0,-10,10],p2_${cat}_bfb[0,-10,10])\"/></Model>" >> $out_xml
    else
      if [ $pdfType = "ExpPoly2" ];then
        #echo "<ModelItem Name=\"EXPR::bkgPdf_${b}('exp(((@0/1000-125)/125)*@1+@2*((@0/1000-125)/125)*((@0/1000-125)/125))',:observable:,p1_${b}[0,-10,10],p2_${b}[0,-10,10])\"/></Model>" >> $out_xml
        echo "<ModelItem Name=\"EXPR::bkgPdf_${cat}_bfb('exp(((@0/1000-125)/125)*@1+@2*((@0/1000-125)/125)*((@0/1000-125)/125))',:observable:,p1_${cat}_bfb[${B},-10,10],p2_${cat}_bfb[${C},-10,10])\"/></Model>" >> $out_xml
        #echo "<ModelItem Name=\"EXPR::bkgPdf_${b}('exp(((@0/1000-125)/125)*@1+@2*((@0/1000-125)/125)*((@0/1000-125)/125))',:observable:,p1_${b}[${B}],p2_${b}[${C}])\"/></Model>" >> $out_xml
      elif [ $pdfType = "Exp" ];then
        echo "<ModelItem Name=\"EXPR::bkgPdf_${cat}_bfb('exp(((@0/1000-125)/125)*@1)',:observable:,p1_${cat}_bfb[${B},-10,10])\"/></Model>" >> $out_xml
      elif [ $pdfType = "Pow" ];then
        echo "<ModelItem Name=\"EXPR::bkgPdf_${cat}_bfb('pow((@0/1000/125),@1)',:observable:,p1_${cat}_bfb[${POWA},-100,100])\"/></Model>" >> $out_xml
      else
        echo Unrecognized PDF type
      fi
    fi
  done
done
