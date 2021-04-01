#!/bin/bash

tmp=tmp.sh
> ${tmp}

d_tilde=$(ls yield_sys/csv/ | grep b3 | grep -v SM | grep TT | cut -d '_' -f 4)
cats=$(cat ../nom_WS/cats.cfg | grep -v "#" | grep ":" | cut -d ":" -f 1)

for d in ${d_tilde};do
for cat in ${cats};do
#for d in p01;do
#for cat in TT_b1;do
  file=config/vbf_cp_${d}/channel/category_OO_${cat}.xml; echo ${file}
  pos=$(sed -n '/background/=' ${file})
  spurious=$(cat shape_sys/bkg_SS.csv | grep ${cat} | cut -d , -f 3)
  insert="<Sample Name=\"spurious\" InputFile=\"config/vbf_cp_${d}/model/signal_:category:.xml\" ImportSyst=\":self:\" MultiplyLumi=\"false\" SharePdf=\"commonSig\">\n    <Systematic Name=\"ATLAS_Hgg_BIAS_:category:\" Constr=\"gaus\" CentralValue=\"0\" Mag=\"${spurious}\" WhereTo=\"yield\"/>\n  </Sample>\n"
  echo "sed -i '${pos}i\  ${insert}' ${file}" >> ${tmp}
done
done

if [ "$1" = "cmd" ];then
  cat ${tmp}
elif [ "$1" = "exe" ];then
  source ${tmp}
fi
rm ${tmp}
