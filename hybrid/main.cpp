//Main file
//Matthew Dyer
//Julian Dymacek
//Created on 5/25/2017
//Last modified: 7/7/2017

#include "Stopwatch.h"
#include "Hybrid.h"
using namespace std;

int main(int argc, char** argv){
	if(argc < 4){
		cerr << "Needs an argument file and analysis to run!";
		return 0;
	}
	Stopwatch watch;
	string argFile = argv[1];
	string analysis = argv[2];
	int nt = atoi(argv[3]);
	
	Analysis* a;

	if(analysis == "Hybrid" || analysis == "H" || analysis == "h"){
		a = new Hybrid(nt);
	}
	a->start(argFile);

	watch.start();	
	a->run();
	a->state->time = watch.formatTime(watch.stop());
	cout << "Total program running time: " << a->state->time << endl;

	a->stop();

	return 0;
}
