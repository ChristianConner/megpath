//Parallel Patterns
//Matthew Dyer
//Created on : 6/9/2017
//Last Modified 6/12/2017

#ifndef PARALLELPATTERNS__H
#define PARALLELPATTERNS__H

#include "Distributed.h"
#include <mpi.h>
#include <unistd.h>
#include <algorithm>

using namespace std;

class ParallelPatterns: virtual public Distributed{
	public:
		ParallelPatterns();
		void startSplit();
		virtual void allAverage(NMFMatrix& mat,MPI_Comm Comm);
		virtual void start();	
		virtual void run();
		virtual void stop();
		virtual void gatherCoefficients();
		virtual void monteCallback(int iter);	
		virtual bool annealCallback(int iter);
		virtual void monteFinalCallback();
		virtual void annealFinalCallback();
	protected:
		int count;
		int disp;
		int bufferSize;
		double* sendBuffer;
		MatrixXd oexpression;
		int startPoint;
};


#endif
