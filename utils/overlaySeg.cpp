#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
#include <libgen.h>

#include <opencv2/opencv.hpp>

using std::string;

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
	if (argc < 4) {
		cerr << "Usage: " << argv[0] << " SEG_IMG_DIR IMG_DIR OUT_DIR";
		return -1;
	}
	char *SEG_IMG_DIR = argv[1];
	char *IMG_DIR = argv[2];
	char *OUT_DIR = argv[3];

	string fname;
	for (int i = 1; i <= 125; i++) {
    string fname = to_string((long long)i) + ".jpg";
		Mat I = imread(string(IMG_DIR) + "/" + fname);
    Mat S = imread(string(SEG_IMG_DIR) + "/" + fname, IMREAD_GRAYSCALE);
		if (!S.data) {
			cerr << "Unable to read image " << string(SEG_IMG_DIR) + "/" + fname << endl;
			continue;
		}
		if (!I.data) {
			cerr << "Unable to read image " << string(IMG_DIR) + "/" + fname << endl;
			continue;
		}

		string outfpath = string(OUT_DIR) + "/" + fname;
		I = I * 0.5;
    int SZ_X = I.cols, SZ_Y = I.rows;
    imwrite("temp.jpg", S);
		resize(S, S, Size(SZ_X, SZ_Y));
		Mat channels[3];
		split(I, channels);
		addWeighted(channels[2], 0.5, S, 1.0, 0.0, channels[2]);
		merge(channels, 3, I);
		imwrite(outfpath, I);
	}
	return 0;
}

