#include "ErrorFunction.h"

ErrorFunction::ErrorFunction(State* st){
	state = st;
}

double ErrorFunction::error(){
	newExpression.noalias() = state->expression;
    newExpression.noalias() -= (state->coefficients.matrix*state->patterns.matrix);
    return (newExpression.cwiseAbs()).sum();		
}

double ErrorFunction::fastError(){
	return error();
}

string ErrorFunction::errorDistribution(int b){
    vector<int> bins(b,0);
    newExpression = state->coefficients.matrix*state->patterns.matrix;
    for(int y = 0; y < state->expression.rows(); ++y){
        for(int x = 0; x < state->expression.cols(); ++x){
            double e = abs(state->expression(y,x)-newExpression(y,x));
            bins[(int)(e*bins.size())] += 1;
        }
    }
    stringstream ss;
    ss << "[";
    for(int i =0; i < bins.size(); ++i){
        ss << bins[i];
        if(i != bins.size()-1)
            ss << ",";
    }
    ss << "]";
    return ss.str();
}