//Main file
//Matthew Dyer
//Julian Dymacek
//Created on 5/25/2017
//Last modified: 7/7/2017

#include "DistNaive.h"
#include "FuncThrow.h"
#include "ParallelPatterns.h"
#include "ParallelFuncThrow.h"
#include "PatternMatching.h"

using namespace std;

int main(int argc, char** argv){
	if(argc < 3){
		cerr << "Needs an argument file and analysis to run!";
		return 0;
	}

	string argFile = argv[1];
	string analysis = argv[2];
	
	Analysis* a;

	if(analysis == "DistNaive" || analysis == "DN" || analysis == "dn"){
		a = new DistNaive();
	}else if(analysis == "FuncThrow" || analysis == "FT" || analysis == "ft"){
		a = new FuncThrow();
	}else if(analysis == "ParallelPatterns" || analysis == "ParPat" || analysis == "PP" || analysis == "pp"){
		a = new ParallelPatterns();
	}else if(analysis == "ParallelFuncThrow" || analysis == "ParFuncThrow" || analysis == "PFT" || analysis == "pft"){
		a = new ParallelFuncThrow();
	}else if(analysis == "PatternMatching" || analysis == "PatMatch" || analysis == "PM" || analysis == "pm"){
		a = new PatternMatching();
	}else if(analysis == "Test" || analysis == "test"){
		a = new Distributed();
	}

	a->load(argFile);
	a->start();

	a->run();
	a->stop();
	cout << "Total program running time: " << a->state->time << endl;
	return 0;
}

