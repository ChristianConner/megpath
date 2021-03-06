#ifndef DISTRIBUTED__H
#define DISTRIBUTED__H

#include <mpi.h>
#include <unistd.h>
#include "Analysis.h"

using namespace std;

class Distributed: public Analysis{
	public:
		Distributed();
		virtual ~Distributed();
		virtual void start();
		virtual void run();
		virtual void stop();
		virtual void montePrintCallback(int iter);
		virtual void annealPrintCallback(int iter);
		virtual void timedRun(int runTime);
		void sendLeastError(int process, double formerError);
		void sendMatrix(MatrixXd& matrix,int dest);
		void recvMatrix(MatrixXd& matrix,int src);
		int broadcastInt(int tosend);
		string hostname;
	protected:
        int rank;
        int systemSize;
};

#endif
