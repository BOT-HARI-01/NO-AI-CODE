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
vector<string> queryTokenizer(const string &);
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
                pos = stream.tellg();
                long start_pos = pos - static_cast<long>(buffer.length()) - 1;
                tokens[buffer].push_back(start_pos);
            }
            buffer.clear();
        }
    }
    if (!buffer.empty() && stopWords.find(buffer) == stopWords.end())
    {
        long pos = stream.tellg();
        long start_pos = pos - static_cast<long>(buffer.length());
        tokens[buffer].push_back(start_pos);
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

vector<string> queryTokenizer(const string &query)
{
    vector<string> queryTokens;
    string q = query;
    transform(q.begin(), q.end(), q.begin(), ::tolower);
    stringstream stream(q);
    char c;
    string buffer;
    int count = 0;
    while (stream.get(c))
    {
        if (isalnum(c))
        {
            buffer += c;
        }
        else
        {
            if (!buffer.empty())
            {
                count++;
                if (stopWords.find(buffer) == stopWords.end())
                {
                    queryTokens.push_back(buffer);
                }
            }
            buffer.clear();
        }
    }
    if (!buffer.empty())
    {
        count++;
        if (stopWords.find(buffer) == stopWords.end())
        {
            queryTokens.push_back(buffer);
        }
    }
    return (queryTokens.size() == count) ? queryTokens : vector<string>{};
}

vector<pair<string, vector<positioning>>> queryIndexFinder(vector<string> query, unordered_map<string, vector<positioning>> &iix)
{
    vector<pair<string, vector<positioning>>> loc;
    int count = 0;
    for (string q : query)
    {
        count++;
        if (iix.find(q) != iix.end())
        {
            loc.push_back({q, iix[q]});
        }
    }
    return (count == loc.size()) ? loc : vector<pair<string, vector<positioning>>>{};
}

unordered_map<int, vector<pair<string, vector<long>>>> intersectionSearch(vector<pair<string, vector<positioning>>> data)
{
    if (data.empty())
        return {};

    sort(data.begin(), data.end(),
         [](auto &a, auto &b)
         {
             return a.second.size() < b.second.size();
         });

    unordered_map<int, vector<pair<string, vector<long>>>> result;

    for (const auto &p : data[0].second)
    {
        result[p.doc_id].push_back({data[0].first, p.positions});
    }

    for (int i = 1; i < data.size(); i++)
    {
        unordered_map<int, vector<long>> curr_doc;
        for (const auto &cd : data[i].second)
        {
            curr_doc[cd.doc_id] = cd.positions;
        }

        for (auto it = result.begin(); it != result.end();)
        {
            int idoc_id = it->first;

            if (curr_doc.find(idoc_id) == curr_doc.end())
            {
                it = result.erase(it);
            }
            else
            {
                vector<long> &positions = curr_doc.find(idoc_id)->second;
                it->second.push_back({data[i].first, positions});
                ++it;
            }
        }
    }
    return result;
}

vector<pair<int, long>> phraseSearch(unordered_map<int, vector<pair<string, vector<long>>>> &data)
{
    vector<pair<int, long>> results;
    long pos;
    for (const auto &doc_entry : data)
    {
        const auto &doc_id = doc_entry.first;
        const auto &words_positions = doc_entry.second;

        bool phrase_found = true;

        for (size_t i = 0; i < words_positions.size() - 1; ++i)
        {
            const string &word1 = words_positions[i].first;
            const vector<long> &pos1 = words_positions[i].second;
            const string &word2 = words_positions[i + 1].first;
            const vector<long> &pos2 = words_positions[i + 1].second;

            bool found_adjacent = false;

            for (long p1 : pos1)
            {
                long expected_pos = p1 + (long)word1.length() + 1;

                if (binary_search(pos2.begin(), pos2.end(), expected_pos))
                {
                    found_adjacent = true;
                    pos = p1;
                    break;
                }
            }

            if (!found_adjacent)
            {
                phrase_found = false;
                break;
            }
        }

        if (phrase_found)
        {
            results.push_back({doc_id, pos});
            return results;
        }
    }
    return results;
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
    // gets the vector of strings of the single stmt query that has been parsed
    vector<string> query_stream = queryTokenizer(query);
    // gets the location of each and every word of the query from the index
    vector<pair<string, vector<positioning>>> qStream_loc = queryIndexFinder(query_stream, iix);
    // Search only
    if (query_stream.size() != qStream_loc.size())
    {
        cout << "The Query Words are not found at index, Search has been stopped!" << endl;
        return 0;
    }

    if (qStream_loc.size() == 1)
    {
        for (const auto &qs : qStream_loc)
        {
            for (const auto &v : qs.second)
            {
                ifstream file(files[v.doc_id], ios::binary);
                if (!file)
                {
                    cerr << "Cannot open file " << files[v.doc_id] << endl;
                    continue;
                }

                for(long l : v.positions){

                    file.seekg(l);
                    
                    char buffer[101] = {0};
                    file.read(buffer, 100);
                    cout << "Snippet: " << buffer << "\n\n";
                }
            }
        }
    }
    else
    {

        unordered_map<int, vector<pair<string, vector<long>>>> intersection = intersectionSearch(qStream_loc);
        auto positions = phraseSearch(intersection);

        if (positions.empty())
        {
            cout << "Phrase NOT found." << endl;
        }
        else
        {
            for (auto &[doc_id, start_pos] : positions)
            {
                cout << "Phrase found in document " << files[doc_id] << " at position " << start_pos << endl;

                ifstream file(files[doc_id], ios::binary);
                if (!file)
                {
                    cerr << "Cannot open file " << files[doc_id] << endl;
                    continue;
                }

                file.seekg(start_pos);

                char buffer[101] = {0};
                file.read(buffer, 100);
                cout << "Snippet: " << buffer << "\n\n";
            }
        }
    }
    return 1;
}