#! /bin/bash

basepath=/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/shape/

#cats=$(ls /scratchfs/atlas/chenhr/atlaswork/VBF_CP/calcBDT/outputs/mc16a/ | grep  343981_ggF_Nominal | cut -d _ -f 4 | cut -d . -f 1)
cats=$(cat ../../nom_WS/cats.cfg | grep -v "#" | grep ":" | cut -d ":" -f 1)

d_tilde=$(ls ${basepath}/../yield/csv/ | grep TT_b2 | grep -v SM | cut -d '_' -f 4)
d_tilde=$(cat ../Dtilde | grep -v "#")
bin=$(ls ${basepath}/../yield/csv/ | grep m01 | cut -d '_' -f 5 | cut -d '.' -f 1)

echo $d_tilde
echo $bin

echo $cats

rundir=".."
ifpara="${basepath}/csv/para_Nom.csv"

for d in $d_tilde;do
#for d in m00;do

  if [ ! -d ${rundir}/config/vbf_cp_${d}/model/ ];then
    mkdir -p ${rundir}/config/vbf_cp_${d}/model/
  fi

for cat in $cats;do
    echo ${d}_${cat}

    out_xml="${rundir}/config/vbf_cp_${d}/model/signal_OO_${cat}.xml"
    > $out_xml

    paras=$(cat ${ifpara} | grep "Nominal_${d}_${cat},"); # b1 b10
    mu=`echo $paras | cut -d ',' -f 2`
    sigma=`echo $paras | cut -d ',' -f 3`
    alpLo=`echo $paras | cut -d ',' -f 4`
    nLo=`echo $paras | cut -d ',' -f 5`
    alpHi=`echo $paras | cut -d ',' -f 6`
    nHi=`echo $paras | cut -d ',' -f 7`

    echo "<!DOCTYPE Model SYSTEM 'AnaWSBuilder.dtd'>" >> $out_xml
    echo "<Model Type=\"UserDef\" CacheBinning=\"100000\">" >> $out_xml
    echo "<Item Name=\"muCBNom_RW[$mu]\"/>" >> $out_xml
    echo "<Item Name=\"sigmaCBNom_RW[$sigma]\"/>" >> $out_xml
    echo "<Item Name=\"alphaCBLo_RW[$alpLo]\"/>" >> $out_xml
    echo "<Item Name=\"nCBLo_RW[$nLo]\"/>" >> $out_xml
    echo "<Item Name=\"alphaCBHi_RW[$alpHi]\"/>" >> $out_xml
    echo "<Item Name=\"nCBHi_RW[$nHi]\"/>" >> $out_xml
    #echo "<Item Name=\"prod::muCB(muCBNom,One[1])\"/>" >> $out_xml
    #echo "<Item Name=\"prod::sigmaCB(sigmaCBNom,One)\"/>" >> $out_xml
    #echo "<Item Name=\"prod::muCB(muCBNom,resp_SCALE,response::ATLAS_LHCmass)\"/>" >> $out_xml
    echo "<Item Name=\"prod::muCB_RW(muCBNom_RW,resp_SCALE_RW)\"/>" >> $out_xml
    echo "<Item Name=\"prod::sigmaCB_RW(sigmaCBNom_RW,resp_RES_RW)\"/>" >> $out_xml
    echo "<ModelItem Name=\"RooTwoSidedCBShape::signal_RW(:observable:, muCB_RW, sigmaCB_RW, alphaCBLo_RW, nCBLo_RW, alphaCBHi_RW, nCBHi_RW)\"/>" >> $out_xml
    echo "</Model>" >> $out_xml

done
done
