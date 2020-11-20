#! /bin/bash

d_tilde=$(ls yield_sys/csv/ | grep b3 | grep -v SM | cut -d '_' -f 4)
bin=$(ls yield_sys/csv/ | grep m01 | cut -d '_' -f 5 | cut -d '.' -f 1)

echo $d_tilde
echo $bin

bkg_para="shape_sys/csv/para_bkg.csv"
sig_para="yield_sys/N_yield.csv"

cat ${sig_para}
cat ${bkg_para}

echo $bkg_para
echo $sig_para

if [ ! -d Input/data/ ];then
  mkdir -p Input/data/
fi

rundir="."
#preSys="ATLAS_PH_"
preSys="ATLAS"

for b in $bin;do
  y_ggH=$(cat ${sig_para} | grep "ggH_${b}," | cut -d ',' -f 2);echo yggH_${b}_${y_ggH}
  y_VBF_SM=$(cat ${sig_para} | grep "VBF_m00_${b}," | cut -d ',' -f 2);echo yVBF_SM_${y_VBF_SM}
  N_bkg=$(cat ${bkg_para} | grep "${b}," | cut -d ',' -f 4); echo nbkg_${b}_${N_bkg}

  sys_ggH=$(cat yield_sys/xml/sample_343981_SM_${b}.xml | sed 's/ /\?/g' | grep ${preSys})
  sys_VBF_SM=$(cat yield_sys/xml/sample_346214_m00_${b}.xml | sed 's/ /\?/g' | grep ${preSys})

  spurious=$(cat shape_sys/bkg_SS.csv | grep ${b} | cut -d , -f 3)

  for d in $d_tilde;do
  #for d in m00 p05;do
    out_xml="${rundir}/config/vbf_cp_${d}/channel/category_OO_${b}.xml"
    > $out_xml

    sys_shape=$(cat shape_sys/xml/shape_${d}_${b}.xml | sed 's/ /\?/g' | grep ${preSys})

    y_VBF_RW=$(cat ${sig_para} | grep "VBF_${d}_${b}," | cut -d ',' -f 2);echo yVBF_${d}_${y_VBF_RW}

    sys_VBF_RW=$(cat yield_sys/xml/sample_346214_${d}_${b}.xml | sed 's/ /\?/g' | grep ${preSys})

    #res_up=$(cat shape_sys/csv/sigma_${d}_${b}.csv | grep EG_RESOLUTION_ALL | cut -d ',' -f 2)
    #res_down=$(cat shape_sys/csv/sigma_${d}_${b}.csv | grep EG_RESOLUTION_ALL | cut -d ',' -f 3)
    #scl_up=$(cat shape_sys/csv/mu_${d}_${b}.csv | grep EG_SCALE_ALL | cut -d ',' -f 2)
    #scl_down=$(cat shape_sys/csv/mu_${d}_${b}.csv | grep EG_SCALE_ALL | cut -d ',' -f 3)

    echo "<!DOCTYPE Channel SYSTEM 'AnaWSBuilder.dtd'>" >> $out_xml
    echo "<Channel Name=\"OO_${b}\" Type=\"shape\" Lumi=\"1\">" >> $out_xml
    #echo "  <Data InputFile=\"Input/data/vbf_cp_${d}/tree_data_OO_${b}.root\" FileType=\"root\" TreeName=\"CollectionTree\" VarName=\"m_yy\" Observable=\"atlas_invMass_:category:[105000,160000]\" Binning=\"220\" InjectGhost=\"true\" BlindRange=\"120000,130000\"/>" >> $out_xml
    echo "  <Data InputFile=\"Input/data/tree_data_OO_${b}.root\" FileType=\"root\" TreeName=\"CollectionTree\" VarName=\"m_yy\" Observable=\"atlas_invMass_:category:[105000,160000]\" Binning=\"220\" InjectGhost=\"true\" BlindRange=\"120000,130000\"/>" >> $out_xml
    echo "" >> $out_xml

    echo "  <Systematic Name=\"ATLAS_lumi_run2\" Constr=\"logn\" CentralValue=\"1\" Mag=\"0.02\" WhereTo=\"yield\"/>" >> $out_xml
    echo "" >> $out_xml

    #echo "  <Systematic Name=\"ATLAS_EG_RESOLUTION_ALL\" Constr=\"asym\" CentralValue=\"1\" Mag=\"${res_up},${res_down}\" WhereTo=\"shape\"/>" >> $out_xml
    #echo "  <Item Name=\"prod::resp_RES(response::ATLAS_EG_RESOLUTION_ALL,one[1],)\"/>" >> $out_xml
    #echo "" >> $out_xml

    #echo "  <Systematic Name=\"ATLAS_EG_SCALE_ALL\" Constr=\"asym\" CentralValue=\"1\" Mag=\"${scl_up},${scl_down}\" WhereTo=\"shape\"/>" >> $out_xml
    #echo "  <Item Name=\"prod::resp_SCALE(response::ATLAS_EG_SCALE_ALL,one,)\"/>" >> $out_xml
    #echo "" >> $out_xml

    for sys in $sys_shape;do
      echo "  $(echo ${sys} | sed 's/\?/ /g')" >> $out_xml
    done
    echo "" >> $out_xml

    echo "  <Sample Name=\"VBF_SM\" InputFile=\"config/vbf_cp_m00/model/signal_:category:.xml\" ImportSyst=\":common:\" MultiplyLumi=\"true\" SharePdf=\"commonSig\">" >> $out_xml
    for sys in $sys_VBF_SM;do
      echo "    $(echo ${sys} | sed 's/\?/ /g')" >> $out_xml
    done
    echo "    <NormFactor Name=\"yield_VBF_SM[${y_VBF_SM}]\"/>" >> $out_xml
    echo "    <NormFactor Name=\"mu[1,0,5]\" />" >> $out_xml
    echo "    <NormFactor Name=\"mu_VBF_SM[1]\" />" >> $out_xml
    echo "  </Sample>" >> $out_xml
    echo "" >> $out_xml

    echo "  <Sample Name=\"VBF_RW\" InputFile=\"config/vbf_cp_${d}/model/signal_:category:.xml\" ImportSyst=\":common:\" MultiplyLumi=\"true\" SharePdf=\"commonSig\">" >> $out_xml
    for sys in $sys_VBF_RW;do
      echo "    $(echo ${sys} | sed 's/\?/ /g')" >> $out_xml
    done
    echo "    <NormFactor Name=\"yield_VBF_RW[${y_VBF_RW}]\"/>" >> $out_xml
    echo "    <NormFactor Name=\"mu[1,0,5]\" />" >> $out_xml
    echo "    <NormFactor Name=\"mu_VBF_RW[1]\" />" >> $out_xml
    echo "  </Sample>" >> $out_xml
    echo "" >> $out_xml

    echo "  <Sample Name=\"ggH\" InputFile=\"config/vbf_cp_${d}/model/signal_:category:.xml\" ImportSyst=\":common:\" MultiplyLumi=\"true\" SharePdf=\"commonSig\">" >> $out_xml
    for sys in $sys_ggH;do
      echo "    $(echo ${sys} | sed 's/\?/ /g')" >> $out_xml
    done
    echo "    <NormFactor Name=\"yield_ggH[${y_ggH}]\"/>" >> $out_xml
    echo "    <NormFactor Name=\"mu[1,0,5]\" />" >> $out_xml
    echo "    <NormFactor Name=\"mu_ggH[1]\" />" >> $out_xml # can be used for turn on/off a process
    echo "  </Sample>" >> $out_xml
    echo "" >> $out_xml

    echo "  <Sample Name=\"spurious\" InputFile=\"config/vbf_cp_${d}/model/signal_:category:.xml\" ImportSyst=\":self:\" MultiplyLumi=\"false\" SharePdf=\"commonSig\">" >> $out_xml
    echo "    <Systematic Name=\"ATLAS_Hgg_BIAS_:category:\" Constr=\"gaus\" CentralValue=\"0\" Mag=\"${spurious}\" WhereTo=\"yield\"/>" >> $out_xml
    echo "  </Sample>" >> $out_xml
    echo "" >> $out_xml

    echo "  <Sample Name=\"background\" InputFile=\"config/vbf_cp_${d}/model/background_:category:.xml\" ImportSyst=\":self:\" MultiplyLumi=\"false\">" >> $out_xml
    echo "    <NormFactor Name=\"nbkg_:category:[${N_bkg},0,100000]\"/>" >> $out_xml
    #echo "    <NormFactor Name=\"nbkg_:category:[${N_bkg}]\"/>" >> $out_xml
    echo "  </Sample>" >> $out_xml
    echo "</Channel>" >> $out_xml
  done
done
