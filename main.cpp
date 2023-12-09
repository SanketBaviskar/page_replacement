#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
#include <utility>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iterator>
using namespace std;

int FIFO(const vector<int>& pageNumbers, int frameSize, vector<double>& pageFaultRates)
{
    list<int> page;
    int pageFaults = 0;
    pageFaultRates.clear();

    cout<<"Simulating First In First Out page replacement algorithm.\n";
    for (int i = 0, n = pageNumbers.size(); i<n; ++i){
        if (find(page.begin(), page.end(), pageNumbers[i]) == page.end()) 
        {    
            if (page.size() < frameSize) {
                page.push_front(pageNumbers[i]);
            } else {
                page.pop_back();
                page.push_front(pageNumbers[i]);
            }
            pageFaults++;
        }
        if (i > 0 && i % 2000 == 0) {
            pageFaultRates.push_back((pageFaults * 1.0) / i);
        }
    }
    
    pageFaultRates.push_back((pageFaults * 1.0) / pageNumbers.size());
    return pageFaults;
    
}

int LRU(const vector<int>& pageNumbers, int frameSize, vector<double>& pageFaultRates)
{
    list<int> page;
    int pageFaults = 0;
    
    pageFaultRates.clear();
    cout<<"Simulating Least Recently Used page replacement algorithm.\n";
    
    for (int i = 0, n = pageNumbers.size(); i < n; ++i) 
    {    
        list<int>::iterator locate = find(page.begin(), page.end(), pageNumbers[i]);
        
        // In case of miss
        if (locate == page.end()) {
            if (page.size() < frameSize) {
                page.push_front(pageNumbers[i]);
            } else {
                page.pop_back();
                page.push_front(pageNumbers[i]);
            }
            pageFaults++;
        } else {
            page.erase(locate);
            page.push_front(pageNumbers[i]);
        }
        
        if (i > 0 && i % 2000 == 0) {
            pageFaultRates.push_back((pageFaults * 1.0) / i);
        }
    }
    pageFaultRates.push_back((pageFaults * 1.0) / pageNumbers.size());
    
    return pageFaults;
}

int optimalPageReplacement(const vector<int>& pageNumbers, int frameSize, vector<double>& pageFaultRates)
{
    vector<int> page;
    vector<int> temp(frameSize, -1);
    int pageFaults = 0;
    pageFaultRates.clear();
    int pos, max;
    bool flag = false;
    cout<<"Simulating Optimal page replacement algorithm.\n";
    
    for (int i = 0, n = pageNumbers.size(); i < n; ++i) {
        bool found = false;
        for (int j = 0; j < page.size(); ++j) {
            if (page[j] == pageNumbers[i]) {
                found = true;
                break;
            }
        }
        //in case of miss
        if (!found) {
            if (page.size() < frameSize) {
                page.push_back(pageNumbers[i]);
            } else {
                for (int j = 0; j < frameSize; ++j) {
                    temp[j] = -1;                    
                    for (int k = i + 1; k < pageNumbers.size(); ++k) {
                        if (page[j] == pageNumbers[k]) {
                            temp[j] = k;
                            break;
                        }
                    }
                }
                // remove the page which is not referenced again in future
                for (int j = 0; j < frameSize; ++j) {
                    if (temp[j] == -1) {
                        pos = j;
                        flag = true;
                        break;
                    }
                }
                // if all pages are referenced again in future, then find the
                // page which is referenced later
                if (!flag) {
                    max = temp[0];
                    pos = 0;
                    for (int j = 1; j < frameSize; ++j) {
                        if (temp[j] > max) {
                            max = temp[j];
                            pos = j;
                        }
                    }
                }
                page[pos] = pageNumbers[i];
            }
            pageFaults++;
        }
        if (i > 0 && i % 2000 == 0) {
            pageFaultRates.push_back((pageFaults * 1.0) / i);
        }
    }
    pageFaultRates.push_back((pageFaults * 1.0) / pageNumbers.size());
    
    return pageFaults;
}

vector<int> readPage(const char* inputFile) {
    cout<<"Reading page from the input file..\n";
    ifstream fin(inputFile);
    vector<int> pageNumbers;
    
    // read the input file in vector
    copy(istream_iterator<int>(fin), istream_iterator<int>(), back_inserter(pageNumbers));
    return pageNumbers;
}

int main(int argc, char* argv[]) {
    cout<<"Starting execution\n";
    if (argc != 4) {
        cout<<"Invalid arguments. Plese provide three arguments:\n(1). The number of page frames of the physical memory.\n"
        <<"(2). Input file name \n(3). Output file name";
        return 1;
    }
    
    int frameSize = atoi(argv[1]);
    int numFaults;
    vector<int> pageNumbers = readPage(argv[2]);
    vector<double> pageFaultRates;
    
    ifstream fileExistsCheck(argv[3]);
    bool fileExists = fileExistsCheck.good();
    fileExistsCheck.close();
    ofstream outputFile;
    if (fileExists) {
        outputFile.open(argv[3], std::ios::app);
    } else {
        outputFile.open(argv[3]);
    }
    
    outputFile <<"\n";
    outputFile << "\n===============================================\n";
    outputFile << "\tPage Replacement Algorithm Simulation (frame size = " << frameSize << " )" << "\n";
    outputFile << "===============================================\n";
    outputFile << "                         Page fault rates\n";
    outputFile << "Algorithm   Total page faults\t2000\t4000\t6000\t8000\t10000\n";
    outputFile << "--------------------------------------------------------------------------------------";
    
    numFaults = FIFO(pageNumbers, frameSize, pageFaultRates);
    outputFile << "\nFIFO     \t\t\t" << numFaults << "\t\t";
    for (int i = 0; i < pageFaultRates.size(); ++i) {
        outputFile << setprecision(3) << pageFaultRates[i] << "\t";
    }
    
    numFaults = LRU(pageNumbers, frameSize, pageFaultRates);
    outputFile << "\nLRU      \t\t\t" << numFaults << "\t\t";
    for (int i = 0; i < pageFaultRates.size(); ++i) {
        outputFile << setprecision(3) << pageFaultRates[i] << "\t";
    }
    
    numFaults = optimalPageReplacement(pageNumbers, frameSize, pageFaultRates);
    outputFile << "\nOptimal  \t\t\t" << numFaults << "\t\t";
    for (int i = 0; i < pageFaultRates.size(); ++i) {
        outputFile << setprecision(3) << pageFaultRates[i] << "\t";
    }
    
    return 0;
}