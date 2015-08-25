/** 
 * @file Temp.cpp
 * @date August 24, 2015
 * @author Nikolaos Apostolakos
 */

#include <iostream>
#include "ModelFitting/Parameters/ManualParameter.h"
#include "ModelFitting/Parameters/EngineParameter.h"
#include "ModelFitting/Parameters/NormalizedConverter.h"
#include "ModelFitting/Parameters/ReferenceUpdater.h"
#include "ModelFitting/Parameters/DependentParameter.h"
#include "ModelFitting/Engine/EngineParameterManager.h"
#include "ModelFitting/Models/SercicProfileProjection.h"


using namespace std;
using namespace ModelFitting;

int main() {
  
//  ManualParameter par1 {5.};
//  
//  ManualParameter par {par1};
//  auto obs1 = par.addObserver([](double v){cout << "Observer 1" << ": " << v <<"\n";});
//  cout << "Obs1 ID " << obs1 << '\n';
//  auto obs2 = par.addObserver([](double v){cout << "Observer 2" << ": " << v <<"\n";});
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
//  par.setValue(3.1);
//  par1.setValue(1.1);
  
//  EngineParameter par1 {5., std::unique_ptr<CoordinateConverter>{new NormalizedConverter{5.}}};
//  par1.addObserver([](double v){cout << "Observer: " << v <<"\n";});
//  
//  cout << par1.getValue() << ' ' << par1.getEngineValue() << '\n';
//  
//  EngineParameter par2 {par1};
//  par2.setEngineValue(2.);
//  cout << par1.getValue() << ' ' << par1.getEngineValue() << '\n';
//  
//  double v = par1.getValue();
//  cout << "V " << v << '\n';
//  ReferenceUpdater updater (par1, v);
//  par1.setEngineValue(1.);
//  cout << "V " << v << '\n';
  
  
  ManualParameter par1 {5.};
  ManualParameter par2 {3.};
  auto calc = [](double p1, double p2){return p1 + p2;};
  {
    auto dp1 = createDependentParameter(calc, par1, par2);
//    DependentParameter<ManualParameter,ManualParameter> dp {std::move(dp1)};
    DependentParameter<ManualParameter,ManualParameter> dp {dp1};
//    auto dp = dp1;
//    DependentParameter<ManualParameter,ManualParameter> dp {calc, par1, par2};
  
    cout << par1.getValue() << '\t' << par2.getValue() << '\t' << dp.getValue() << '\n';
    par1.setValue(2.);
    cout << par1.getValue() << '\t' << par2.getValue() << '\t' << dp.getValue() << '\n';
  }
  cout << par2.getValue() << '\n';
  par2.setValue(10.);
  cout << par2.getValue() << '\n';
  
  
//  EngineParameter par1 {2., std::unique_ptr<NormalizedConverter>{new NormalizedConverter{4.}}};
//  EngineParameter par2 {2., std::unique_ptr<CoordinateConverter>{new NormalizedConverter{6.}}};
//  
//  EngineParameterManager manager {};
//  manager.registerParameter(par1);
//  manager.registerParameter(par2);
//  
//  std::cout << "Par1: " << par1.getValue() << '\t' << par1.getEngineValue() << '\n';
//  std::cout << "Par2: " << par2.getValue() << '\t' << par2.getEngineValue() << '\n';
//  
//  double v[] = {0.,0.};
//  manager.getEngineValues(v);
//  std::cout << v[0] << ' ' << v[1] << '\n';
//  v[0] = 1;
//  v[1] = 1;
//  manager.updateEngineValues(v);
//  
//  std::cout << "Par1: " << par1.getValue() << '\t' << par1.getEngineValue() << '\n';
//  std::cout << "Par2: " << par2.getValue() << '\t' << par2.getEngineValue() << '\n';
  
//  std::unique_ptr<ManualParameter> par {new ManualParameter{5.}};
//  double v = 0.;
//  cout << v << '\n';
//  ReferenceUpdater upd (*par, v);
//  cout << upd.isParameterAlive() << ' ' << v << '\n';
//  par->setValue(6.);
//  cout << upd.isParameterAlive() << ' ' << v << '\n';
//  par->setValue(7.);
//  cout << upd.isParameterAlive() << ' ' << v << '\n';
//  par.reset();
//  cout << upd.isParameterAlive() << ' ' << v << '\n';
  
  
//  std::unique_ptr<ManualParameter> i0 {new ManualParameter{1.}};
//  ManualParameter n {4};
//  ManualParameter k {7.66924944};
//  
//  std::unique_ptr<SersicProfileProjection> proj1 {new SersicProfileProjection{*i0, n, k}};
//  i0.reset();
//  SersicProfileProjection proj2 {*proj1};
//  
//  cout << "Proj1 : " << (*proj1)(0) << ' ' << (*proj1)(0.01) << '\n';
//  cout << "Proj2 : " << proj2(0) << ' ' << proj2(0.01) << '\n';
//  
//  proj1.reset();
//  n.setValue(1.);
//  k.setValue(1.);
//  
////  cout << "Proj1 : " << proj1(0) << ' ' << proj1(0.01) << '\n';
////  cout << "Proj1 : " << (*proj1)(0) << ' ' << (*proj1)(0.01) << '\n';
//  cout << "Proj2 : " << proj2(0) << ' ' << proj2(0.01) << '\n';
  
}