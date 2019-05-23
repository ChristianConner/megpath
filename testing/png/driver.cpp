//PNG Testing
//Justin Moore
//May 22, 2019

#include <iostream>
#include "../../shared/PNG.h"
#include <time.h>

using namespace std;

void pngMake(){
	Image* mak = createImage(16,16);	
	unsigned int len = mak->width*mak->height*mak->channels;
	int rless = 0;
	for(int i = 0; i<len; i++){
		if(i%4 == 0){
			mak->data[i] = rand()%(rless+1);
		}else if(i%4 == 1){
			mak->data[i] = rand()%(rless+1);
		}else if(i%4 == 2){
			rless = rand()%256;
			mak->data[i] = rless;
		}else{
			mak->data[i] = 255;
		}
	}
	writePng("randb.png",mak);
}

int main(int argc, char* argv[]){
	srand(time(0));
	if(argc < 2){
		pngMake();
		return 0;
	}

	Image* tst;

	tst = readPng(argv[1]);

	cout << tst->width << endl;
	cout << tst->height << endl;
	cout << tst->channels << endl;

	unsigned int len = tst->width*tst->height*tst->channels;

	if(argc > 2){
		cout << argc << endl;
		for(int i = 0; i<len; i++){
			if(i%4 != 3){
				tst->data[i] = 255 - tst->data[i];
			}
		}
	}else{
		unsigned int wag = 0;
		for(int i = 0; i<len; i++){
			if(i%4 == 0){
				wag += tst->data[i]*2;
			}else if(i%4 == 1){
				wag += tst->data[i]*3;
			}else if(i%4 == 2){
				wag += tst->data[i];
			}else{
				wag /= 6;
				tst->data[i-1] = wag;
				tst->data[i-2] = wag;
				tst->data[i-3] = wag;
				wag = 0;
			}
		}	
	}
	
	writePng("output.png",tst);

	return 0;
}
