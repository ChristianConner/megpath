#ifndef THREADED__H
#define THREADED__H

#include "Analysis.h"
#include "ThreadedMonteAnneal.h"

using namespace std;

//thinking that the global state should be declaired outside of class say here:

class Threaded:public Analysis{
	public:
		Threaded(int nt);
		virtual bool annealCallback(int iterations);
		virtual void annealPrintCallback(int iterations);
		virtual void montePrintCallback(int iterations);
		virtual void start(string filename);
		virtual void run();
		virtual void stop();
	protected:
		int numThreads;
};

#endif
