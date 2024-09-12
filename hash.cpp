#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <set>
#include <sstream>
#include <algorithm>
using namespace std;

// Function to split a string by spaces
vector<string> split(const string &s, char delimiter = ' ') {
    vector<string> tokens;
    string token;
    stringstream ss(s);
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to hash the candidate pairs and find frequent pairs
void hashBasedApriori(const string &inputFile, const string &outputFile, int minSupport) {
    ifstream inFile(inputFile);
    ofstream outFile(outputFile);

    if (!inFile.is_open()) {
        cerr << "Error: Unable to open input file!" << endl;
        return;
    }

    if (!outFile.is_open()) {
        cerr << "Error: Unable to open output file!" << endl;
        return;
    }

    vector<set<int>> transactions;  // Vector to store all transactions
    unordered_map<int, int> itemCount;  // Map to store count of each item
    unordered_map<int, int> hashBuckets;  // Hash buckets for candidate pairs

    string line;
    while (getline(inFile, line)) {
        vector<string> items = split(line);
        set<int> transaction;
        for (const string &item : items) {
            int itemID = stoi(item);
            transaction.insert(itemID);
            itemCount[itemID]++;
        }
        transactions.push_back(transaction);
    }

    // First pass: finding frequent 1-itemsets
    set<int> frequentItems;
    for (const auto &item : itemCount) {
        if (item.second >= minSupport) {
            frequentItems.insert(item.first);
        }
    }

    // Hashing candidate 2-itemsets
    for (const auto &transaction : transactions) {
        for (auto it1 = transaction.begin(); it1 != transaction.end(); ++it1) {
            for (auto it2 = next(it1); it2 != transaction.end(); ++it2) {
                int hashValue = (*it1 * 1000 + *it2) % 100;  // Simple hash function for demo
                hashBuckets[hashValue]++;
            }
        }
    }

    // Second pass: finding frequent 2-itemsets using hash buckets
    unordered_map<pair<int, int>, int, hash<pair<int, int>>> candidatePairs;
    for (const auto &transaction : transactions) {
        for (auto it1 = transaction.begin(); it1 != transaction.end(); ++it1) {
            for (auto it2 = next(it1); it2 != transaction.end(); ++it2) {
                int hashValue = (*it1 * 1000 + *it2) % 100;
                if (hashBuckets[hashValue] >= minSupport) {
                    candidatePairs[{*it1, *it2}]++;
                }
            }
        }
    }

    // Writing the frequent 1-itemsets and 2-itemsets to the output file
    outFile << "Frequent 1-itemsets:\n";
    for (const int &item : frequentItems) {
        outFile << item << " (" << itemCount[item] << ")\n";
    }

    outFile << "\nFrequent 2-itemsets:\n";
    for (const auto &pair : candidatePairs) {
        if (pair.second >= minSupport) {
            outFile << pair.first.first << ", " << pair.first.second << " (" << pair.second << ")\n";
        }
    }

    inFile.close();
    outFile.close();
}

int main() {
    string inputFile = "transaction_hash.txt";  // Input file name
    string outputFile = "ans_hash.txt";         // Output file name
    int minSupport = 2;  // Minimum support count

    // Run the hash-based Apriori algorithm
    hashBasedApriori(inputFile, outputFile, minSupport);

    cout << "Frequent itemsets written to " << outputFile << endl;
    return 0;
}

