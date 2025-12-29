#include "headers.h"
#include <cctype>
using namespace std;
struct positioning
{
    int doc_id;
    vector<long> positions;
};
// function declaration
string fileReader(const string &);
void swMapper(const string &);
map<string, vector<positioning>> buildInvertedIndex(vector<string>);

unordered_set<string> stopWords;
void sWMapper(const string &file)
{
    ifstream inputFile(file);

    if (!inputFile.is_open())
    {
        cerr << "Error: Could not open Stop Words file." << endl;
    }
    string word;
    while (inputFile >> word)
    {
        stopWords.insert(word);
    }
    inputFile.close();
}

string fileReader(const string &file)
{
    ifstream inputFile(file);

    if (!inputFile.is_open())
    {
        cerr << "Error: Could not open the File in fileReader" << endl;
        return "";
    }

    string line, res;

    while (getline(inputFile, line))
    {
        res.append(line);
    }
    inputFile.close();
    return res;
}

map<string,vector<long>> tokenizer(const string &file)
{
    map<string,vector<long>> tokens;
    ifstream stream(file);
    if(!stream.is_open()){
        cerr << "Error: Could not open the File in tokenizer" << endl;
        return {};
    }
    string token;
    while (stream >> token)
    {
        long end_pos = stream.tellg();
        long start_pos = end_pos - static_cast<long>(token.length());

        string clean;
        for (char c : token)
        {

            if (isalnum(c))
            {
                clean += tolower(c);
            }
        }

        if (!clean.empty() && !(stopWords.find(clean) != stopWords.end()))
        {
            tokens[clean].push_back(start_pos);
            clean.clear();
        }
    }
    return tokens;
}

map<string, vector<positioning>> buildInvertedIndex(vector<string> files)
{
    int file_id = 0;
    map<string, vector<positioning>> invertedIndex;
    while(file_id < files.size()){
        cout << files[file_id];
        map<string,vector<long>> tokens = tokenizer(files[file_id]);
        for(const auto t : tokens){
            invertedIndex[t.first].push_back({file_id,t.second});
        }
        file_id++;
    }
    return invertedIndex;
}

int main()
{
    sWMapper("stopWords.txt");
    vector<string> files = {"sample.txt","sample2.txt"};
    ofstream outputFile;
    outputFile.open("example.txt");
    map<string,vector<positioning>> iix = buildInvertedIndex(files);
    for(auto v : iix){
        outputFile << v.first << " -> " ;
        for(positioning pos : v.second){
            outputFile << " ID : " << pos.doc_id << " positions : ";
            for(long pp : pos.positions){
                outputFile << pp << ",";
            } 
        }
        outputFile << endl;
    }

    return 0;
}