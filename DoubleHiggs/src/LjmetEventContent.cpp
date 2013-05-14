/*
  Container class for passing internal event content

   Author: Gena Kukartsev, 2012
*/



#include "interface/LjmetEventContent.h"
#include "TH1.h"
#include "TDirectory.h"


LjmetEventContent::LjmetEventContent():
  mName("LjmetEventContent"),
  mLegend("[LjmetEventContent]: "),
  mpTree(0),
  mFirstEntry(true),
  mVerbosity(0){
}



LjmetEventContent::~LjmetEventContent(){

  // save tree
  mpTree->Write();

  // save histograms
  std::map<std::string,std::map<std::string,LjmetEventContent::HistMetadata> >::iterator iMod;
  std::map<std::string,LjmetEventContent::HistMetadata>::iterator iHist;
  TDirectory * rootDir = mpTree->GetDirectory();
  for (iMod=mDoubleHist.begin();iMod!=mDoubleHist.end();++iMod){
    TDirectory * _dir = 0;
    _dir = rootDir->mkdir( iMod->first.c_str() );
    if (!_dir) rootDir->GetDirectory( iMod->first.c_str() );
    for (iHist=iMod->second.begin();iHist!=iMod->second.end();++iHist){
      std::cout << mLegend
		<< "Saving " << iMod->first << "/"
		<< iHist->second.GetName() << std::endl;
      _dir->cd();
      iHist->second.GetHist( )->SetDirectory(_dir);
      iHist->second.GetHist( )->Print();
      iHist->second.GetHist( )->Write();
    }
  }

}



void LjmetEventContent::SetTree(TTree * tree){
mpTree = tree;
}



void LjmetEventContent::SetValue(std::string key, bool value){
  mBoolBranch[key] = value;
}



void LjmetEventContent::SetValue(std::string key, int value){
  mIntBranch[key] = value;
}



void LjmetEventContent::SetValue(std::string key, double value){
  mDoubleBranch[key] = value;
}

void LjmetEventContent::SetValue(std::string key, std::vector<int> value){
  mVectorIntBranch[key] = value;
}

void LjmetEventContent::SetValue(std::string key, std::vector<double> value){
  mVectorDoubleBranch[key] = value;
}



void LjmetEventContent::Fill(){
  if (mFirstEntry){
    createBranches();
    mFirstEntry = false;
  }
  mpTree->Fill();


  // fill histograms
  // create histograms
  std::map<std::string,std::map<std::string,LjmetEventContent::HistMetadata> >::iterator iMod;
  std::map<std::string,LjmetEventContent::HistMetadata>::iterator iHist;
  for (iMod=mDoubleHist.begin();iMod!=mDoubleHist.end();++iMod){
    for (iHist=iMod->second.begin();iHist!=iMod->second.end();++iHist){
      TH1 * _hist = iHist->second.GetHist();
      if (_hist) _hist->Fill(iHist->second.GetValue());
    }
  }


  return;
}



int LjmetEventContent::createBranches(){
  //
  // create branches in the tree according to maps
  //

  mLegend = "["+mName+"]: ";
  std::cout << mLegend << "Creating branches in output tree" << std::endl;


  //std::cout << mLegend << "Creating histograms: "
  //	    << mDoubleHist.size() << std::endl;
  //std::cout << mLegend << "Creating histograms: "
  //	    << mDoubleHist["StopEventSelector"].size() << std::endl;


  // boolean branches
  for(std::map<std::string,bool>::iterator br = mBoolBranch.begin();
      br != mBoolBranch.end();
      ++br){
    std::string name_type = br->first+"/O";
    mpTree -> Branch(br->first.c_str(),
		   &(br->second),
		   name_type.c_str());

    if (mVerbosity>0){
      std::cout << mLegend << "Branch " << name_type
		<< " created" << std::endl;    
    }
  }
  std::cout << mLegend << "boolean branches created: " 
	    << mBoolBranch.size() << std::endl;


  // integer branches
  for(std::map<std::string,int>::iterator br = mIntBranch.begin();
      br != mIntBranch.end();
      ++br){
    std::string name_type = br->first+"/I";
    mpTree -> Branch(br->first.c_str(),
		   &(br->second),
		   name_type.c_str());

    if (mVerbosity>0){
      std::cout << mLegend << "Branch " << name_type
		<< " created" << std::endl;    
    }
  }
  std::cout << mLegend << "integer branches created: " 
	    << mIntBranch.size() << std::endl;


  // double branches
  for(std::map<std::string,double>::iterator br = mDoubleBranch.begin();
      br != mDoubleBranch.end();
      ++br){
    std::string name_type = br->first+"/D";
    mpTree -> Branch(br->first.c_str(),
		   &(br->second),
		   name_type.c_str());

    if (mVerbosity>0){
      std::cout << mLegend << "Branch " << name_type
		<< " created" << std::endl;    
    }
  }
  std::cout << mLegend << "double precision branches created: " 
	    << mDoubleBranch.size() << std::endl;


  // vector int branches
  for(std::map<std::string, std::vector<int> >::iterator br = mVectorIntBranch.begin();
      br != mVectorIntBranch.end();
      ++br){
    std::string name_type = br->first+" std::vector<int>";
    mpTree -> Branch(br->first.c_str(), 
		     &(br->second));

    if (mVerbosity>0){
      std::cout << mLegend << "Branch " << name_type
		<< " created" << std::endl;    
    }
  }
  std::cout << mLegend << "vector<int> branches created: " 
	    << mVectorIntBranch.size() << std::endl;


  // vector double branches
  for(std::map<std::string, std::vector<double> >::iterator br = mVectorDoubleBranch.begin();
      br != mVectorDoubleBranch.end();
      ++br){
    std::string name_type = br->first+" std::vector<double>";
    mpTree -> Branch(br->first.c_str(), 
		     &(br->second));

    if (mVerbosity>0){
      std::cout << mLegend << "Branch " << name_type
		<< " created" << std::endl;    
    }
  }
  std::cout << mLegend << "vector<double> branches created: " 
	    << mVectorDoubleBranch.size() << std::endl;


  return 0;
}



void
LjmetEventContent::SetHistogram(std::string modname, std::string histname,
				int nbins, double low, double high){
  //
  // create histogram entry in event content,
  // so it is created by the LjmetFactory
  //

  mLegend = "["+mName+"]: ";

  //mDoubleHist[modname][histname]

  std::map<std::string,std::map<std::string,HistMetadata> >::iterator iMod;
  std::map<std::string,HistMetadata>::iterator iHist;


  iMod = mDoubleHist.lower_bound(modname);

  if(iMod != mDoubleHist.end() && !(mDoubleHist.key_comp()(modname, iMod->first))){
    // key already exists
    // update iMod->second
    
    iHist = iMod->second.lower_bound(histname);
    if(iHist != iMod->second.end() && !(iMod->second.key_comp()(histname, iHist->first))){
      // key already exists
      // update iHist->second - not needed
      std::cout << mLegend
		<< "Histogram " << modname << "/" << histname
		<< " is already set" << std::endl;
    }
    else{
      // the key does not exist in the map
      // add it to the map

      iMod->second.insert(
			  iHist,
			  std::pair<std::string,HistMetadata>(histname,
							      HistMetadata(histname,
									   nbins,
									   low,
									   high)));
    }
  }
  
  else{
    // the key does not exist in the map
    // add it to the map
    std::map<std::string,HistMetadata> _map;
    _map.insert(std::pair<std::string,HistMetadata>(histname,
						    HistMetadata(histname,
								 nbins,
								 low,
								 high)));
    mDoubleHist.insert(iMod, 
		       std::pair<std::string,std::map<std::string,HistMetadata> >(modname, _map));
		       // Use iMod as a hint to insert,
		       // so it can avoid another lookup
		       
  }


  return;
}



void
LjmetEventContent::SetHistValue(std::string modname,
				std::string histname,
				double value){
  //
  // assign current hist value to hist metadata collection
  //

  mLegend = "["+mName+"]: ";

  std::map<std::string,std::map<std::string,HistMetadata> >::iterator iMod;
  std::map<std::string,HistMetadata>::iterator iHist;


  iMod = mDoubleHist.lower_bound(modname);

  if(iMod != mDoubleHist.end() && !(mDoubleHist.key_comp()(modname, iMod->first))){
    // key already exists
    // update iMod->second
    
    iHist = iMod->second.lower_bound(histname);
    if(iHist != iMod->second.end() && !(iMod->second.key_comp()(histname, iHist->first))){
      // key already exists
      // update iHist->second - not needed
      iHist->second.SetValue(value);
    }
    else{
      // the key does not exist in the map
      // add it to the map
      std::cout << mLegend << "Cannot set value, histogram "
		<< histname << " in module " << modname
		<< " does not exist" << std::endl;
    }
  }
  
  else{
    // the key does not exist in the map
    // add it to the map
    std::cout << mLegend << "Cannot set value, histogram "
	      << modname << "/" << histname
	      << " does not exist" << std::endl;
  }


  return;
}
