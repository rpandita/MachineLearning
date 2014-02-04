#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <math.h>
using namespace std;

static const int divisions = 25;

vector<vector<float> > readTextFile() {
	vector<vector<float> > data;
	vector<float> tempFloatVector;
	string sLine = "";
	ifstream infile;

	cout << "Establishing connection to read the sample data..." << endl;
	infile.open("data.txt");
	cout << "Loading the data into the memory..." << endl;
	while (!infile.eof()) {
		getline(infile, sLine);
		tempFloatVector.clear();
		if (sLine != "") {
			string temp = "";
			for (unsigned int i = 0; i < sLine.length(); i++) {
				if (sLine[i] == '|' || sLine[i] == '\b') {
					if (temp != "") {
						tempFloatVector.push_back((float) (atof(temp.c_str())));
					}
					temp = "";
				} else {
					temp.append(1, sLine[i]);
				}
			}
			data.push_back(tempFloatVector);
		}
	}
	infile.close();
	return data;
}

void printData(vector<vector<float> > data) {
	for (unsigned short int i = 0; i < data.size(); i++) {
		for (unsigned short int j = 0; j < data[i].size(); j++) {
			cout << data[i][j] << " ::";
		}
		cout << endl;
	}
}

void printHistogram(vector<vector<int> > data) {
	for (unsigned short int i = 0; i < data.size(); i++) {
		for (unsigned short int j = 0; j < data[i].size(); j++) {
			cout << data[i][j] << " ::";
		}
		cout << endl;
	}
}
vector<float> computeProbabilityEachOutput(vector<vector<float> > data) {
	float counters[4] = {0};
	vector<float> probabilities;
	for (unsigned int i = 0; i < data.size(); i++) {
		counters[((int) data[i][4])]++;
		counters[0]++;
	}
	for (unsigned int i = 1; i < 4; i++) {
		probabilities.push_back(counters[i] / counters[0]);
	}
	return probabilities;
}

// returns minimum of each column and
vector<vector<float> > getHistogramsDimentions(vector<vector<float> > data) {

	vector<float> minEachColumn;
	vector<float> maxEachColumn;
	vector<vector<float> > HistoGramDimentions;
	vector<float> binsizes;

	for (unsigned int i = 0; i < data[0].size() - 1; i++) {
		minEachColumn.push_back(data[0][i]);
		maxEachColumn.push_back(data[0][i]);
		binsizes.push_back(0);
	}

	for (unsigned int i = 0; i < data.size(); i++) {
		for (unsigned int j = 0; j < data[0].size() - 1; j++) {
			if (minEachColumn[j] > data[i][j]) {
				minEachColumn[j] = data[i][j];
			}
			if (maxEachColumn[j] < data[i][j]) {
				maxEachColumn[j] = data[i][j];
			}
		}
	}

	//number of buckets = 10 = divisions
	for (unsigned int i = 0; i < data[0].size() - 1; i++) {
		float min = (int) minEachColumn[i];
		float max = ((int) maxEachColumn[i]) + 1;
		binsizes[i] = ((max - min) / divisions);
		minEachColumn[i] = min;

	}

	HistoGramDimentions.push_back(minEachColumn);
	HistoGramDimentions.push_back(binsizes);

	return HistoGramDimentions;
}

void initHistogram(int classes, const vector<float>& binSizes,
		vector<vector<int> >& histogramData) {
	//Init Histogram
	vector<int> temp;
	for (unsigned int i = 0; i < (classes * binSizes.size()); i++) {
		temp.clear();
		for (int j = 0; j < divisions; j++) {
			temp.push_back(1);
		}
		histogramData.push_back(temp);
	}
}

vector<int> findIntervals(const vector<vector<float> > &histogramDimentions,
		vector<float> dataRow) {
	vector<int> intervals;
	for (unsigned int i = 0; i < histogramDimentions[0].size(); i++) {
		intervals.push_back(
				(int) ((dataRow[i] - histogramDimentions[0][i])
						/ histogramDimentions[1][i]));
	}
	return intervals;
}

vector<vector<int> > getHistogramData(
		vector<vector<float> > histogramDimentions, vector<vector<float> > data,
		int classes) {

	vector<vector<int> > histogramData;
	initHistogram(classes, histogramDimentions[1], histogramData);
	vector<int> tempIntervals;
	for (unsigned int i = 0; i < data.size(); i++) {
		tempIntervals = findIntervals(histogramDimentions, data[i]);
		for (unsigned int j = 0; j < data[0].size() - 1; j++) {
			histogramData[(data[i][4] - 1) * 4 + j][tempIntervals[j]]++;
		}
	}
	return histogramData;
}
vector<vector<float> > getRandomSampleSet(const vector<vector<float> > & data,
		int percent) {
	int number = (percent * data.size()) / 100;
	int i = 0;
	vector<vector<float> > dataset;
	while (i < number) {
		int temp = rand() % data.size();
		dataset.push_back(data[temp]);
		i++;
	}
	return dataset;
}

vector<int> runBayesAlgorithms(vector<vector<int> > histogram,
		vector<vector<float> > histogramDimentions, vector<vector<float> > data,
		int classes, vector<float> probabilities) {
	vector<int> resultSet;
	vector<int> tempIntervals;
	for (unsigned int i = 0; i < data.size(); i++) {
		//get the bins for each of the data
		//get intervals for each row
		tempIntervals = findIntervals(histogramDimentions, data[i]);
		float maxProbability = 0.0;
		int resultType = 0;
		for (int j = 0; j < classes; j++) {
			float temp = (probabilities[j]
			                            * (histogram[j * 4][tempIntervals[0]]
			                                                * histogram[j * 4 + 1][tempIntervals[1]]
			                                                                       * histogram[j * 4 + 2][tempIntervals[2]]
			                                                                                              * histogram[j * 4 + 3][tempIntervals[3]]));
			if (temp > maxProbability) {
				maxProbability = temp;
				resultType = j + 1;
			}
		}

		resultSet.push_back(resultType);
	}
	return resultSet;
}

void printResult(vector<int> result) {
	for (unsigned int i = 0; i < result.size(); i++) {
		cout << result[i] << endl;
	}
}

void printResult(vector<float> result) {
	for (unsigned int i = 0; i < result.size(); i++) {
		cout << result[i] << endl;
	}
}

void printProbabilities(vector<float> prob){
	cout << "probability setosa     ::" << prob[0] << " " << endl;
	cout << "probability versicolor ::" << prob[1] << " " << endl;
	cout << "probability virginica  ::" << prob[2] << " " << endl;
}

void printBinWidth(vector<float> binWidth){
	cout<< "Sepal length ::" << binWidth[0] << endl;
	cout<< "Sepal width  ::" << binWidth[1] << endl;
	cout<< "Petal length ::" << binWidth[2] << endl;
	cout<< "Petal width  ::" << binWidth[3] << endl;
}

void printResultAnalysis(vector<int> result, vector<vector<float> > &data) {
	int counter = 0, counter1 = 0, counter2 = 0, counter3 = 0;
	for (unsigned int i = 0; i < result.size(); i++) {
		if (data[i][4] != result[i]) {
			counter++;
			switch (((int) data[i][4])) {
			case 1:
				counter1++;
				break;
			case 2:
				counter2++;
				break;
			case 3:
				counter3++;
				break;
			default:
				cout << "error" << endl;
			}
		}
	}
	cout << "After Analysis Found " << counter << " mismatches" << endl;
	cout << "Details::" << endl;
	cout << "setosa     ::" << counter1 << " mismatches" << endl;
	cout << "versicolor ::" << counter2 << " mismatches" << endl;
	cout << "virginica  ::" << counter3 << " mismatches" << endl;
}
int main(int argc, char** argv) {

	unsigned short int choices, classes, percent;
	vector<vector<float> > data;

	// TAKE INPUT FROM USER
	cout << "Welcome to Baye's Classifier :)" << endl;
	cout << "Please enter the number of choices considered to classify ?"
			<< endl;
	cin >> choices;
	cout << "Please enter the number of entities in the result set ?" << endl;
	cin >> classes;
	cout << "Thanks for the input..." << endl;
	cout << "Please select the sampling percentage to be used" << endl;
	cin >> percent;

	// READ DATA IN TO THE MEMORY
	data = readTextFile();
	cout << "Read file completed!!" << endl;

	vector<vector<float> > histogramsDimentions;
	histogramsDimentions = getHistogramsDimentions(data);
	cout<<"Printing the intervals/binwidth used for bins in each feature"<<endl;
	printBinWidth(histogramsDimentions[1]);

	vector<vector<float> > dataset;
	dataset = getRandomSampleSet(data, percent);
	cout << "Printing random dataset using " << percent << "% of the sample"
			<< endl;
	printData(dataset);

	cout<<"Bin width is calculated based on the data set provided. Total span of data is divided in to 20 bins"<<endl;

	vector<float> probClass;
	probClass = computeProbabilityEachOutput(dataset);
	cout << "Printing the probabilities"<<endl;
	printProbabilities(probClass);

	cout << "The percentages of each class in sample is 100 times probabilities"<<endl;


	vector<vector<int> > histograms;
	histograms = getHistogramData(histogramsDimentions, dataset, classes);
	cout << "Printing histograms" << endl;
	printHistogram(histograms);

	vector<int> resultSet;
	resultSet = runBayesAlgorithms(histograms, histogramsDimentions, data,
			classes, probClass);

	printResultAnalysis(resultSet, data);

	return 0;
}

