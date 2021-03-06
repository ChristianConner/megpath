//HistoPF.h -- Histogram Based Probability Function
//Julian Dymacek
//Matthew Dyer
//Created 5/24/2017
//Modified on: 6/6/2017
#ifndef HISTOPF__H
#define HISTOPF__H

#include <vector>
#include <random>
#include <sstream>
#include <string.h>
#include "ProbFunc.h"

using namespace std;

class HistoPF: public ProbFunc{
	public:
		HistoPF();
		double random();
		void addObservation(double d);
		string toString();
		vector<double> getVector();
		void setVector(vector<double> vec);
		void toDoubles(double* buffer);
		void fromDoubles(double* buffer);
		int dataSize();	
	private:
		piecewise_linear_distribution<double> dist;
		vector<double> intervals;
		vector<double> weights;
};

#endif
