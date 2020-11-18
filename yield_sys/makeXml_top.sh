#! /bin/bash

d_tilde=$(ls csv/ | grep b3 | grep -v SM | cut -d '_' -f 4)
bin=$(ls csv/ | grep m01 | cut -d '_' -f 5 | cut -d '.' -f 1)

echo $d_tilde
echo $bin

rundir=".."

for d in $d_tilde;do
#for d in m00;do

  if [ ! -d ${rundir}/config/vbf_cp_${d}/channel/ ];then
    mkdir -p ${rundir}/config/vbf_cp_${d}/channel/
  fi
  if [ ! -d ${rundir}/workspace/vbf_cp_${d}/ ];then
    mkdir -p ${rundir}/workspace/vbf_cp_${d}/
  fi
  out_xml="${rundir}/config/vbf_cp_${d}/d_tilde_${d}.xml"
  > $out_xml
  echo "<!DOCTYPE Combination  SYSTEM 'AnaWSBuilder.dtd'>" >> $out_xml
  echo "<Combination WorkspaceName=\"combWS\" ModelConfigName=\"ModelConfig\" DataName=\"combData\" OutputFile=\"workspace/vbf_cp_${d}/vbf_cp_${d}.root\" Blind=\"true\">" >> $out_xml
  echo "" >> $out_xml

  for b in $bin;do
    echo "  <Input>config/vbf_cp_${d}/channel/category_OO_${b}.xml</Input>" >> $out_xml
  done

  echo "  <POI>mu_VBF_SM,mu_VBF_RW,mu</POI>" >> $out_xml
  echo "" >> $out_xml

  #echo "  <Asimov Name=\"asimovData_SM_noFit\"   Setup=\"mu_VBF_RW=0,mu_VBF_SM=1,mu=1\"     Action=\"raw:fixall:genasimov:float:savesnapshot\" SnapshotNuis=\"nominalNuis_SM_noFit\" SnapshotGlob=\"nominalGlob_SM_noFit\"/>" >> $out_xml # PE b, c
  echo "  <Asimov Name=\"asimovData_B_blind\"   Setup=\"mu=0\"     Action=\"fixsyst:fit:genasimov:float:savesnapshot\" SnapshotNuis=\"nominalNuis_B_blindFit\" SnapshotGlob=\"nominalGlob_B_blindFit\"/>" >> $out_xml
  echo "  <Asimov Name=\"asimovData_SB_SM\"  Setup=\"mu=1,mu_VBF_RW=0\"     Action=\"genasimov:reset\"/>" >> $out_xml
  echo "" >> $out_xml

  echo "</Combination>" >> $out_xml

done


