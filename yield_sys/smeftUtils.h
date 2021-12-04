//#include <iostream>
//#include <iterator>
//#include <string>
//#include <regex>
// 
//void match()
//{
//    std::string s = "Some people, when confronted with a problem, think "
//        "\"I know, I'll use regular expressions.\" "
//        "Now they have two problems.";
// 
//    std::regex self_regex("REGULAR EXPRESSIONS",
//            std::regex_constants::ECMAScript | std::regex_constants::icase);
//    if (std::regex_search(s, self_regex)) {
//        std::cout << "Text contains the phrase 'regular expressions'\n";
//    }
// 
//    std::regex word_regex("(\\w+)");
//    auto words_begin = 
//        std::sregex_iterator(s.begin(), s.end(), word_regex);
//    auto words_end = std::sregex_iterator();
// 
//    std::cout << "Found "
//              << std::distance(words_begin, words_end)
//              << " words\n";
// 
//    const int N = 6;
//    std::cout << "Words longer than " << N << " characters:\n";
//    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
//        std::smatch match = *i;
//        std::string match_str = match.str();
//        if (match_str.size() > N) {
//            std::cout << "  " << match_str << '\n';
//        }
//    }
// 
//    std::regex long_word_regex("(\\w{7,})");
//    std::string new_s = std::regex_replace(s, long_word_regex, "[$&]");
//    std::cout << new_s << '\n';
//}

#include <regex>

using namespace std;

void regMatch(string istr, string sreg, std::vector<float> &varVec){
  std::regex reg(sreg);

  /*std::regex reg("([a-zA-Z]+):\\s(-?0\\.[0-9]+)\\s,");
  std::smatch m;
  auto ret = std::regex_match(istr, m, reg);
  for(auto mi : m){
    cout<<mi<<endl;
  }

  std::regex reg("\\s,\\s");
  auto ret = std::regex_replace(istr, reg, " ");
  cout<< ret << endl;*/

  std::sregex_iterator pos(istr.cbegin(), istr.cend(), reg);
  decltype(pos) end;
  for (; pos != end; ++pos){
    varVec.push_back(atof(pos->str().data()));  //cout<<pos->str()<<endl;
  }

}

void regToken(string istr, string stok, std::vector<string> &varVec){
  std::regex reg(stok);

  std::sregex_token_iterator pos(istr.cbegin(), istr.cend(), reg, -1);
  decltype(pos) end;
  for (; pos != end; ++pos){
    varVec.push_back(pos->str().data()); // cout<<pos->str()<<endl;
  }

}

float getIPattern(string line, int ibin, string sreg = "\\s(-?[0-9]+\\.[0-9]+(e-[0-9]+)?)|\\s(0)\\s|\\s(0)$"){
  string istr = "Up: 0 , 2.58684e-05 , 0 , 0.000126958 , -0.00305438 , 0";
  std::vector<float> varVec;
  //regMatch(istr, sreg, varVec); // for(auto var : varVec) cout<<var<<endl;
  regMatch(line, sreg, varVec); // for(auto var : varVec) cout<<var<<endl;
  return varVec.at(ibin);
}

string getTok(string line, int ibin, string stok = ","){
  std::vector<string> varVec;
  regToken(line, stok, varVec); // for(auto var : varVec) cout<<var<<endl;
  return varVec.at(ibin);
}

float getVariKF(string sys = "JET_JER_DataVsMC_MC16", string cat = "LL", string cHW = "-0.5", int bin = 5, string ud = "Up"){
  string path = "/publicfs/atlas/atlasnew/higgs/hgg/chenhr/vbfcp/syst/theory/kfactor_SMEFT_fine_updatedBDT/"+cat+"_"+sys+"_cHW_"+cHW+".txt";

  fstream inFile;
  inFile.open(path);
  if( ! inFile.is_open())
  {
    cout<<path<<endl;
    cout<<"can not open cfg file! returning 0"<<endl;
    return 0.;
  }
  char tmp[1000];
  while(!inFile.eof())
  {
    inFile.getline(tmp,1000);
    string line(tmp); //cout<<line<<endl;
    if(line.find(ud)==std::string::npos) continue;
    return getIPattern(line, bin);
  }
  return 0.;
}

bool getVariY(TString fpath, map<TString, pair<float, float>> &sysud){
//void getVariY(TString cat = "LL_b1"){
  /*std::vector<TString> sys;
  std::vector<float> vu;
  std::vector<float> vd;*/
 
  ifstream file;
  file.open(fpath.Data());
  if( ! file.is_open())
  {
      cout<<"can not open file! "<<fpath<<endl;
      return false;
  }
  char tmp[1000];
  while(!file.eof()){
    file.getline(tmp,1000);
    string line(tmp);
    size_t ptmp;

    if(line == "") continue;

    TString sys = "";
    size_t pos = line.find(','); //std::cout<<line.substr(0,pos)<<std::endl;
    if(pos!=string::npos) sys = line.substr(0,pos).data();
    else continue;
 
    /*ptmp = pos;
    pos = line.find(',', pos+1); cout<<line.substr(ptmp+1, pos-ptmp-1)<<endl;;
    if(pos!=string::npos && line != "") vu.push_back(atof(line.substr(ptmp+1, pos-ptmp-1).data()));
    else vu.push_back(0.);
 
    ptmp = pos;
    pos = line.find(',', pos+1); cout<<line.substr(ptmp+1, pos-ptmp-1)<<endl;;
    if(line != "") vd.push_back(atof(line.substr(ptmp+1, pos-ptmp-1).data()));
    else vd.push_back(0.);*/

    float utmp = atof(getTok(line, 1).data()); //cout<<utmp<<endl;
    float dtmp = atof(getTok(line, 2).data()); //cout<<dtmp<<endl;
    sysud[sys] = std::make_pair(utmp, dtmp); //cout<<sys<<": "<<utmp<<","<<dtmp<<endl;
  }
  return true;
}

float qdrYK(float yv, float kv, float prun = 10){
  float ayv = std::abs(yv);
  float akv = std::abs(kv);
  if(kv==0 || ayv/akv>prun) return yv;
  if(yv==0 || ayv/akv<1/prun) return kv;

  float sqSum = yv*ayv + kv*akv;
  float sqrYK = sqSum >= 0. ? std::sqrt(sqSum) : -(std::sqrt(-sqSum));

  return sqrYK;
}
