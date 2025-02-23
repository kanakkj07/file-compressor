#include <bits/stdc++.h>
using namespace std;

struct HuffmanNode
{
    char character = '\0';
    int frequency;
    HuffmanNode *left = nullptr;
    HuffmanNode *right = nullptr;

    HuffmanNode(char character, int frequency) : character(character), frequency(frequency) {}
    HuffmanNode(int frequency, HuffmanNode *left, HuffmanNode *right) : frequency(frequency), left(left), right(right) {}
};

HuffmanNode *constructHuffmanTree(const unordered_map<char, int> &frequencyMap)
{
    function<bool(HuffmanNode *, HuffmanNode *)> compareNodes = [&](HuffmanNode *a, HuffmanNode *b)
    {
        return a->frequency > b->frequency;
    };
    priority_queue<HuffmanNode *, vector<HuffmanNode *>, decltype(compareNodes)> minHeap(compareNodes);

    for (auto entry : frequencyMap)
        minHeap.push(new HuffmanNode(entry.first, entry.second));

    while (minHeap.size() > 1)
    {
        HuffmanNode *node1 = minHeap.top();
        minHeap.pop();
        HuffmanNode *node2 = minHeap.top();
        minHeap.pop();

        minHeap.push(new HuffmanNode(node1->frequency + node2->frequency, node1, node2));
    }
    return minHeap.top();
}

void assignBitLength(HuffmanNode *node, int depth, vector<pair<int, char>> &bitLengths)
{
    if (node->character != '\0')
        bitLengths.emplace_back((depth == 0) ? 1 : depth, node->character);
    else
    {
        assignBitLength(node->left, depth + 1, bitLengths);
        assignBitLength(node->right, depth + 1, bitLengths);
    }
}

string incrementBinaryString(string binaryStr)
{
    int index = binaryStr.size() - 1;
    while (binaryStr[index] == '1')
        binaryStr[index--] = '0';
    if (index >= 0)
        binaryStr[index] = '1';
    else
        binaryStr = '1' + binaryStr;
    return binaryStr;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << endl;
        return 1;
    }

    string inputFilePath = argv[1];
    int lastSlashIndex = inputFilePath.find_last_of("/\\");
    string outputFilePath = ((lastSlashIndex == -1) ? inputFilePath : inputFilePath.substr(lastSlashIndex + 1, inputFilePath.size() - 5 - lastSlashIndex)) + "_compressed.bin";

    ifstream inputFile(inputFilePath);
    if (!inputFile)
    {
        cerr << "Error: Could not open file: " << argv[1] << endl;
        return 1;
    }

    unordered_map<char, int> frequencyMap;
    char character;
    while (inputFile.get(character))
        frequencyMap[character]++;
    inputFile.close();

    HuffmanNode *root = constructHuffmanTree(frequencyMap);

    vector<pair<int, char>> bitLengths;
    assignBitLength(root, 0, bitLengths);
    sort(bitLengths.begin(), bitLengths.end());

    unordered_map<char, string> encodingMap;
    string lastBinary = "";
    for (int i = 0; i < bitLengths.size(); i++)
    {
        if (i)
            lastBinary = incrementBinaryString(lastBinary);
        lastBinary.resize(bitLengths[i].first, '0');
        encodingMap[bitLengths[i].second] = lastBinary;
    }

    int totalCharacters = root->frequency;
    int uniqueCharacters = bitLengths.size();

    ofstream outputFile(outputFilePath, ios::binary);
    if (!outputFile.is_open())
    {
        cerr << "Error: Could not open file: " << outputFilePath << endl;
        return 1;
    }
    inputFile.open(inputFilePath);
    if (!inputFile.is_open())
    {
        cerr << "Error: Could not open file: " << inputFilePath << endl;
        return 1;
    }

    outputFile.write(reinterpret_cast<const char *>(&totalCharacters), sizeof(totalCharacters));
    outputFile.write(reinterpret_cast<const char *>(&uniqueCharacters), sizeof(uniqueCharacters));
    for (auto entry : bitLengths)
    {
        outputFile.write(reinterpret_cast<const char *>(&entry.first), sizeof(entry.first));
        outputFile.write(&entry.second, sizeof(entry.second));
    }

    int characterCount = 0;
    string currentBinary = "";
    while (inputFile.get(character))
    {
        characterCount++;
        currentBinary += encodingMap[character];

        while (currentBinary.size() >= 8)
        {
            unsigned char byte = 0;
            for (int i = 0; i < 8; i++)
                byte = ((byte << 1) | ((currentBinary[i] == '1') ? 1 : 0));
            outputFile.write(reinterpret_cast<const char *>(&byte), sizeof(byte));
            currentBinary = (currentBinary.size() != 8) ? currentBinary.substr(8) : "";
        }
    }
    if (!currentBinary.empty())
    {
        currentBinary.resize(8, '0');
        unsigned char byte = 0;
        for (int i = 0; i < 8; i++)
            byte = ((byte << 1) | ((currentBinary[i] == '1') ? 1 : 0));
        outputFile.write(reinterpret_cast<const char *>(&byte), sizeof(byte));
    }
    assert(characterCount == totalCharacters);
    outputFile.close();
    inputFile.close();

    cout << "File Compressed Successfully.." << endl;
    cout << "Compressed File: " << outputFilePath << endl;
    return 0;
}
