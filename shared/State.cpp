#include "State.h"

State::State(){
	analysis = "";
	directory = "";
	filename = "";
	printRuns = 1000;
}

bool State::load(string argFileName){
	int ROWS = 0;
	int COLUMNS = 0;
	int PATTERNS = 0;
	
	ArgFile args;
	CSVFile file;
	
	vector<Value> origin;
	vector<Value> controls;
	vector<Value> columns;
	vector<Value> patternArgs;
		
	//grab arguments
	args.load(argFileName);

	if(args.isArgument("analysis")){
		Value val = args.getArgument("analysis");
		analysis = val.asString();
		analysis = analysis + "_";
	}

	if(args.isArgument("max_runs")){
		Value val = args.getArgument("max_runs");
		MAX_RUNS = val.asInt();
	}

	cout << "Using analysis: " << analysis.substr(0,analysis.size()-1) << "\n";

	if(args.isArgument(analysis + "filename")){
		Value val = args.getArgument(analysis + "filename");
		filename = val.asString();
	}

	if(args.isArgument(analysis + "patterns")){
		Value val = args.getArgument(analysis + "patterns");
		patternArgs = val.asVector();
		PATTERNS = patternArgs.size();
		int i = 0;
		while(i < PATTERNS){
			patternNames.push_back(patternArgs[i].toString());
			++i;
		}
	}

	if(args.isArgument(analysis + "origin")){
		Value val = args.getArgument(analysis + "origin");
		origin = val.asVector();
	}else{
		Value val;
		origin.push_back(val);
		origin.push_back(val);
	}

	if(args.isArgument(analysis + "directory")){
		Value val = args.getArgument(analysis + "directory");
		directory = val.asString();
	}

	if(args.isArgument(analysis + "controls")){
		Value val = args.getArgument(analysis + "controls");
		controls = val.asVector();
	}

	if(args.isArgument("print_runs")){
		Value val = args.getArgument("print_runs");
		printRuns = val.asInt();
	}

	vector<vector<Value> > csv = file.readCSV(filename);

	//expression matrix
	ROWS = csv.size() - origin[1].asInt();
	if(args.isArgument(analysis + "columns")){
		Value val = args.getArgument(analysis + "columns");
		columns = val.asVector();
		COLUMNS = columns.size();
	}else{
		COLUMNS = csv[0].size() - origin[0].asInt();
		for(int i = 0; i < COLUMNS;++i){
			Value newVal(i);
			columns.push_back(newVal);
		}
	}

	cout << ROWS << "x" << COLUMNS << endl;

	expression = MatrixXd(ROWS,COLUMNS);
	expression = MatrixXd::Zero(ROWS,COLUMNS);
	
	if(columns.size() != controls.size() && args.isArgument(analysis + "controls")){
		cout << "Columns and controls must be the same size.";
		return false;
	}

	for(int i = 0; i < ROWS; ++i){
		for(int j = 0; j < COLUMNS; ++j){
			if(columns.size() == controls.size()){
				expression(i,j) = csv[i+origin[1].asInt()][columns[j].asInt()+origin[0].asInt()].asDouble() - csv[i+origin[1].asInt()][controls[j].asInt()+origin[0].asInt()].asDouble();
			}else{
				expression(i,j) = csv[i+origin[1].asInt()][columns[j].asInt()+origin[0].asInt()].asDouble();
			}
		}
	}

	normalize();

	//should be PATTERNS and COLUMNS
	patterns.resize(PATTERNS,COLUMNS);
	//should be ROWS and PATTERNS
	coefficients.resize(ROWS,PATTERNS);	

	for(int i = 0; i < patternArgs.size(); ++i){
		vector<Value> intoMatrix;
		string findPattern = patternArgs[i].asString();
		if(args.isArgument(findPattern)){
			Value newVal = args.getArgument(findPattern);
			intoMatrix = newVal.asVector();

			ShiftPF* shared = new ShiftPF();			
			vector<Entry> constraints(intoMatrix.size(),{0,0,0});
			for(int j = 0; j < intoMatrix.size(); ++j){
				patterns.matrix(i,j) = intoMatrix[j].asDouble();
				patterns.functions[i][j] = shared;	
				constraints[j].x = j;
				constraints[j].y = i;
				constraints[j].val = patterns.matrix(i,j);	
			}
			shared->setEntries(constraints);
		}
	}
	return true;
}

void State::patternMatch(MatrixXd& other){
	//generate match
	//indices [0 1 2]
	//int index = 0;
	/*
	int minIndex
	double min
	for(int j =0; j < ; ++j){
		get pattern j
		min error from indices[index] to j
		minIndex = index

		for(int i =index; i < ; ++i){
			get indices[i] pattern
			if(error from indices[i] to j){
				
			} 
		}
		
		swap(indices[index],indices[minIndex])
		index += 1;
	}
	*/

	
	//generate permutation

}


void State::normalize(){
	double max = expression.maxCoeff();
	double min = expression.minCoeff();
	expression = expression.array() - min;
	expression = expression/(max-min);
}

