#! /bin/bash

unblind=0

bkgFuncBias_Toy=0
bkgFuncBias_Asi=0

injectTest=0
injectPoint=m03

d_tilde=$(ls csv/ | grep TT_b3 | grep -v SM | cut -d '_' -f 4)
d_tilde=$(cat ../Dtilde | grep -v "#")
bin=$(ls csv/ | grep m01 | cut -d '_' -f 5 | cut -d '.' -f 1)

#cats=$(ls /scratchfs/atlas/chenhr/atlaswork/VBF_CP/calcBDT/outputs/mc16a/ | grep  343981_ggF_Nominal | cut -d _ -f 4 | cut -d . -f 1)
cats=$(cat ../../nom_WS/cats.cfg | grep -v "#" | grep ":" | cut -d ":" -f 1)
cats=$(cat ../../nom_WS/cats.cfg | grep -v "#" | grep ":" | cut -d ":" -f 1 | grep -v LL)

echo $d_tilde
echo $bin

echo $cats

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
  if [ ${unblind} -eq 0 ];then
    echo "<Combination WorkspaceName=\"combWS\" ModelConfigName=\"ModelConfig\" DataName=\"combData\" OutputFile=\"workspace/vbf_cp_${d}/vbf_cp_${d}.root\" Blind=\"true\">" >> $out_xml
  else
    echo "<Combination WorkspaceName=\"combWS\" ModelConfigName=\"ModelConfig\" DataName=\"combData\" OutputFile=\"workspace/vbf_cp_${d}/vbf_cp_${d}.root\" Blind=\"false\">" >> $out_xml
  fi
  echo "" >> $out_xml

  for cat in $cats;do
    echo "  <Input>config/vbf_cp_${d}/channel/category_OO_${cat}.xml</Input>" >> $out_xml
  done

  if [ $injectTest -eq 1 ];then
    echo "  <POI>mu_VBF_RW,mu_VBF_SM,mu_VBF_${injectPoint},mu_ggH,mu_ggH_SM,mu_ggH_${injectPoint},mu</POI>" >> $out_xml
  elif [ $bkgFuncBias_Asi -eq 1 ];then
    echo "  <POI>mu_bkg,mu_bkg_funcBias_Asi,mu_VBF_RW,mu_VBF_SM,mu_ggH,mu_ggH_SM,mu</POI>" >> $out_xml
  else
    echo "  <POI>mu_VBF_RW,mu_VBF_SM,mu_ggH,mu_ggH_SM,mu</POI>" >> $out_xml
  fi
  echo "" >> $out_xml

  if [ $bkgFuncBias_Asi -eq 1 ];then
    echo "  <Asimov Name=\"asimovData_B_blind\"   Setup=\"mu=0,mu_bkg=0,mu_bkg_funcBias_Asi=1\"     Action=\"fixsyst:fit:genasimov:float:savesnapshot\" SnapshotNuis=\"nominalNuis_B_blindFit\" SnapshotGlob=\"nominalGlob_B_blindFit\"/>" >> $out_xml
  elif [ $bkgFuncBias_Toy -eq 1 ];then
    echo "do nothing"
#    echo "  <Asimov Name=\"toyFit\"   Setup=\"mu=1,mu_VBF_RW=1_0_5,mu_ggH=1,mu_VBF_SM=0,mu_ggH_SM=0\"     Action=\"fixsyst:fit:float:savesnapshot\" SnapshotNuis=\"nominalNuis_toyFit\" SnapshotGlob=\"nominalGlob_toyFit\"/>" >> $out_xml
  else
    #echo "  <Asimov Name=\"asimovData_SM_noFit\"   Setup=\"mu_VBF_RW=0,mu_VBF_SM=1,mu=1\"     Action=\"raw:fixall:genasimov:float:savesnapshot\" SnapshotNuis=\"nominalNuis_SM_noFit\" SnapshotGlob=\"nominalGlob_SM_noFit\"/>" >> $out_xml # PE b, c
    echo "  <Asimov Name=\"asimovData_B_blind\"   Setup=\"mu=0\"     Action=\"fixsyst:fit:genasimov:float:savesnapshot\" SnapshotNuis=\"nominalNuis_B_blindFit\" SnapshotGlob=\"nominalGlob_B_blindFit\"/>" >> $out_xml
  fi

  if [ $injectTest -eq 1 ];then
    echo "  <Asimov Name=\"asimovData_SB_SM\"  Setup=\"mu=1,mu_VBF_RW=0,mu_ggH=0,mu_VBF_${injectPoint}=0,mu_ggH_${injectPoint}=0\"     Action=\"genasimov:reset\"/>" >> $out_xml
    echo "  <Asimov Name=\"asimovData_SB_${injectPoint}\"  Setup=\"mu=1,mu_VBF_RW=0,mu_ggH=0,mu_VBF_SM=0,mu_ggH_SM=0\"     Action=\"genasimov:reset\"/>" >> $out_xml
  elif [ $bkgFuncBias_Toy -eq 1 ];then
    echo "do nothing"
  elif [ $bkgFuncBias_Asi -eq 1 ];then
#    echo "  <Asimov Name=\"asimovData_SB_SM\"  Setup=\"mu=1,mu_VBF_RW=0,mu_ggH=0,mu_bkg=1,mu_bkg_funcBias_Asi=0\"     Action=\"genasimov:reset\"/>" >> $out_xml
    echo "  <Asimov Name=\"asimovData_SB_bkgFuncBias\"  Setup=\"mu=1,mu_VBF_RW=0,mu_ggH=0,mu_bkg=0,mu_bkg_funcBias_Asi=1\"     Action=\"genasimov:reset\"/>" >> $out_xml
  else
    echo "  <Asimov Name=\"asimovData_SB_SM\"  Setup=\"mu=1,mu_VBF_RW=0,mu_ggH=0\"     Action=\"genasimov:reset\"/>" >> $out_xml
  fi
  echo "  <Asimov Name=\"asimovData_SB_CPeven10\"  Setup=\"mu=1,mu_VBF_SM=1.1,mu_VBF_RW=0,mu_ggH=0\"     Action=\"genasimov:reset\"/>" >> $out_xml
  echo "  <Asimov Name=\"asimovData_SB_CPeven20\"  Setup=\"mu=1,mu_VBF_SM=1.2,mu_VBF_RW=0,mu_ggH=0\"     Action=\"genasimov:reset\"/>" >> $out_xml
  echo "" >> $out_xml

  echo "</Combination>" >> $out_xml

done


