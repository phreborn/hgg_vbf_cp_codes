//#include "sysUtils.h"
#include "../shape_sys/sysUtils.h"

void getMag(int sampleID, int iSysInit = 1, int iSysFin = 1, bool doSys = true){

  char *cf_cats = (char*)"../../nom_WS/cats.cfg";
  map<TString, string> catCuts;
  getCatCuts(cf_cats, catCuts); for(auto c : catCuts) cout<<c.first<<c.second<<endl;

  string config = "../../nom_WS/config";
  readConfigFile(config.data(), "cuts", baseCuts);
  readConfigFile(config.data(), "blindSel", blindCut);

  // config maps
  lumi["mc16a"] = 36207.66;
  lumi["mc16d"] = 44307.4;
  lumi["mc16e"] = 58450.1;

  vector<int> v_mcID;
  v_mcID.push_back(sampleID);
  int mcID = -1;
  //v_mcID.push_back(346214);
  //v_mcID.push_back(343981);
  //int mcID = 346214;
  //int mcID = 343981;

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

  map<TString, pair<float, float>> bins;
  bins["b1"] = make_pair(-999999999, -2);
  bins["b2"] = make_pair(-2, -1);
  bins["b3"] = make_pair(-1, 0);
  bins["b4"] = make_pair(0, 1);
  bins["b5"] = make_pair(1, 2);
  bins["b6"] = make_pair(2, 99999999);

  std::map<TString, std::vector<float>> cats;
  cats["TT"] = {0.14, 1., 0.23, 1.};
  cats["TL"] = {0.14, 1., -1., 0.23};
  cats["LT"] = {-1, 0.14, 0.05, 1.};
  cats["LL"] = {-1, 0.14, -1., 0.05};

  // file path list
  //TString dirpath = "/scratchfs/atlas/chenhr/atlaswork/VBF_CP/calcBDT/outputs/";
  TString dirpath = "../../ntuples/sys/yield/";
  std::string path_str = dirpath.Data();
  std::vector<std::string> sub_dirs = getDirBinsSortedPath(path_str);

  std::vector<std::string> files(0);

  for(auto d : sub_dirs){
    if(d==".") continue;
    if(d.find("mc") == std::string::npos) continue;
    cout<<"d: "<<path_str+d<<endl;
    std::vector<std::string> fs = getDirBinsSortedPath(path_str+d+"/");
    for(auto f : fs){
      if(f==".") continue;
      if(f.find(".root")==std::string::npos) continue;
      cout<<"f: "<<path_str+"/"+d+"/"+f<<endl;
      files.push_back(path_str+d+"/"+f);

    }
  }

  std::vector<TString> sysList;
  sysList.clear();
  if(doSys) {
    getSysList("/scratchfs/atlas/chenhr/atlaswork/VBF_CP/ntuples/sys/yield/mc16a/343981_ggF_allSys.root", sysList);
    //getSysList("/scratchfs/atlas/chenhr/atlaswork/VBF_CP/ntuples/sys/yield/mc16e/346214_VBF_allSys.root", sysList);
    cout<<endl<<"sys list got"<<endl;
  }
  else sysList.push_back("Nominal");

  // get syst list
  ////TFile *f_in = new TFile("sample.root", "read");
  //TFile *f_in = new TFile("../shape_sys/sample.root", "read");

  //TTree *tree = (TTree*) f_in->Get("output");

  //std::vector<TString> sysList;
  //sysList.clear();

  //getSysList(tree, "m_yy", sysList);

  //delete f_in;

  vector<TString> ignoreList;
  ignoreList.clear();
  //readList(Form("ignore_syst_yield_%i.txt", mcID), ignoreList);

  map<TString, bool> sysList_noUD;

  for(auto sys : sysList){
    bool ignore = false;
    //cout<<"sys: "<<sys<<endl;
    //TString tmp = sys;
    //tmp.ReplaceAll("__1up", "").ReplaceAll("__1down", ""); // don't put ReplaceAll() in for()
    for(auto igl : ignoreList) {
      //if(tmp == igl) ignore = true;
      if(sys.Contains(igl)) ignore = true;
      //if(ignore) cout<<"match ignore: "<<igl<<endl;
      if(ignore) break;
    }
    if(ignore) continue;
    if (sys.Contains("__1up")) sysList_noUD[sys.ReplaceAll("__1up", "")] = true;
    else if(!sys.Contains("__1down")) sysList_noUD[sys] = false;
  }

  for(auto sys : sysList_noUD){
    cout<<sys.first<<": "<<sys.second<<endl;
  }

  // loop syst fill hist
  for(auto id : v_mcID){
    mcID = id;
    cout<<"======="<<mcID<<"========"<<endl;

    std::map<TString, TH1F*> histVec;
    map<TString, double> N_nominal;
    map<TString,double> mag_up;
    map<TString,double> mag_down;

    map<TString, double> d_tmp = {{"SM", 0.}};
    if(mcID == 346214) d_tmp = d_map;

    getMyyHist(histVec, mcID, "Nominal", files, bins, true, d_tmp, catCuts);
    //getMyyHist(histVec, mcID, "Nominal", files, bins);
  
for(auto cat : catCuts){
      for(auto d = d_tmp.begin(); d != d_tmp.end(); d++){
        N_nominal[d->first+"_"+cat.first] = histVec["Nominal_"+d->first+"_"+cat.first]->Integral(); cout<<d->first+"_"+cat.first<<","<<N_nominal[d->first+"_"+cat.first]<<endl;
      }
}
    vector<TString> calc_sysList;

    int testCounter = 0;
    for(auto sys : sysList_noUD){
      if(sys.first=="Nominal") continue;

      testCounter++;
      if(testCounter < iSysInit || testCounter > iSysFin) continue;

      if(!sysExistInAllFiles(files, sys.first)) continue;

      //if(!sysExistInAllFiles(files, sys.first)) continue;
      //if(!sys.first.Contains("PRW")&&!sys.first.Contains("JET_EffectiveNP_Detector")&&!sys.first.Contains("MET_SoftTrk_ResoPara")) continue;
      cout<<"======="<<sys.first<<"========"<<endl;
      if(sys.second){
        getMyyHist(histVec, mcID, sys.first+"__1up", files, bins, true, d_tmp, catCuts);// to delete
        getMyyHist(histVec, mcID, sys.first+"__1down", files, bins, true, d_tmp, catCuts);// to delete
for(auto cat : catCuts){
  TString catName = cat.first; std::cout<<catName<<endl;
          for(auto d = d_tmp.begin(); d != d_tmp.end(); d++){
            TString dname = d->first;

            TString combName = dname+"_"+catName;

            TString nomKey = combName;
            TString sysKey = sys.first+"_"+combName;
            TString upKey = sys.first+"__1up"+"_"+combName;
            TString downKey = sys.first+"__1down"+"_"+combName;

            mag_up[sysKey] = (histVec[upKey]->Integral()-N_nominal[nomKey])/N_nominal[nomKey]; //cout<<"UP"<<","<<sys.first<<","<<mag_up[sys.first]<<endl;
            mag_down[sysKey] = (histVec[downKey]->Integral()-N_nominal[nomKey])/N_nominal[nomKey]; //cout<<"DOWN"<<","<<sys.first<<","<<mag_down[sys.first]<<endl;
          }
}
      }else {
        getMyyHist(histVec, mcID, sys.first, files, bins, true, d_tmp, catCuts);// to delete
for(auto cat : catCuts){
  TString catName = cat.first; std::cout<<catName<<endl;
          for(auto d = d_tmp.begin(); d != d_tmp.end(); d++){
            TString dname = d->first;

            TString combName = dname+"_"+catName;

            TString nomKey = combName;
            TString sysKey = sys.first+"_"+combName;

            mag_up[sysKey] = (histVec[sysKey]->Integral()-N_nominal[nomKey])/N_nominal[nomKey]; //cout<<"UP/DOWN"<<","<<sys.first<<","<<mag_up[sys.first]<<endl;
          }
}
      }
  
      calc_sysList.push_back(sys.first);
  
      //if((int)calc_sysList.size()>0) break;
    }// end syst
  
    TString dirName = "csv/"+TString(Form("Collect_%i_%i", iSysInit, iSysFin));
    TString tsCommand = "if [ ! -d "+dirName+" ];then mkdir -p "+dirName+";fi"; cout<<endl<<tsCommand<<endl<<endl;
    system(tsCommand.Data());
 
    // fill csv file
    for(auto cat : catCuts){
      for(auto d = d_tmp.begin(); d != d_tmp.end(); d++){
        ofstream ofsyst(Form("csv/Collect_%i_%i/mag_yield_%i_"+d->first+"_"+cat.first+".csv", iSysInit, iSysFin, mcID), ios::out);
        if(!ofsyst){
          ofsyst.close();
          cout<<"error can't open file for record"<<endl;
        }
  
        for(auto sys : calc_sysList){
          if(sysList_noUD[sys]) ofsyst<<sys<<","<<mag_up[sys+"_"+d->first+"_"+cat.first]<<","<<mag_down[sys+"_"+d->first+"_"+cat.first]<<endl;
          else ofsyst<<sys<<","<<mag_up[sys+"_"+d->first+"_"+cat.first]<<endl;
        }
  
        ofsyst.close();
      }
    }

    // fill nominal yields
    ofstream ofyield_clear(Form("csv/Collect_%i_%i/N_yield.csv", iSysInit, iSysFin), ios::app);
    ofyield_clear.close();
    ofstream ofyield(Form("csv/Collect_%i_%i/N_yield.csv", iSysInit, iSysFin), ios::app);
    if(!ofyield){
      ofyield.close();
      cout<<"error can't open file for record"<<endl;
    }
for(auto cat : catCuts){
      if(mcID==343981){
        for(auto d = d_tmp.begin(); d != d_tmp.end(); d++){
          ofyield<<"ggH_"+cat.first<<","<<N_nominal[d->first+"_"+cat.first]<<endl;
        }
      }
      else if(mcID==346214) {// why VBF saved twice?
        for(auto d = d_tmp.begin(); d != d_tmp.end(); d++){
          ofyield<<"VBF_"+d->first+"_"+cat.first<<","<<N_nominal[d->first+"_"+cat.first]<<endl;
        }
      }
}

    // release hist heaps in getMyyHist()
    for(auto hist = histVec.begin(); hist != histVec.end(); hist++){
      delete hist->second;
    }
    cout<<"end mcID"<<endl;
  }// end mcID
}
