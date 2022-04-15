#!/usr/bin/python
# -*- coding: UTF-8 -*-

import fileinput

ggF_qcd_NPs=["_mu",
"_res",
"_mig01",
"_mig12",
"_pTH10",
"_pTH60",
"_pTH120",
"_mjj350",
"_mjj700",
"_mjj1000",
"_mjj1500",
"_pTHjj",
"_pTH300",
"_pTH450",
"_pTH650",
"_pTHj_over_pTH",
"_mu_highpTH",
"_mTop_scheme",
"_Acc ",
]

VBF_qcd_NPs=["_mu",
"_PTH200",
"_Mjj60",
"_Mjj120",
"_Mjj350",
"_Mjj700",
"_Mjj1000",
"_Mjj1500",
"_pTjH25",
"_JET01",
"_Acc",
]

dvals = ['m00',
'm01',
'm02',
'm03',
'm04',
'm05',
'm06',
'm07',
'm08',
'm09',
'm10',
'm11',
'm12',
'm13',
'm14',
'm15',
'm16',
'm17',
'm18',
'm19',
'm20',
'p01',
'p02',
'p03',
'p04',
'p05',
'p06',
'p07',
'p08',
'p09',
'p10',
'p11',
'p12',
'p13',
'p14',
'p15',
'p16',
'p17',
'p18',
'p19',
'p20',
]
mcid = '346214'

bdtcats = ['TT', 'TL', 'LT']
oobins = ['b1', 'b2', 'b3', 'b4', 'b5', 'b6']

dvals = ['SM']
mcid = '343981'
for d in dvals:
  for cat in bdtcats:
    for b in oobins:
      fname = "mag_theory_%s_%s_%s_%s.csv"%(mcid, d, cat, b)
      print fname
      with open(fname, 'r') as f:
        txt = f.read()
        i = 0
        if mcid == '346214':
          for np in VBF_qcd_NPs:
            i += 1
            txt = txt.replace('qcd%i_VBF'%(i), 'qcd_VBF%s'%(np))
        elif mcid == '343981':
          for np in ggF_qcd_NPs:
            i += 1
            txt = txt.replace('qcd%i_ggF'%(i), 'qcd_ggF%s'%(np))
#        print txt
      
      with open(fname, 'w') as f:
        f.write(txt)
