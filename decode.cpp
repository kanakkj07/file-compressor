#include <bits/stdc++.h>
using namespace std;

string incrementBinary(string binaryStr)
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

string convertCharToBinary(char character)
{
    return bitset<8>(character).to_string();
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << endl;
        return 1;
    }
    
    string filePath = argv[1];
    int pathIndex = filePath.find_last_of("/\\");
    string outputFile = ((pathIndex == -1) ? filePath : filePath.substr(pathIndex + 1, filePath.size() - 16 - pathIndex)) + "_original.txt";

    ifstream inputFile(filePath, ios::binary);
    if (!inputFile)
    {
        cerr << "Error: Could not open file: " << filePath << endl;
        return 1;
    }

    int totalCharacters, uniqueCharacters;
    inputFile.read(reinterpret_cast<char *>(&totalCharacters), sizeof(totalCharacters));
    inputFile.read(reinterpret_cast<char *>(&uniqueCharacters), sizeof(uniqueCharacters));

    unordered_map<string, char> decodingMap;
    string lastBinary = "";

    for (int i = 0; i < uniqueCharacters; i++)
    {
        if (i)
            lastBinary = incrementBinary(lastBinary);
        
        int length;
        char character;
        inputFile.read(reinterpret_cast<char *>(&length), sizeof(length));
        inputFile.read(&character, sizeof(character));
        lastBinary.resize(length, '0');
        decodingMap[lastBinary] = character;
    }

    ofstream outputFileStream(outputFile);
    if (!outputFileStream.is_open())
    {
        cerr << "Error: Could not open file: " << outputFile << endl;
        return 1;
    }

    int charCount = 0;
    char byte;
    string currentBinary = "";
    while (inputFile.read(&byte, sizeof(byte)))
    {
        currentBinary += convertCharToBinary(byte);
        int index = 0;
        string tempBinary = "";
        
        while (index != currentBinary.size())
        {
            tempBinary += currentBinary[index++];
            if (decodingMap.find(tempBinary) != decodingMap.end())
            {
                outputFileStream << decodingMap[tempBinary];
                charCount++;
                if (charCount == totalCharacters)
                    break;
                if (index == currentBinary.size())
                    currentBinary = "";
                else
                    currentBinary = currentBinary.substr(index);
                tempBinary = "";
                index = 0;
            }
        }
        if (charCount == totalCharacters)
            break;
    }
    
    cout << charCount << " " << totalCharacters << endl;
    assert(charCount == totalCharacters);
    cout << "File Decoded Successfully.." << endl;
    cout << "Decoded File: " << outputFile << endl;
    return 0;
}
