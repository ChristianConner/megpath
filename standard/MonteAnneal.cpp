//StandardAnalysis
//Matthew Dyer
//Julian Dymacek
//Created on 6/6/2017

#include "MonteAnneal.h"

bool MonteAnneal::accept(double de,double t){
    return de < 0 ||  exp(-de/t) > uniform->random();
}




void MonteAnneal::monteCarloStep(NMFMatrix& m,ErrorFunction* ef){
	double oldError = 0;
	double error = 0; 
	 for(int y =0; y < m.rows; ++y){
        for(int x =0; x < m.columns; ++x){
            ProbFunc* function = m.functions[y][x];
			double r = function->random();
            if(function->size() == 1){
				oldError = ef->fastError();
                m.matrix(y,x) = r;
				error = ef->error();
            }else{
				oldError = ef->error();
                for(int k=0; k < function->size(); ++k){
                    Entry e = function->getEntry(k);
                    m.matrix(e.y,e.x) = e.val;
                }
				error = ef->error();
            }
            if(error <= oldError){
                function->addObservation(r);
            }
        }
    }
}

void MonteAnneal::annealStep(NMFMatrix& m, double t,ErrorFunction* ef){
    vector<Entry> entries;
    entries.push_back({0,0,0});
	double olderror = 0;
	double error = 0;

    for(int y =0; y < m.rows; y++){
        for(int x =0; x < m.columns; x++){
            double olderror = findError(m,y,x);

            ProbFunc* function = m.functions[y][x];
            double r = function->random();
            if(function->size() == 1){
				olderror = ef->fastError();
                entries[0].x = x;
                entries[0].y = y;
                entries[0].val = m.matrix(y,x);
                m.matrix(y,x) = r;
				error = ef->error();
            }else{
                while(entries.size() < function->size()){
                    entries.push_back({0,0,0});
                }
				olderror = ef->error();
                for(int k=0; k < function->size(); ++k){
                    Entry e = function->getEntry(k);
                    double old = m.matrix(e.y,e.x);
                    m.matrix(e.y,e.x) = e.val;
                    e.val = old;
                    entries[k] = e;
                }
				error = ef->error();
            }
            if(!accept(error-olderror,t)){
                for(int i =0; i < function->size(); ++i){
                    m.matrix(entries[i].y,entries[i].x) = entries[i].val;
                }
            }
        }
    }
}

/*Run a monte carlo markov chain*/
void MonteAnneal::monteCarlo(){
	Stopwatch watch;
	watch.start();
	ErrorFunctionRow efRow(state)
	ErrorFunctionCol efCol(state);

	//For each spot take a gamble and record outcome
	for(int i =0; i < state->MAX_RUNS; i++){
		monteCarloStep(state->patterns,&efRow);
		monteCarloStep(state->coefficients,&efCol);
		if(i % 1000 == 0){
			double error = efRow.error();
			cout << i << "\t Error = " << error << "\t Time = " << watch.formatTime(watch.lap()) << endl;
		}
	}
	cout << "Final Error: " << efRow.error() << endl;
	cout << "Error Histogram: " << efRow.errorDistribution(10) << endl;	
	cout << "Total time: " << watch.formatTime(watch.stop()) << endl;
}

void MonteAnneal::anneal(){
	Stopwatch watch;
	int ndx = 0;
	double t = 0.5;
	watch.start();

    ErrorFunctionRow efRow(state)
    ErrorFunctionCol efCol(state);

	double formerError = 2*start->expression.rows()*start->expression.cols();
	bool running = true;
	while(running && ndx < MAX_RUNS){
		annealStep(start->coefficients,t,&efCol);
		annealStep(start->patterns,t,&efRow);
		if(ndx % 1000 == 0){
			double error = efRow.findError();
			cout << ndx << "\t Error = " << error << "\t Time = " << watch.formatTime(watch.lap()) << endl;
			if(abs(formerError - error) < 0.005 && error < formerError)
				running = false;
			formerError = error;
		}
		ndx++;
		t *= 0.99975;
	}
	cout << "Final Error: " << efRow.findError() << endl;
	cout << "Error Histogram: " << efRow.errorDistribution(10) << endl;
	cout << "Total time: " << watch.formatTime(watch.stop()) << endl;
}

void MonteAnneal::run(){
	//Could put stop watch in here
	ProbFunc::generator.seed(time(0));
	monteCarlo();
	anneal();		
}

void MonteAnneal::stop(){
	state->patterns.write(analysis + "patterns.csv");
	state->coefficients.write(analysis + "coefficients.csv");

	ofstream fout;
	fout.open(state->analysis + "expression.txt");
	fout << state->coefficients.matrix*state->patterns.matrix;
	fout.close();
}
