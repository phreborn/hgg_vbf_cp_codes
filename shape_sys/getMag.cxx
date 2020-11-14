#include "/afs/ihep.ac.cn/users/g/guofy/HggTwoSidedCBPdf.cxx"
#include "/afs/ihep.ac.cn/users/g/guofy/HggTwoSidedCBPdf.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <iterator>
#include <vector>
#include <stdio.h>
#include <dirent.h>


using namespace std;
using namespace RooFit;

// directory scanner
int fileNameFilter(const struct dirent *cur) {
    std::string str(cur->d_name);
    if (str.find("..") != std::string::npos) {
        return 0;
    }
    return 1;
}

std::vector<std::string> getDirBinsSortedPath(std::string dirPath) {
    struct dirent **namelist;
    std::vector<std::string> ret;
    int n = scandir(dirPath.c_str(), &namelist, fileNameFilter, alphasort);
    if (n < 0) {
        return ret;
    }
    for (int i = 0; i < n; ++i) {
        std::string filePath(namelist[i]->d_name);
        ret.push_back(filePath);
        free(namelist[i]);
    };
    free(namelist);
    return ret;
}

// a config file reader
static bool readConfigFile(const char * cfgfilepath, const string & key, string & value)
{
    fstream cfgFile;
    cfgFile.open(cfgfilepath);
    if( ! cfgFile.is_open())
    {
        cout<<"can not open cfg file!"<<endl;
        return false;
    }
    char tmp[1000];
    while(!cfgFile.eof())
    {
        cfgFile.getline(tmp,1000);
        string line(tmp);
        size_t pos = line.find(':');
        //if(pos==string::npos) return false;
        if(pos==string::npos) continue;
        string tmpKey = line.substr(0,pos);
        if(key==tmpKey)
        {
            value = line.substr(pos+1);
        }
    }
    return false;
}

void readList(TString file, std::vector<TString> &vec){
    fstream listFile;
    listFile.open(file);
    if( ! listFile.is_open())
    {
        cout<<"can not open list file!"<<endl;
        return false;
    }
    char tmp[1000];
    while(!listFile.eof())
    {
        listFile.getline(tmp,1000);
        string line(tmp);
        if(line!="") vec.push_back(line.data());
    }
}


TString getMCSampleName(int mcID){
  string name;
  //cout<<Form("SampleName.%d", mcID)<<endl;
  readConfigFile("../MCSamples.config", Form("SampleName.%d", mcID), name);
  while(name.find(" ")!=std::string::npos) { name.replace(name.find(" "), 1, ""); }
  return name.data();
}

TH1F *getCutFlowHist(int mcID, TFile* file){
  TString suffix = "_noDalitz_weighted";
  TString cutFlowName = Form("CutFlow_%s%s", getMCSampleName(mcID).Data(), suffix.Data());
  //cout<<cutFlowName<<endl;
  TH1F *cutFlow = (TH1F*) file->Get(cutFlowName);
  return cutFlow;
}

double getSumOfWeights(int mcID, TFile* file){
  double NxAOD = getCutFlowHist(mcID, file)->GetBinContent(1);
  double NDxAOD = getCutFlowHist(mcID, file)->GetBinContent(2);
  double WDxAOD = getCutFlowHist(mcID, file)->GetBinContent(3);

  double weightSum = WDxAOD*NxAOD/NDxAOD;
  cout<<"xAOD, DxAOD, allEvt: "<<NxAOD<<", "<<NDxAOD<<", "<<WDxAOD<<endl;
  return weightSum;
}

void getSysList(TTree *tree, TString anchorVar, std::vector<TString> &sysList){
  TObjArray *branches = tree->GetListOfBranches(); 
  
  for(int i = 0; i < branches->GetEntries(); i++){
    TBranch *branch = (TBranch *)branches->At(i);
    TString bname = branch->GetName();

    if (!bname.Contains(anchorVar)) { continue; }

    bname.ReplaceAll("."+anchorVar, "");

    if (bname.IsNull()) { continue; }
    //if (bname=="Nominal") { continue; } //cout<<bname<<endl;

    sysList.push_back(bname);
  }
}

map<TString,float> lumi;

void getMyyHist(map<TString, TH1F*> &hists, int mcID, TString syst, std::vector<std::string> fpaths, map<TString, pair<float, float>> OObins = {{"full", make_pair(-99999, 99999)}}, bool isVBF = false, map<TString, double> d_tildes = {{"SM", 0.}}){
  TString id = Form("%i", mcID);

  for(auto bin = OObins.begin(); bin != OObins.end(); bin++){
    for(auto d = d_tildes.begin(); d != d_tildes.end(); d++){
      hists[syst+"_"+d->first+"_"+bin->first] = new TH1F(syst+"_m_yy_"+d->first+"_"+bin->first, "", 550, 105000,160000);
    }
  }

  for(auto camp = lumi.begin(); camp != lumi.end(); camp++){
    cout<<camp->first<<endl;

    TString fpath = "";

    for(auto f : fpaths){
      TString filepath = f.data();
      if(f.find(camp->first) == std::string::npos || f.find(Form("%i", mcID)) == std::string::npos) continue; // to select mc name
      fpath = filepath; cout<<fpath<<endl;
    }

    if(fpath == "") cout<<"file path not found for "<<mcID<<", "<<camp->first<<endl;

    TFile *f_w = new TFile(fpath, "read");

    double sumOfWeights = getSumOfWeights(mcID, f_w); cout<<sumOfWeights<<endl;

    TTree *tree = (TTree*) f_w->Get("output");

    Int_t N_j_30,N_photon,cutflow,Category;
    Float_t m_yy,pT_y1,pT_y2,m_jj_30,DeltaEta_jj,Zepp,oo1,oo2,WeightDtilde1,WeightDtilde2,weight,xsec_kF_eff,total_weights;
    Bool_t isDalitz,isPassedIsolation,isPassedPID,isPassedTriggerMatch,isPassed;
  
    tree->SetBranchAddress(syst+".Category", &Category);
    tree->SetBranchAddress(syst+".isPassed", &isPassed);
    tree->SetBranchAddress(syst+".m_yy", &m_yy);
    tree->SetBranchAddress(syst+".N_j_30", &N_j_30);
    tree->SetBranchAddress(syst+".m_jj_30", &m_jj_30);
    tree->SetBranchAddress(syst+".DeltaEta_jj", &DeltaEta_jj);
    tree->SetBranchAddress(syst+".Zepp", &Zepp);
    tree->SetBranchAddress(syst+".oo1", &oo1);
    tree->SetBranchAddress(syst+".oo2", &oo2);
    tree->SetBranchAddress(syst+".weight", &weight);
    tree->SetBranchAddress("xsec_kF_eff", &xsec_kF_eff);
    tree->SetBranchAddress("isDalitz", &isDalitz);

    if(isVBF){
      tree->SetBranchAddress("WeightDtilde1", &WeightDtilde1);
      tree->SetBranchAddress("WeightDtilde2", &WeightDtilde2);
    }
  
    Long64_t endentry = tree->GetEntries();
  
    for(int i = 0; i < endentry; i++){
      tree->GetEntry(i);
      if(i==endentry-1) cout<<endentry<<" events processed"<<endl;
  
      if(isDalitz==1||isPassed==0) continue;
      if(N_j_30<2) continue;
      if(m_jj_30/1000<400) continue;
      if(DeltaEta_jj>-2&&DeltaEta_jj<2) continue;
      if(Zepp>5||Zepp<-5) continue;
      if(Category<11||Category>14) continue;
 
      float w = camp->second*xsec_kF_eff*weight/sumOfWeights;
      float wd = 1.;

      for(auto bin = OObins.begin(); bin != OObins.end(); bin++){
        double b_l = bin->second.first;
        double b_r = bin->second.second;
 
        if(oo1<b_l || oo1>b_r) continue;

        for(auto d = d_tildes.begin(); d != d_tildes.end(); d++){
          double d_tilde = d->second;
          if(isVBF) wd = 1. + WeightDtilde1*d_tilde + WeightDtilde2*d_tilde*d_tilde;

          hists[syst+"_"+d->first+"_"+bin->first]->Fill(m_yy, w*wd);
        }// end d_tilde
      }// end OO bin
    }// end fill
    delete f_w;
  }// end camp
}

enum para{MEAN, SIGMA, ALPHALo, NLo, ALPHAHi, NHi};

void getFitPara(map<TString, vector<double>> &para, vector<std::string> files, TString sys, map<TString, double> d_tilde, map<TString, pair<float, float>> bins){
  int id_VBF = 346214;
  int id_ggH = 343981;

  map<TString, TH1F*> hist_VBF;
  map<TString, TH1F*> hist_ggH;
  getMyyHist(hist_VBF, id_VBF, sys, files, bins, true, d_tilde);
  getMyyHist(hist_ggH, id_ggH, sys, files, bins, false, {{"SM", 0.}});

  for(auto bin = bins.begin(); bin != bins.end(); bin++){
    for(auto d = d_tilde.begin(); d != d_tilde.end(); d++){
      TH1F hist_sig = *(hist_ggH[sys+"_SM_"+bin->first]) + *(hist_VBF[sys+"_"+d->first+"_"+bin->first]);

      RooRealVar myy("m_yy","myy",105000,160000);

      RooDataHist dh_myy_sig("dh_myy_sig","dh_myy_sig", myy, Import(hist_sig));

      RooRealVar mean("mean","mean",120000,130000);
      RooRealVar sigma("sigma","sigma",100,10000);
      RooRealVar n1("n1","",0,30);
      RooRealVar alpha1("alpha1","",0,5);
      RooRealVar n2("n2","",0,55);
      RooRealVar alpha2("alpha2","",0,5);
      HggTwoSidedCBPdf DSCB_myy("sig","signal component",myy,mean,sigma,alpha1,n1,alpha2,n2);

      DSCB_myy.fitTo(dh_myy_sig);
      para[sys+"_"+d->first+"_"+bin->first].push_back(mean.getVal());
      para[sys+"_"+d->first+"_"+bin->first].push_back(sigma.getVal());
      para[sys+"_"+d->first+"_"+bin->first].push_back(alpha1.getVal());
      para[sys+"_"+d->first+"_"+bin->first].push_back(n1.getVal());
      para[sys+"_"+d->first+"_"+bin->first].push_back(alpha2.getVal());
      para[sys+"_"+d->first+"_"+bin->first].push_back(n2.getVal());
    }
  }
}

void getMag(){

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


  // file path list
  TString dirpath = "/scratchfs/bes/chenhr/atlaswork/VBF_CP/ntuples/sys";
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
      cout<<"f: "<<path_str+"/"+d+"/"+f<<endl;
      files.push_back(path_str+d+"/"+f);

    }
  }

  // get syst list
  TFile *f_in = new TFile("sample.root", "read");

  TTree *tree = (TTree*) f_in->Get("output");

  std::vector<TString> sysList;
  sysList.clear();

  getSysList(tree, "m_yy", sysList);

  delete f_in;

  vector<TString> ignoreList;
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

  map<TString, vector<double>> para_Nom;
  getFitPara(para_Nom, files, "Nominal", d_map, bins);

  map<TString, vector<double>> para_SysUp;
  map<TString, vector<double>> para_SysDown;

  map<TString,double> mu_up;
  map<TString,double> mu_down;
  map<TString,double> sigma_up;
  map<TString,double> sigma_down;

  vector<TString> calc_sysList;

  for(auto sys : sysList_noUD){
    if(sys.first=="Nominal") continue;
    cout<<"======="<<sys.first<<"========"<<endl;
    if(sys.second){
      getFitPara(para_SysUp, files, sys.first+"__1up", d_map, bins);
      getFitPara(para_SysDown, files, sys.first+"__1down", d_map, bins);
      for(auto bin = bins.begin(); bin != bins.end(); bin++){
        for(auto d = d_map.begin(); d != d_map.end(); d++){
          mu_up[sys.first+"_"+d->first+"_"+bin->first] = (para_SysUp[sys.first+"__1up"+"_"+d->first+"_"+bin->first][MEAN]-para_Nom["Nominal_"+d->first+"_"+bin->first][MEAN])/para_Nom["Nominal_"+d->first+"_"+bin->first][MEAN];
          mu_down[sys.first+"_"+d->first+"_"+bin->first] = (para_SysDown[sys.first+"__1down"+"_"+d->first+"_"+bin->first][MEAN]-para_Nom["Nominal_"+d->first+"_"+bin->first][MEAN])/para_Nom["Nominal_"+d->first+"_"+bin->first][MEAN];

          sigma_up[sys.first+"_"+d->first+"_"+bin->first] = (para_SysUp[sys.first+"__1up"+"_"+d->first+"_"+bin->first][SIGMA]-para_Nom["Nominal_"+d->first+"_"+bin->first][SIGMA])/para_Nom["Nominal_"+d->first+"_"+bin->first][SIGMA];
          sigma_down[sys.first+"_"+d->first+"_"+bin->first] = (para_SysDown[sys.first+"__1down"+"_"+d->first+"_"+bin->first][SIGMA]-para_Nom["Nominal_"+d->first+"_"+bin->first][SIGMA])/para_Nom["Nominal_"+d->first+"_"+bin->first][SIGMA];
        }
      }
    }else {
      getFitPara(para_SysUp, files, sys.first, d_map, bins);
      for(auto bin = bins.begin(); bin != bins.end(); bin++){
        for(auto d = d_map.begin(); d != d_map.end(); d++){
          mu_up[sys.first+"_"+d->first+"_"+bin->first] = (para_SysUp[sys.first+"_"+d->first+"_"+bin->first][MEAN]-para_Nom["Nominal_"+d->first+"_"+bin->first][MEAN])/para_Nom["Nominal_"+d->first+"_"+bin->first][MEAN];

          sigma_up[sys.first+"_"+d->first+"_"+bin->first] = (para_SysUp[sys.first+"_"+d->first+"_"+bin->first][SIGMA]-para_Nom["Nominal_"+d->first+"_"+bin->first][SIGMA])/para_Nom["Nominal_"+d->first+"_"+bin->first][SIGMA];
        }
      }
    }

    calc_sysList.push_back(sys.first);

    //if((int)calc_sysList.size()>0) break;
  }// end syst


  // fill csv file
  ofstream ofpara_Nom("csv/para_Nom.csv", ios::out);
  if(!ofpara_Nom){
    ofpara_Nom.close();
    cout<<"error can't open file for record"<<endl;
  }
  for(auto p = para_Nom.begin(); p!=para_Nom.end(); p++){
    ofpara_Nom<<p->first<<","<<p->second[MEAN]<<","<<p->second[SIGMA]<<","<<p->second[ALPHALo]<<","<<p->second[NLo]<<","<<p->second[ALPHAHi]<<","<<p->second[NHi]<<endl;
  }
  ofpara_Nom.close();

 for(auto bin = bins.begin(); bin != bins.end(); bin++){
   for(auto d = d_map.begin(); d != d_map.end(); d++){
      ofstream ofmu("csv/mu_"+d->first+"_"+bin->first+".csv", ios::out);
      if(!ofmu){
        ofmu.close();
        cout<<"error can't open file for record"<<endl;
      }
    
      ofstream ofsigma("csv/sigma_"+d->first+"_"+bin->first+".csv", ios::out);
      if(!ofsigma){
        ofsigma.close();
        cout<<"error can't open file for record"<<endl;
      }
    
      for(auto sys : calc_sysList){
        if(sysList_noUD[sys]) {
          ofmu<<sys<<","<<mu_up[sys+"_"+d->first+"_"+bin->first]<<","<<mu_down[sys+"_"+d->first+"_"+bin->first]<<endl;
          ofsigma<<sys<<","<<sigma_up[sys+"_"+d->first+"_"+bin->first]<<","<<sigma_down[sys+"_"+d->first+"_"+bin->first]<<endl;
        }
        else{
          ofmu<<sys<<","<<mu_up[sys+"_"+d->first+"_"+bin->first]<<endl;
          ofsigma<<sys<<","<<sigma_up[sys+"_"+d->first+"_"+bin->first]<<endl;
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
