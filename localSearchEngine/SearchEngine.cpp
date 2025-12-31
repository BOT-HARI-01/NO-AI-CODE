#include "headers.h"
#include <cctype>
using namespace std;
struct positioning
{
    int doc_id;
    vector<long> positions;
};
// function declaration
void sWMapper(const string &);
string fileReader(const string &);
vector<string> queryParser(const string &);
unordered_map<string, vector<positioning>> buildInvertedIndex(vector<string>);

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
        transform(word.begin(), word.end(), word.begin(), ::tolower);
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

unordered_map<string, vector<long>> tokenizer(const string &file)
{
    unordered_map<string, vector<long>> tokens;
    ifstream stream(file);
    if (!stream.is_open())
    {
        cerr << "Error: Could not open the File in tokenizer" << endl;
        return {};
    }
    string buffer;
    char c;
    long pos = 0;
    while (stream.get(c))
    {
        if (isalnum(c))
        {
            buffer += tolower(c);
        }
        else
        {
            if (!buffer.empty() && stopWords.find(buffer) == stopWords.end())
            {

                tokens[buffer].push_back(pos - static_cast<long>(buffer.length()));
            }
            buffer.clear();
        }
        pos++;
    }
    if (!buffer.empty() && stopWords.find(buffer) == stopWords.end())
    {

        tokens[buffer].push_back(pos - static_cast<long>(buffer.length()));
    }
    buffer.clear();
    return tokens;
}

unordered_map<string, vector<positioning>> buildInvertedIndex(vector<string> files)
{
    int file_id = 0;
    unordered_map<string, vector<positioning>> invertedIndex;
    while (file_id < files.size())
    {
        cout << files[file_id];
        unordered_map<string, vector<long>> tokens = tokenizer(files[file_id]);
        for (const auto &t : tokens)
        {
            invertedIndex[t.first].push_back({file_id, t.second});
        }
        file_id++;
    }
    return invertedIndex;
}

void invertedIndexWriter(unordered_map<string, vector<positioning>> &iix, ofstream &outputFile)
{
    for (const auto &v : iix)
    {
        outputFile << v.first << " -> ";
        for (positioning pos : v.second)
        {
            outputFile << " ID : " << pos.doc_id << " positions : ";
            for (long pp : pos.positions)
            {
                outputFile << pp << ",";
            }
        }
        outputFile << endl;
    }
}

vector<string> queryParser(const string &query)
{
    vector<string> queryTokens;
    string q = query;
    transform(q.begin(), q.end(), q.begin(), ::tolower);
    stringstream stream(q);
    char c;
    string buffer;
    while (stream.get(c))
    {
        if (isalnum(c))
        {
            buffer += c;
        }
        else
        {
            if (!buffer.empty() && stopWords.find(buffer) == stopWords.end())
            {
                queryTokens.push_back(buffer);
            }
            buffer.clear();
        }
    }
    if (!buffer.empty() && stopWords.find(buffer) == stopWords.end())
    {
        queryTokens.push_back(buffer);
    }
    return queryTokens;
}

vector<vector<positioning>> search(vector<string> query, unordered_map<string, vector<positioning>> &iix)
{
    vector<vector<positioning>> loc;
    for (string q : query)
    {
        if (iix.find(q) != iix.end())
        {
            loc.push_back(iix[q]);
        }
    }
    return loc;
}

vector<positioning> intersector(vector<positioning> &a, vector<positioning> &b)
{
    vector<positioning> res;
    int i = 0, j = 0;
    while (i < a.size() && j < b.size())
    {
        if (a[i].doc_id == b[j].doc_id)
        {
            res.push_back({a[i].doc_id});
            i++;
            j++;
        }
        else if (a[i].doc_id < b[j].doc_id)
        {
            i++;
        }
        else
        {
            j++;
        }
    }
    return res;
}

vector<positioning> intersectionSearch(vector<vector<positioning>> data)
{
    if (data.empty()) return {};

    sort(data.begin(), data.end(),
         [](auto &a, auto &b)
         {
             return a.size() < b.size();
         });

    vector<positioning> result = data[0];

    for (int i = 1; i < data.size(); i++)
    {
        
        result = intersector(result, data[i]);
        if (result.empty())
            break;
    }
    return result;
}

int main()
{
    sWMapper("stopWords.txt");
    vector<string> files = {"sample.txt", "sample2.txt"};
    ofstream outputFile;
    outputFile.open("index.txt");
    unordered_map<string, vector<positioning>> iix = buildInvertedIndex(files);
    invertedIndexWriter(iix, outputFile);
    cout << endl
         << "Enter your Query: ";
    string query;
    getline(cin, query);
    vector<string> res = queryParser(query);
    vector<vector<positioning>> searchLoc = search(res, iix);
    unordered_map<int, int> max_repeated;
    vector<positioning> inter = intersectionSearch(searchLoc);
    for(const auto &v : inter){
        cout << v.doc_id << endl;
    }
    // for (const vector<positioning> &val : searchLoc)
    // {
    //     for (positioning pos : val)
    //     {
    //         max_repeated[pos.doc_id]++;
    //     }
    // }
    int maxCnt = 0, rep = -1;
    // for (auto i : max_repeated)
    // {
    //     int val = i.first, cnt = i.second;

    //     if (maxCnt < cnt || (cnt == maxCnt && val > rep))
    //     {
    //         rep = val;
    //         maxCnt = cnt;
    //     }
    // }

    // for (const auto &val : searchLoc)
    // {
    //     for (positioning pos : val)
    //     {
    //         if (pos.doc_id == rep)
    //         {
    //             for (long l : pos.positions)
    //             {
    //                 cout << l << ", ";
    //             }
    //             cout << endl;
    //         }
    //     }
    // }
    return 0;
}