#ifndef DISTNAIVE__H
#define DISTNAIVE__H

#include "MonteAnneal.h"
#include <mpi.h>
#include <unistd.h>

using namespace std;

class DistNaive: public Distributed{
	public:
		DistNaive();
		virtual void start(string filename);
		virtual void run();
		virtual void stop();
};


#endif
