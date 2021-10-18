#include "sysUtils.h"

#include "RooTwoSidedCBShape.h"

#include "RooFitResult.h"
#include "RooDataHist.h"
#include "RooWorkspace.h"
#include "RooRealVar.h"

using namespace RooFit;

enum para{MEAN, SIGMA, ALPHALo, NLo, ALPHAHi, NHi};

void getFitPara(map<TString, vector<double>> &para, vector<std::string> files, TString sys, map<TString, double> d_tilde, map<TString, pair<float, float>> bins, map<TString, string> cats){
  int id_VBF = 346214;
  int id_ggH = 343981;

  map<TString, TH1F*> hist_VBF;
  map<TString, TH1F*> hist_ggH;
  getMyyHist(hist_VBF, id_VBF, sys, files, bins, true, d_tilde, cats);
  getMyyHist(hist_ggH, id_ggH, sys, files, bins, false, {{"SM", 0.}}, cats);

  for(auto d = d_tilde.begin(); d != d_tilde.end(); d++){
    RooWorkspace *ws = new RooWorkspace("ws","workspace");
    for(auto cat : cats){
      cout<<endl<<"========= "<<sys+"_"+d->first+"_"+cat.first<<" ========="<<endl<<endl;

      TH1F h_VBF = *(hist_VBF[sys+"_"+d->first+"_"+cat.first]);
      TH1F h_ggH = *(hist_ggH[sys+"_SM_"+cat.first]);
      TH1F hist_sig = *(hist_ggH[sys+"_SM_"+cat.first]) + *(hist_VBF[sys+"_"+d->first+"_"+cat.first]);

      RooRealVar myy("m_yy","myy",105000,160000);

      RooDataHist dh_myy_VBF("dh_myy_VBF_"+cat.first,"dh_myy_VBF", myy, Import(h_VBF));
      RooDataHist dh_myy_ggH("dh_myy_ggH_"+cat.first,"dh_myy_ggH", myy, Import(h_ggH));
      RooDataHist dh_myy_sig("dh_myy_sig_"+cat.first,"dh_myy_sig", myy, Import(hist_sig));

      RooRealVar mean("mean_"+cat.first,"mean",120000,130000);
      RooRealVar sigma("sigma_"+cat.first,"sigma",100,10000);
      RooRealVar n1("n1_"+cat.first,"",0,25);
      RooRealVar alpha1("alpha1_"+cat.first,"",0,3);
      RooRealVar n2("n2_"+cat.first,"",0,45);
      RooRealVar alpha2("alpha2_"+cat.first,"",0,3);
      RooTwoSidedCBShape DSCB_myy("sig_"+cat.first,"signal component",myy,mean,sigma,alpha1,n1,alpha2,n2);

      RooFitResult* result = DSCB_myy.fitTo(dh_myy_sig, Save(kTRUE));
      if(result->status()!=0) cout<<"WARNING : fit status != 0"<<endl;

      //TCanvas *canv = new TCanvas("c", "canvas", 800, 600);

      //RooPlot* myy_frame = myy.frame(Title("fit to m_yy distribution"));
      //dh_myy_sig.plotOn(myy_frame);
      //DSCB_myy.plotOn(myy_frame);
      //myy_frame->Draw();

      //canv->SaveAs("plotFit/"+sys+"_"+d->first+"_"+cat.first+"_"+bin->first+".png");
      //delete canv;

      if(d->first=="m00"){
        ws->import(myy);
        ws->import(mean);
        ws->import(sigma);
        ws->import(n1);
        ws->import(alpha1);
        ws->import(n2);
        ws->import(alpha2);
        ws->import(dh_myy_sig);
        ws->import(DSCB_myy);
        ws->import(dh_myy_VBF);
        ws->import(dh_myy_ggH);
      }

      para[sys+"_"+d->first+"_"+cat.first].clear();

      para[sys+"_"+d->first+"_"+cat.first].push_back(mean.getVal());
      para[sys+"_"+d->first+"_"+cat.first].push_back(sigma.getVal());
      para[sys+"_"+d->first+"_"+cat.first].push_back(alpha1.getVal());
      para[sys+"_"+d->first+"_"+cat.first].push_back(n1.getVal());
      para[sys+"_"+d->first+"_"+cat.first].push_back(alpha2.getVal());
      para[sys+"_"+d->first+"_"+cat.first].push_back(n2.getVal());
    }
    if(d->first=="m00") ws->writeToFile(("plotFit/ws_"+sys+"_"+d->first+".root").Data());
  }
}

void getMag_w1(int iSysInit = 1, int iSysFin = 1, TString sysSet = "photonallsys1", bool doSys = true){

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

  //vector<int> v_mcID;
  //v_mcID.push_back(346214);
  //v_mcID.push_back(343981);
  //int mcID = 346214;
  int mcID = 343981;

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
  TString dirpath = "/scratchfs/atlas/huirun/atlaswork/VBF_CP/ntuples/sys/shape/";
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
      if(f.find(sysSet.Data())==std::string::npos) continue;
      cout<<"f: "<<path_str+"/"+d+"/"+f<<endl;
      files.push_back(path_str+d+"/"+f);

    }
  }

  std::vector<TString> sysList;
  sysList.clear();
  if(doSys) getSysList("/scratchfs/atlas/huirun/atlaswork/VBF_CP/ntuples/sys/shape/mc16a/343981_ggF_"+sysSet+".root", sysList);
  else sysList.push_back("Nominal");

  // get syst list
  //TFile *f_in = new TFile("sample.root", "read");

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

  cout<<endl<<"total: "<<sysList_noUD.size()<<endl;
  for(auto sys : sysList_noUD){
    cout<<sys.first<<": "<<sys.second<<endl;
  }

  // loop syst fill hist

  map<TString, vector<double>> para_Nom;
  getFitPara(para_Nom, files, "Nominal", d_map, bins, catCuts);

  map<TString, vector<double>> para_SysUp;
  map<TString, vector<double>> para_SysDown;

  map<TString,double> mu_up;
  map<TString,double> mu_down;
  map<TString,double> sigma_up;
  map<TString,double> sigma_down;

  vector<TString> calc_sysList;

  int testCounter = 0;
  for(auto sys : sysList_noUD){
    if(sys.first=="Nominal") continue;
    if(!sysExistInAllFiles(files, sys.first)) continue;

    testCounter++;
    if(testCounter < iSysInit || testCounter > iSysFin) continue;

    cout<<endl<<"======="<<sys.first<<"========"<<endl;

    if(sys.second){
      getFitPara(para_SysUp, files, sys.first+"__1up", d_map, bins, catCuts);
      getFitPara(para_SysDown, files, sys.first+"__1down", d_map, bins, catCuts);
      for(auto cat : catCuts){
        TString catName = cat.first; std::cout<<catName<<endl;
        for(auto d = d_map.begin(); d != d_map.end(); d++){
          TString dname = d->first;

          TString combName = dname+"_"+catName;

          TString nomKey = "Nominal_"+combName;
          TString sysKey = sys.first+"_"+combName;
          TString upKey = sys.first+"__1up"+"_"+combName;
          TString downKey = sys.first+"__1down"+"_"+combName;

          mu_up[sysKey] = 0.;
          mu_down[sysKey] = 0.;
          sigma_up[sysKey] = 0.;
          sigma_down[sysKey] = 0.;

          mu_up[sysKey] = (para_SysUp[upKey][MEAN]-para_Nom[nomKey][MEAN])/para_Nom[nomKey][MEAN];
          mu_down[sysKey] = (para_SysDown[downKey][MEAN]-para_Nom[nomKey][MEAN])/para_Nom[nomKey][MEAN];

          sigma_up[sysKey] = (para_SysUp[upKey][SIGMA]-para_Nom[nomKey][SIGMA])/para_Nom[nomKey][SIGMA];
          sigma_down[sysKey] = (para_SysDown[downKey][SIGMA]-para_Nom[nomKey][SIGMA])/para_Nom[nomKey][SIGMA];
        }
      }
    }else {
      getFitPara(para_SysUp, files, sys.first, d_map, bins, catCuts);
      for(auto cat : catCuts){
        TString catName = cat.first; std::cout<<catName<<endl;
        for(auto d = d_map.begin(); d != d_map.end(); d++){
          TString dname = d->first;

          TString combName = dname+"_"+catName;

          TString nomKey = "Nominal_"+combName;
          TString sysKey = sys.first+"_"+combName;

          mu_up[sysKey] = 0.;
          sigma_up[sysKey] = 0.;

          mu_up[sysKey] = (para_SysUp[sysKey][MEAN]-para_Nom[nomKey][MEAN])/para_Nom[nomKey][MEAN];

          sigma_up[sysKey] = (para_SysUp[sysKey][SIGMA]-para_Nom[nomKey][SIGMA])/para_Nom[nomKey][SIGMA];
        }
      }
    }

    calc_sysList.push_back(sys.first);

    //if((int)calc_sysList.size()>0) break;
  }// end syst


  TString dirName = "/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/shape/csv/"+TString(Form("Collect_%s_%i_%i", sysSet.Data(), iSysInit, iSysFin));
  TString tsCommand = "if [ ! -d "+dirName+" ];then mkdir -p "+dirName+";fi"; cout<<endl<<tsCommand<<endl<<endl;
  system(tsCommand.Data());

  // fill csv file
  ofstream ofpara_Nom(Form("/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/shape/csv/Collect_%s_%i_%i/para_Nom.csv", sysSet.Data(), iSysInit, iSysFin), ios::out);
  if(!ofpara_Nom){
    ofpara_Nom.close();
    cout<<"error can't open file for record"<<endl;
  }
  for(auto p = para_Nom.begin(); p!=para_Nom.end(); p++){
    ofpara_Nom<<p->first<<","<<p->second[MEAN]<<","<<p->second[SIGMA]<<","<<p->second[ALPHALo]<<","<<p->second[NLo]<<","<<p->second[ALPHAHi]<<","<<p->second[NHi]<<endl;
  }
  ofpara_Nom.close();

  for(auto cat : catCuts){
   for(auto d = d_map.begin(); d != d_map.end(); d++){
      ofstream ofmu(Form("/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/shape/csv/Collect_%s_%i_%i/mu_"+d->first+"_"+cat.first+".csv", sysSet.Data(), iSysInit, iSysFin), ios::out);
      if(!ofmu){
        ofmu.close();
        cout<<"error can't open file for record"<<endl;
      }
    
      ofstream ofsigma(Form("/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/shape/csv/Collect_%s_%i_%i/sigma_"+d->first+"_"+cat.first+".csv", sysSet.Data(), iSysInit, iSysFin), ios::out);
      if(!ofsigma){
        ofsigma.close();
        cout<<"error can't open file for record"<<endl;
      }
    
      for(auto sys : calc_sysList){
        if(sysList_noUD[sys]) {
          ofmu<<sys<<","<<mu_up[sys+"_"+d->first+"_"+cat.first]<<","<<mu_down[sys+"_"+d->first+"_"+cat.first]<<endl;
          ofsigma<<sys<<","<<sigma_up[sys+"_"+d->first+"_"+cat.first]<<","<<sigma_down[sys+"_"+d->first+"_"+cat.first]<<endl;
        }
        else{
          ofmu<<sys<<","<<mu_up[sys+"_"+d->first+"_"+cat.first]<<endl;
          ofsigma<<sys<<","<<sigma_up[sys+"_"+d->first+"_"+cat.first]<<endl;
        }

      }
    
      ofmu.close();
      ofsigma.close();
    }
  }

//  // release hist heaps in getMyyHist()
//  for(auto hist = histVec.begin(); hist != histVec.end(); hist++){
//    delete hist->second;
//  }
}

int main(int argc, char* argv[]){
  int init = std::atoi(argv[1]);
  int fin = std::atoi(argv[2]);
  TString sysSet(argv[3]);
  getMag_w1(init,fin,sysSet);
  return 0;
}
