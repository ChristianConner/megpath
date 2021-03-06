//Parallelized Function Thrower
//Matthew Dyer
//Created on 6/22/2017
//Last Modified: 6/22/2017

#ifndef PARALLELFUNCTHROW__H
#define PARALLELFUNCTHROW__H

#include "PatternMatching.h"
#include "FuncThrow.h"
#include "Distributed.h"

using namespace std;

class ParallelFuncThrow: public Distributed{
	public:
		ParallelFuncThrow();
		virtual void monteCallback(int iter);
		virtual bool annealCallback(int iter);
		virtual void start();
		virtual void run();
		virtual void stop();
	private:
		FuncThrow* ft;
		PatternMatching* pp;

};


#endif
