#include "BlockThrow.h"
#include "time.h"
#include <set>

BlockThrow::BlockThrow(): BlockParallel(){
	program = "BlockThrow";
}

void BlockThrow::start(){
	srand(time(0)+rank);
	BlockParallel::start();

	int smallCol = systemSize;
	for(auto c: cGroups){
		int cSize;
		MPI_Group_size(c,&cSize);
		if(cSize < smallCol){
			smallCol = cSize;
		}
	}
	vector<BlockSet> shareSets;
	shareSets.resize(smallCol);

	MPI_Group worldGroup;
	MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);
	for(int i = 0; i < smallCol; i++){
		int nSize = 0;
		for(int j = 0; j < systemSize; j++){
			if(j%smallCol == i){
				nSize++;
			}
		}
		int NGA[nSize];
		int k = 0;
		for(int j = 0; j < systemSize; j++){
			if(j%smallCol == i){
				NGA[k] = j;
				k++;
			}
		}
		MPI_Group_incl(worldGroup,nSize,&NGA[0],&shareSets[i].group);
		MPI_Comm_create(MPI_COMM_WORLD,shareSets[i].group,&shareSets[i].comm);
	}
	shareSet = shareSets[rank%smallCol];


	Range fixRange;
	fixRange.rowStart = 0;
	fixRange.rowEnd = state->patterns.rows()-1;
	fixRange.colStart = state->patterns.columns();

	state->patterns.resize(state->patterns.rows(), state->patterns.columns() + sampleSize*(shareSet.groupSize()-1));
	fixRange.colEnd = state->patterns.columns()-1;

	state->patterns.createBuffers();
	state->patterns.fixRange(fixRange);
	state->patterns.matrix = MatrixXd::Constant(state->patterns.rows(),state->patterns.columns(),rank);
	state->expression.conservativeResize(state->coefficients.rows(),state->patterns.columns());
}

void BlockThrow::run(){

	//XXX: (Z_[a/2])^2 *p*(1-p)/(M.O.E)^2 = X   -----   N*X/(X+N-1) = *n*

	state->both = true;
	double error = 0;

	algorithm->setObserver(this);
	algorithm->monteCarlo();
	averagePatterns();
	averageCoefficients();
	error = algorithm->anneal();
	averagePatterns();
	state->both = false;
	error = algorithm->anneal();
	averageCoefficients();
	Range s = block;
	s.rowEnd = state->patterns.rows()-1;
	s.colEnd -= s.colStart;
	s.rowStart = 0;
	s.colStart = 0;
	state->patterns.shrink(s);

	gatherPatterns();
	gatherCoefficients();
	if(rank == 0){
		state->errorToPNG();
	}
}

void BlockThrow::throwPatterns(){
	int commSize = shareSet.groupSize();
	int columnSize = state->patterns.rows()+1;
	double recvBuf[ sampleSize*columnSize*commSize];
	vector<int> columnIndices(block.colSize());
	iota(columnIndices.begin(),columnIndices.end(),block.colStart);
	random_shuffle(columnIndices.begin(),columnIndices.end());

	double* from = state->patterns.matrix.data();	
	for(int i =0; i < sampleSize; ++i){
		state->patterns.sendBuffer[i*columnSize] = columnIndices[i];
		int ndx = columnIndices[i]-block.colStart;
		copy(&from[ndx*state->patterns.rows()],&from[(ndx+1)*state->patterns.rows()],&state->patterns.sendBuffer[i*columnSize+1]);
	}

	MPI_Allgather(state->patterns.sendBuffer,sampleSize*columnSize,MPI_DOUBLE,&recvBuf[0],sampleSize*columnSize,MPI_DOUBLE,shareSet.comm);

	set<int> patternIndices;
	patternIndices.insert(columnIndices.begin(),columnIndices.end());
	double* recv = &recvBuf[0];
	int currentColumn = block.colSize();
	for(int i =0; i < sampleSize*commSize; ++i){
		if(patternIndices.find(recv[0]) == patternIndices.end()){
			Map<MatrixXd> pmap(&recv[1],state->patterns.rows(),1);
			state->patterns.matrix.col(currentColumn) = pmap;
			state->expression.col(currentColumn) = oexpression.block(block.rowStart,recv[0],state->expression.rows(),1);
			currentColumn += 1;
		}
		recv += columnSize;
	}
	Range cross = {0,state->patterns.rows()-1,block.colSize(),state->patterns.columns()-1};
	state->patterns.observeRange(cross);
}


void BlockThrow::throwPatterns2(){
	int count = 0;
	Range colGrab;
	int cSize;
	MPI_Comm_size(shareSet.comm, &cSize);
	//Set receive buffer size
	int intake = sampleSize*(state->patterns.rows()+1)*cSize;
	double recvBuf[intake];
	double* m = state->patterns.matrix.data();

	//Generate random column number and check if it hasn't been used before
	set<int> pushed;
	bool unique = true;
	for(int i = 0; i < sampleSize; i++){
		int rCol = rand()%state->patterns.columns();
		while(!unique){
			unique = true;
			if(pushed.find(rCol) != pushed.end()){
				rCol = rand()%state->patterns.columns();
				unique = false;
			}
		}
		cout << rank << ' ' << rCol+block.colStart << ' ' << hostname << endl;
		pushed.insert(rCol);

		//Set column number and data for that column into buffer
		state->patterns.sendBuffer[i*(state->patterns.rows()+1)] = rCol+block.colStart;
		colGrab.rowStart = 0;
		colGrab.colStart = rCol;
		colGrab.rowEnd = state->patterns.rows()-1;
		colGrab.colEnd = rCol;
		m += (rCol-block.colStart)*state->patterns.rows();
		memcpy(&state->patterns.sendBuffer[i*(state->patterns.rows()+1)+1],m,state->patterns.rows()*sizeof(double));
		m -= (rCol-block.colStart)*state->patterns.rows();	
	}

	cout << "Gathering..." << endl;
	MPI_Allgather(state->patterns.sendBuffer,sampleSize*(state->patterns.rows()+1),MPI_DOUBLE,recvBuf,intake,MPI_DOUBLE,shareSet.comm);
	cout << "Gathered" << endl;
	double* ptr = recvBuf;
	//Create set of already seen columns, starting with own columns
	set<int> added;
	for(int i = block.rowStart; i <= block.rowEnd; i++){
		added.insert(i);
	}
	colGrab.colStart = state->patterns.columns()-sampleSize;
	colGrab.colEnd = state->patterns.columns()-sampleSize;
	while(count < sampleSize){
		if(added.find(*ptr) != added.end()){
			state->patterns.read((ptr+1),colGrab);
			state->patterns.observeRange(colGrab);
			added.insert(*ptr);
			colGrab.colStart++;
			colGrab.colEnd++;
			count++;
		}
		ptr += state->patterns.rows()+1;
	}
	//	delete[] ptr;
}

void BlockThrow::monteCallback(int iter){
	if(iter/state->interruptRuns%2 == 1){
		averagePatterns();
		throwPatterns();
	}else{
		averageCoefficients();
	}
}

bool BlockThrow::annealCallback(int iter){
	if(iter/state->interruptRuns%2 == 1){
		averagePatterns();
		throwPatterns();
	}else{
		averageCoefficients();
	}
	return true;
}

void BlockThrow::stop(){
	BlockParallel::stop();
}
