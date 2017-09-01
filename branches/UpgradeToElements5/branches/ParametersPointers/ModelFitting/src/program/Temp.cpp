/** 
 * @file Temp.cpp
 * @date August 24, 2015
 * @author Nikolaos Apostolakos
 */

#include <iostream>
#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Parameters/DependentParameter.h"
#include "ModelFitting/Parameters/EngineParameter.h"
#include "ModelFitting/Parameters/NormalizedConverter.h"
#include "ModelFitting/Engine/EngineParameterManager.h"
#include "ModelFitting/Models/SersicProfileProjection.h"

using namespace std;
using namespace ModelFitting;

int main() {
  
//  ManualParameter par {5.};
//  auto obs1 = par.addObserver([](double){cout << "Observer 1\n";});
//  cout << "Obs1 ID " << obs1 << '\n';
//  auto obs2 = par.addObserver([](double){cout << "Observer 2\n";});
//  cout << "Obs2 ID " << obs2 << '\n';
//  
//  par.setValue(4.5);
//  
//  if (par.removeObserver(obs1))
//    cout << "First\n";
//  
//  if (par.removeObserver(obs1))
//    cout << "Second\n";
//  
//  par.setValue(3);
  
  
  
//  shared_ptr<ManualParameter> par1 {new ManualParameter(5.)};
//  shared_ptr<ManualParameter> par2 {new ManualParameter(3.)};
//  auto calc = [](double p1, double p2){return p1 + p2;};
////  DependentParameter<ManualParameter, ManualParameter> dp {calc, par1, par2};
//  {
//    auto dp = createDependentParameter(calc, par1, par2);
//  
//    cout << par1->getValue() << '\t' << par2->getValue() << '\t' << dp->getValue() << '\n';
//    par1->setValue(2.);
//    cout << par1->getValue() << '\t' << par2->getValue() << '\t' << dp->getValue() << '\n';
//  }
//  cout << par2->getValue() << '\n';
//  par2->setValue(10.);
//  cout << par2->getValue() << '\n';
  
  
//  std::shared_ptr<EngineParameter> par1 {
//    new EngineParameter{2., std::unique_ptr<CoordinateConverter>{new NormalizedConverter{4.}}}
//  };
//  std::shared_ptr<EngineParameter> par2 {
//    new EngineParameter{2., std::unique_ptr<CoordinateConverter>{new NormalizedConverter{6.}}}
//  };
//  
//  EngineParameterManager manager {};
//  manager.registerParameter(par1);
//  manager.registerParameter(par2);
//  
//  std::cout << "Par1: " << par1->getValue() << '\t' << par1->getEngineValue() << '\n';
//  std::cout << "Par2: " << par2->getValue() << '\t' << par2->getEngineValue() << '\n';
//  
//  vector<double> v {0.,0.};
//  manager.getEngineValues(v.begin());
//  std::cout << v[0] << ' ' << v[1] << '\n';
//  v[0] = 1;
//  v[1] = 1;
//  manager.updateEngineValues(v.begin());
//  
//  std::cout << "Par1: " << par1->getValue() << '\t' << par1->getEngineValue() << '\n';
//  std::cout << "Par2: " << par2->getValue() << '\t' << par2->getEngineValue() << '\n';
  
  shared_ptr<ManualParameter> i0 {new ManualParameter(1.)};
  shared_ptr<ManualParameter> n {new ManualParameter(4)};
  shared_ptr<ManualParameter> k {new ManualParameter(7.66924944)};
  
  std::unique_ptr<SersicProfileProjection> proj1 {new SersicProfileProjection{i0, n, k}};
  i0.reset();
  SersicProfileProjection proj2 {*proj1};
  
  cout << "Proj1 : " << (*proj1)(0) << ' ' << (*proj1)(0.01) << '\n';
  cout << "Proj2 : " << proj2(0) << ' ' << proj2(0.01) << '\n';
  
  proj1.reset();
  n->setValue(1.);
  k->setValue(1.);
  
//  cout << "Proj1 : " << proj1(0) << ' ' << proj1(0.01) << '\n';
//  cout << "Proj1 : " << (*proj1)(0) << ' ' << (*proj1)(0.01) << '\n';
  cout << "Proj2 : " << proj2(0) << ' ' << proj2(0.01) << '\n';
  
}