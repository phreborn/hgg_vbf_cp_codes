#include "ioUtils.h"
#include "../shape_sys/sysUtils.h"

void JER_variation(){
  char *cf_cats = (char*)"../../nom_WS/cats.cfg";
  map<TString, string> catCuts;
  getCatCuts(cf_cats, catCuts); for(auto c : catCuts) cout<<c.first<<c.second<<endl;

  map<TString, double> d_map;
  d_map["m00"] = 0.;
  d_map["m01"] = -0.01;
  d_map["m02"] = -0.02;
  d_map["m03"] = -0.03;
  d_map["m04"] = -0.04;
  d_map["m05"] = -0.05;
  d_map["m06"] = -0.06;
  d_map["m07"] = -0.07;
  d_map["m08"] = -0.08;
  d_map["m10"] = -0.10;
  d_map["m12"] = -0.12;
  d_map["m14"] = -0.14;
  d_map["m16"] = -0.16;
  d_map["m18"] = -0.18;
  d_map["m20"] = -0.20;
  d_map["p01"] = 0.01;
  d_map["p02"] = 0.02;
  d_map["p03"] = 0.03;
  d_map["p04"] = 0.04;
  d_map["p05"] = 0.05;
  d_map["p06"] = 0.06;
  d_map["p07"] = 0.07;
  d_map["p08"] = 0.08;
  d_map["p10"] = 0.10;
  d_map["p12"] = 0.12;
  d_map["p14"] = 0.14;
  d_map["p16"] = 0.16;
  d_map["p18"] = 0.18;
  d_map["p20"] = 0.20;

  vector<TString> v_mcID = {"346214", "343981"};

  for(auto dsid : v_mcID){
    map<TString, double> d_tmp = {{"SM", 0.}};
    if(dsid == "346214") d_tmp = d_map;
    for(auto d : d_tmp){
      for(auto cat : catCuts){
        TString dName = d.first;
        TString catName = cat.first;

        TString basepath = "/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/yield/";

        TString mfpath = basepath+"csv_w1/mag_yield_"+dsid+"_"+dName+"_"+catName+".csv";
        std::map<TString, std::pair<float,float>> msysud;
        getVariation(mfpath, msysud);

        TString dfpath = "/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/yield/csv_w1/jd_mag_yield_"+dsid+"_"+dName+"_"+catName+".csv";
        std::map<TString, std::pair<float,float>> dsysud;
        getVariation(dfpath, dsysud);
 
        std::map<string, std::pair<float,float>> mdsysud;
        for(auto sys : dsysud){
          TString sysName = sys.first;
          if(!sysName.Contains("JER_")) continue;
          float mvu = msysud[sysName].first;
          float mvd = msysud[sysName].second;
          float dvu = dsysud[sysName].first;
          float dvd = dsysud[sysName].second;

          mdsysud[sysName.Data()] = std::make_pair(mvu-dvu, mvd-dvd);
        }
        string outcsv = Form("/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/yield/csv_jd_w1/mag_yield_%s_%s_%s.csv", dsid.Data(), dName.Data(), catName.Data());
        writeCsv(mdsysud, outcsv);        
      }
    }
  }
}
