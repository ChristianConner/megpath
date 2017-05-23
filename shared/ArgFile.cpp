#include "ArgFile.h"

ArgFile::ArgFile(){
}

void ArgFile::load(string filename){
	string line = "";
	string key = "";
	string equals = "";
	string value = "";
	Value val = Value();

	ifstream inFile;
	inFile.open(filename);

	while(getline(inFile,line)){
		if(line[0] == '#'){
			continue;
		}else{
			/* XXX breaks if the value has a space within it XXX */
			stringstream ss;
			ss << line;
			ss >> key;
			ss >> equals;
			ss >> value;
			val = Value(value);
		}

		args.insert(pair<string,Value>(key,val));
	}

	inFile.close();
}

string ArgFile::toString(){
	string rv = "";
	stringstream ss;

	for(map<string,Value>::iterator i = args.begin(); i != args.end(); ++i){
		ss << i->first << " = " << i->second.toString() << "\n";
		ss >> rv;
	}

	return rv;
}

Value ArgFile::getArgument(string key){
	Value rv = Value();
	string val = "";

	for(map<string,Value>::iterator i = args.begin(); i != args.end(); ++i){
		if(i->first == key){
			rv = i->second;
			return rv;
		}
	}

	return rv;
}

bool ArgFile::isArgument(string key);
	for(map<string,Value>::iterator i = args.begin(); i != args.end(); ++i){
		if(i->first == key)
			return true;
		}
	}
	return false;
}
