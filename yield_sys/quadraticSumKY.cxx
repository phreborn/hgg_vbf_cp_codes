#include "smeftUtils.h"

void writeXml(map<string, pair<float, float>> sysV, string file){
  ofstream ofsyst(file, ios::out);
  if(!ofsyst){
    ofsyst.close();
    cout<<file<<endl;
    cout<<"error can't open file for record"<<endl;
  }
  for(auto sys : sysV){
    string sysName = sys.first;
    float vu = sys.second.first;
    float vd = sys.second.second;
    ofsyst<<sysName<<","<<vu<<","<<vd<<endl;
  }
  ofsyst.close();
}

void quadraticSumKY(){
  map<TString, TString> cHWs;
  cHWs["m0d00"] = "-0";
  cHWs["m0d05"] = "-0.05";
  cHWs["m0d10"] = "-0.1";
  cHWs["m0d15"] = "-0.15";
  cHWs["m0d20"] = "-0.2";
  cHWs["m0d25"] = "-0.25";
  cHWs["m0d30"] = "-0.3";
  cHWs["m0d35"] = "-0.35";
  cHWs["m0d40"] = "-0.4";
  cHWs["m0d45"] = "-0.45";
  cHWs["m0d50"] = "-0.5";
  cHWs["m0d55"] = "-0.55";
  cHWs["m0d60"] = "-0.6";
  cHWs["m0d65"] = "-0.65";
  cHWs["m0d70"] = "-0.7";
  cHWs["m0d75"] = "-0.75";
  cHWs["m0d80"] = "-0.8";
  cHWs["m0d85"] = "-0.85";
  cHWs["m0d90"] = "-0.9";
  cHWs["m0d95"] = "-0.95";
  cHWs["p0d05"] = "0.05";
  cHWs["p0d10"] = "0.1";
  cHWs["p0d15"] = "0.15";
  cHWs["p0d20"] = "0.2";
  cHWs["p0d25"] = "0.25";
  cHWs["p0d30"] = "0.3";
  cHWs["p0d35"] = "0.35";
  cHWs["p0d40"] = "0.4";
  cHWs["p0d45"] = "0.45";
  cHWs["p0d50"] = "0.5";
  cHWs["p0d55"] = "0.55";
  cHWs["p0d60"] = "0.6";
  cHWs["p0d65"] = "0.65";
  cHWs["p0d70"] = "0.7";
  cHWs["p0d75"] = "0.75";
  cHWs["p0d80"] = "0.8";
  cHWs["p0d85"] = "0.85";
  cHWs["p0d90"] = "0.9";
  cHWs["p0d95"] = "0.95";
  cHWs["p1d00"] = "1";

  /*cHWs["m2d5"] = "-2.5";
  cHWs["m2d0"] = "-2";
  cHWs["m1d5"] = "-1.5";
  cHWs["m1d0"] = "-1";
  cHWs["m0d5"] = "-0.5";
  cHWs["m0d0"] = "-0";
  cHWs["p0d5"] = "0.5";
  cHWs["p1d0"] = "1";
  cHWs["p1d5"] = "1.5";
  cHWs["p2d0"] = "2";
  cHWs["p2d5"] = "2.5";*/

  vector<TString> cats = {"TT", "TL", "LT", "LL"};

  map<TString, int> mBins;
  mBins["b1"] = 0;
  mBins["b2"] = 1;
  mBins["b3"] = 2;
  mBins["b4"] = 3;
  mBins["b5"] = 4;
  mBins["b6"] = 5;

  for(auto cHW : cHWs){
    string cHWName(cHW.first.Data());
    string cHWNum(cHW.second.Data());
    for(auto cat : cats){
      for(auto bin : mBins){
        int ibin = bin.second;
        TString bName = bin.first;
        TString cbin = cat+"_"+bName; cout<<endl<<cbin<<endl;

        TString fpath = "csv/mag_yield_346214_m00_"+cbin+".csv";
        std::map<TString, std::pair<float,float>> sysud;
        if(!getVariY(fpath, sysud)) continue;

        TString jerfpath = "/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/yield/csv_jd/mag_yield_346214_m00_"+cbin+".csv";
        std::map<TString, std::pair<float,float>> jersysud;
        if(!getVariY(jerfpath, jersysud)) continue;

        map<string, pair<float, float>> sysV;
        for(auto sys : sysud){
          string sysName(sys.first.Data());
          float yu = sysName.find("JER_") != std::string::npos ? jersysud[sysName.data()].first : sys.second.first;
          float yd = sysName.find("JER_") != std::string::npos ? jersysud[sysName.data()].second : sys.second.second;
          float ku = getVariKF(sysName, cat.Data(), cHWNum, ibin, "Up");
          float kd = getVariKF(sysName, cat.Data(), cHWNum, ibin, "Down");
          if(std::abs(ku) > 0.1) { cout<<"Warning: |ku| > 10%, "<<ku<<", "<<cbin<<", "<<sys.first<<endl; ku = 0; }
          if(std::abs(kd) > 0.1) { cout<<"Warning: |kd| > 10%, "<<kd<<", "<<cbin<<", "<<sys.first<<endl; kd = 0; }
          float yku = qdrYK(yu, ku); cout<<yu<<", "<<ku<<": "<<yku<<endl;
          float ykd = qdrYK(yd, kd); cout<<yd<<", "<<kd<<": "<<ykd<<endl;
          sysV[sysName] = make_pair(yku, ykd);
        }
        //string outxml = Form("sample_346214_%s_%s.xml", cHWName.data(), cbin.Data());
        string outxml = Form("/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/yield/csv/mag_yield_346214_%s_%s.csv", cHWName.data(), cbin.Data());
        writeXml(sysV, outxml);
      }
    }
  }
}
