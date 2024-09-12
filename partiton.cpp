#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <algorithm>
using namespace std;

// Function to read transactions from a file
vector<set<int>> readTransactions(const string& filename) {
    vector<set<int>> transactions;
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        set<int> transaction;
        stringstream ss(line);
        int item;
        while (ss >> item) {
            transaction.insert(item);
        }
        transactions.push_back(transaction);
    }

    file.close();
    return transactions;
}

// Function to generate candidate itemsets of size k
vector<set<int>> generateCandidates(const vector<set<int>>& frequentItemsets, int k) {
    vector<set<int>> candidates;
    int size = frequentItemsets.size();

    for (int i = 0; i < size; ++i) {
        for (int j = i + 1; j < size; ++j) {
            set<int> candidate = frequentItemsets[i];
            candidate.insert(frequentItemsets[j].begin(), frequentItemsets[j].end());

            if (candidate.size() == k) {
                candidates.push_back(candidate);
            }
        }
    }

    return candidates;
}

// Function to count the support of itemsets in transactions
map<set<int>, int> countSupport(const vector<set<int>>& transactions, const vector<set<int>>& candidates) {
    map<set<int>, int> supportCount;

    for (const auto& transaction : transactions) {
        for (const auto& candidate : candidates) {
            if (includes(transaction.begin(), transaction.end(), candidate.begin(), candidate.end())) {
                supportCount[candidate]++;
            }
        }
    }

    return supportCount;
}

// Function to filter itemsets based on minimum support
vector<set<int>> filterFrequentItemsets(const map<set<int>, int>& supportCount, int minSupport) {
    vector<set<int>> frequentItemsets;

    for (const auto& pair : supportCount) {
        if (pair.second >= minSupport) {
            frequentItemsets.push_back(pair.first);
        }
    }

    return frequentItemsets;
}

// Partition the database into smaller parts and generate frequent itemsets for each partition
vector<set<int>> partitionApriori(const vector<set<int>>& transactions, int partitionSize, int minSupport) {
    int numTransactions = transactions.size();
    vector<set<int>> globalFrequentItemsets;

    for (int i = 0; i < numTransactions; i += partitionSize) {
        vector<set<int>> partition(transactions.begin() + i, transactions.begin() + min(i + partitionSize, numTransactions));

        vector<set<int>> frequentItemsets;
        int k = 1;

        // Generate frequent 1-itemsets
        map<set<int>, int> supportCount;
        for (const auto& transaction : partition) {
            for (const int& item : transaction) {
                set<int> itemset = {item};
                supportCount[itemset]++;
            }
        }

        frequentItemsets = filterFrequentItemsets(supportCount, minSupport);

        while (!frequentItemsets.empty()) {
            globalFrequentItemsets.insert(globalFrequentItemsets.end(), frequentItemsets.begin(), frequentItemsets.end());

            vector<set<int>> candidates = generateCandidates(frequentItemsets, ++k);
            supportCount = countSupport(partition, candidates);
            frequentItemsets = filterFrequentItemsets(supportCount, minSupport);
        }
    }

    return globalFrequentItemsets;
}

// Final scan over the entire database to generate global frequent itemsets
vector<set<int>> finalScan(const vector<set<int>>& transactions, const vector<set<int>>& candidates, int minSupport) {
    map<set<int>, int> supportCount = countSupport(transactions, candidates);
    return filterFrequentItemsets(supportCount, minSupport);
}

int main() {
    string filename = "transaction_hash.txt";
    int partitionSize = 3;  // Adjust this as needed
    int minSupport = 2;     // Adjust this as needed

    vector<set<int>> transactions = readTransactions(filename);

    // First pass: Partition-based Apriori to generate global candidate itemsets
    vector<set<int>> candidates = partitionApriori(transactions, partitionSize, minSupport);

    // Second pass: Count the support of global candidate itemsets in the entire dataset
    vector<set<int>> globalFrequentItemsets = finalScan(transactions, candidates, minSupport);

    // Output the global frequent itemsets
    ofstream outfile("ans_part.txt");
    for (const auto& itemset : globalFrequentItemsets) {
        for (const auto& item : itemset) {
            outfile << item << " ";
        }
        outfile << endl;
    }

    outfile.close();
    cout << "Frequent itemsets have been written to ans_part.txt" << endl;

    return 0;
}

