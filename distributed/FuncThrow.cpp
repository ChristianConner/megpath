
FuncThrow::FuncThrow(): MonteAnneal(){}


void FuncThrow::start(string filename){
	MonteAnneal::start(filename);
	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &process);
    char hostbuff[100];
    gethostname(hostbuff,99);
    hostname = string(hostbuff);

}


void FuncThrow::monteCarlo(){
	int tag = 0;
	int flag = 0;
	MPI_Status status;
	int bufferSize = state->patterns.size()+1;
	double* buffer = new double[bufferSize];
    Stopwatch watch;
    watch.start();


    ErrorFunctionRow efRow(state);
    ErrorFunctionCol efCol(state);

    //For each spot take a gamble and record outcome
    for(int i =0; i < state->MAX_RUNS; i++){
        monteCarloStep(state->patterns,&efCol);
        monteCarloStep(state->coefficients,&efRow);

		if(i%1000 == 0){
			double error = efRow.error();
			buffer[0] = error;
			state->patterns.write(&buffer[1]);	
			int randProcess = (rand()%process-1)+1;
			MPI_Isend(buffer,bufferSize,MPI_DOUBLE,randProcess,tag,MPI_COMM_WORLD,&req);
			MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,&status);
		    if(flag == 1){
		        int source = status.MPI_SOURCE;
			    MPI_Recv(buffer,bufferSize,MPI_DOUBLE,source,tag,MPI_COMM_WORLD,&status);
				if(buffer[0] < error){
					state->patterns.write(&buffer[1]);
				}
				cout << hostname << " switched patterns -- old error: " << error << endl;
				error = efRow.error();
			}
			cout << myHost << ": " << i << "\t Error = " 
				 << error << "\t Time = " << watch.formatTime(watch.lap()) << endl;
		}
	}

    cout << hostname << "\tFinal Error: " << efRow.error() << endl;
    cout << hostname << "\tError Histogram: " << efRow.errorDistribution(10) << endl;
    cout << hostname << "\tTotal time: " << watch.formatTime(watch.stop()) << endl;

	delete buffer;
}

void FuncThrow::run(){
	double error = 0;
	double* allError;

    int minRank = 0;

    //MPI variables
    int process = 0;
    int rank = 0;
	int tag  = 0;
    MPI_Status status;


	if(rank == 0){
		allError = new double[process];
	}else{
		monteCarlo();
		error = anneal();
		cout << hostname << " has " << error << " error" << endl;
	}

	MPI_Gather(&error, 1, MPI_DOUBLE, allError, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	int minRank =0;
	if(rank == 0){
		error = state->expression.rows()*state->expression.cols();
		for(int i =1;i < process; ++i){
			if(error >= allError[i]){
				minRank = i;
			}
		}
	}

	MPI_Bcast(&minRank, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if(rank ==0)	
		MPI_Recv(state->patterns.matrix.data(),
			state->patterns.matrix.rows()*state->patterns.matrix.cols(),
			MPI_DOUBLE,minRank,tag,MPI_COMM_WORLD,&status);
        MPI_Recv(state->coefficients.matrix.data(),
			state->coefficients.matrix.rows()*state->coefficients.matrix.cols(),
			MPI_DOUBLE,minRank,tag,MPI_COMM_WORLD,&status);
	}else if(minRank == rank){
		cout << hostname << " is sending the final data" << endl;
		MPI_Send(state->patterns.matrix.data(),
			state->patterns.matrix.rows()*state->patterns.matrix.cols(),
			MPI_DOUBLE,0,tag,MPI_COMM_WORLD);
		MPI_Send(state->coefficients.matrix.data(),
			state->coefficients.matrix.rows()*state->coefficients.matrix.cols(),
			MPI_DOUBLE,0,tag,MPI_COMM_WORLD);
	}	
}

void DistNaive::stop(){
    //write out the best matrices to files
    if(rank == 0){
        state->patterns.write(state->analysis + "patterns.csv");
        state->coefficients.write(state->analysis + "coefficients.csv");

        ofstream fout;
        fout.open(state->analysis + "expression.txt");
        fout << state->coefficients.matrix*state->patterns.matrix;
        fout.close();
    }

    MPI_Finalize();

}
