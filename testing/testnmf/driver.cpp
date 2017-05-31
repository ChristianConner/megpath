//connnmf tester
//Matthew Dyer
//Created on 5/26/2017
//Last Modified: 5/26/2017

#include <iostream>
#include <random>
#include <fstream>
#include "../../../Eigen/Core"

using namespace std;
using namespace Eigen;

void writeCSV(string filename, MatrixXd m){
	ofstream outFile;
	outFile.open(filename);

	for(int i = 0; i < m.rows(); ++i){
		for(int j = 0; j < m.cols(); ++j){
			if(j < m.cols()-1){
				outFile << m(i,j) << ",";
			}else{
				outFile << m(i,j);
			}
		}
		outFile << "\n";
	}
	
	outFile.close();
}

int main(){
	MatrixXd patterns(3,5);
	patterns << 0,0.1,0.2,0.3,0.4, 0.7,0.5,0.3,0.1,0, 0.1,0.25,0.5,0.25,0.1;

	default_random_engine gen;
	normal_distribution<double> dist(0.5,0.1);
	MatrixXd coefficients(500,3);
	for(int i = 0; i < coefficients.rows(); ++i){
		for(int j = 0; j < coefficients.cols(); ++j){
			double rando = dist(gen);
			coefficients(i,j) = rando;
			while(rando < 0.0 || rando > 1.0){
				rando = dist(gen);
				coefficients(i,j) = rando;
			}
		}
	}

	MatrixXd mult = coefficients * patterns;

	writeCSV("test_patterns.csv",patterns);
	writeCSV("test_coefficients.csv",coefficients);
	writeCSV("test_multiplied.csv",mult);

	ofstream outfile;
	outfile.open("test_arguments.txt");
	outfile << "analysis = \"test\"\n";
	outfile << "max_runs = 40000\n";
	outfile << "test_patterns = [\"\",\"\",\"\"]\n";
	outfile << "test_filename = \"../testing/testnmf/test_multiplied.csv\"\n";
	outfile << "test_origin = [0,0]\n";
	outfile << "test_directory = \".\"\n";
	outfile << "test_controls = [0]\n";
	outfile << "test_columns = [0]\n";
	outfile.close();

	return 0;
}