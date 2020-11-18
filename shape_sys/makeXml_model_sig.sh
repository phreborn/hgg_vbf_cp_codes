#! /bin/bash

d_tilde=$(ls ../yield_sys/csv/ | grep b2 | grep -v SM | cut -d '_' -f 4)
bin=$(ls ../yield_sys/csv/ | grep m01 | cut -d '_' -f 5 | cut -d '.' -f 1)

echo $d_tilde
echo $bin

rundir=".."
ifpara="csv/para_Nom.csv"

for d in $d_tilde;do
#for d in m00;do

  if [ ! -d ${rundir}/config/vbf_cp_${d}/model/ ];then
    mkdir -p ${rundir}/config/vbf_cp_${d}/model/
  fi

  for b in $bin;do

    out_xml="${rundir}/config/vbf_cp_${d}/model/signal_OO_${b}.xml"
    > $out_xml

    paras=$(cat ${ifpara} | grep "Nominal_${d}_${b},"); # b1 b10
    mu=`echo $paras | cut -d ',' -f 2`
    sigma=`echo $paras | cut -d ',' -f 3`
    alpLo=`echo $paras | cut -d ',' -f 4`
    nLo=`echo $paras | cut -d ',' -f 5`
    alpHi=`echo $paras | cut -d ',' -f 6`
    nHi=`echo $paras | cut -d ',' -f 7`

    echo "<!DOCTYPE Model SYSTEM 'AnaWSBuilder.dtd'>" >> $out_xml
    echo "<Model Type=\"UserDef\" CacheBinning=\"100000\">" >> $out_xml
    echo "<Item Name=\"muCBNom[$mu]\"/>" >> $out_xml
    echo "<Item Name=\"sigmaCBNom[$sigma]\"/>" >> $out_xml
    echo "<Item Name=\"alphaCBLo[$alpLo]\"/>" >> $out_xml
    echo "<Item Name=\"nCBLo[$nLo]\"/>" >> $out_xml
    echo "<Item Name=\"alphaCBHi[$alpHi]\"/>" >> $out_xml
    echo "<Item Name=\"nCBHi[$nHi]\"/>" >> $out_xml
    #echo "<Item Name=\"prod::muCB(muCBNom,One[1])\"/>" >> $out_xml
    #echo "<Item Name=\"prod::sigmaCB(sigmaCBNom,One)\"/>" >> $out_xml
    #echo "<Item Name=\"prod::muCB(muCBNom,resp_SCALE,response::ATLAS_LHCmass)\"/>" >> $out_xml
    echo "<Item Name=\"prod::muCB(muCBNom,resp_SCALE)\"/>" >> $out_xml
    echo "<Item Name=\"prod::sigmaCB(sigmaCBNom,resp_RES)\"/>" >> $out_xml
    echo "<ModelItem Name=\"RooTwoSidedCBShape::signal(:observable:, muCB, sigmaCB, alphaCBLo, nCBLo, alphaCBHi, nCBHi)\"/>" >> $out_xml
    echo "</Model>" >> $out_xml

  done
done
