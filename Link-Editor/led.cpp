/*
Reymond Pamelar, Tyler Nguyen
cssc3703, cssc3761
CS 530, Spring 2021
Assignment #2, XE Link-Editor
main.cpp
*/

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string.h>
#include <bits/stdc++.h>
#include <string>
#include <vector>

using namespace std;

void parseFile(istream&);
void checkMM();
void objFile();
void splitLine(string, const char*);
string genEstab(istream&);
string globalEstab = "";
int globalLen = 0;
string fileName;
int main(int argc, char* argv[]) {

    if (argc == 1) {
        cout << "No Arguments inputted." << endl;
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        string file = argv[i];
        fileName = argv[i];
        if (file.substr(file.find_last_of(".") + 1) == "lis") {

            //Parse file
            ifstream fp;
            fp.open(argv[i]);
            parseFile(fp);
            fp.close();

            //Check Memory Mapping
            checkMM();

            //Writing ESTAB into .st file
            ofstream file;
            file.open("estab.st");
            file << genEstab(fp);
            file.close();

            //Output object file
            objFile();
        }
    }
    return 0;
}

//--------------------------------------------------------------------------------

//A structure for the fields of each line
typedef struct listCols {
    string loc;
    string label;
    string op;
    string operand;
    string objCode;
}listCols;

vector<listCols> ListingFile;

//A structure for the references
typedef struct references {
    string reference;
}references;

vector<references> refList;

//--------------------------------------------------------------------------------

/*
Parses the file line by line and grabs the fields for the listCols structure
- Iterates through the line and grabs substrings for each field
*/
void parseFile(istream& f) {
    ListingFile.clear();
    string s;

    //Grabs each line and separates by columns to provide for the fields in listCols
    while (getline(f, s)) {
        int commentPos = s.find(".");

        //remove all comments
        if (commentPos > -1) {
            s = s.substr(0, commentPos);
            if (commentPos == 0) {
                continue;
            }
        }

        if (s.at(0) == ' ') {
            continue;
        }

        struct listCols line;
        size_t start = 0;
        size_t end = 0;

        //grab LOCS
        end = s.find(" ");
        if (start - end == 1) {
            line.loc = "";
        }
        line.loc = s.substr(start, end - start);

        //grab labels
        start = s.find_first_not_of(' ', end);
        if (start - end > 8) {
            end = start - 1;
            line.label = "";
        }
        else {
            end = s.find_first_of(" ", start);
            line.label = s.substr(start, end - start);
        }

        //grab OP Codes
        start = s.find_first_not_of(" ", end);
        end = s.find_first_of(" ", start);
        line.op = s.substr(start, end - start);

        //grab operands
        start = s.find_first_not_of(' ', end);
        if (start - end > 8) {
            end = start - 1;
            line.operand = " ";
        }
        else {
            end = s.find_first_of(' ', start);
            if (start > 1000) {
                line.operand = " ";
                ListingFile.push_back(line);
                continue;
            }
            line.operand = s.substr(start, end - start);
            line.operand.erase(std::remove_if(line.operand.begin(), line.operand.end(), ::isspace), line.operand.end());
        }

        //grab object codes
        if (end > 1000) {
            line.objCode = " ";
            line.objCode.erase(std::remove_if(line.objCode.begin(), line.objCode.end(), ::isspace), line.objCode.end());
            ListingFile.push_back(line);
            continue;
        }

        line.objCode = s.substr(end + 1);
        line.objCode.erase(std::remove_if(line.objCode.begin(), line.objCode.end(), ::isspace), line.objCode.end());

        ListingFile.push_back(line);
    }

    for (int i = 0; i < ListingFile.size(); i++) {
        std::stringstream stream;
        stream << std::setfill('0') << std::setw(6) << std::hex << ListingFile[i].loc;
        ListingFile[i].loc = stream.str();
    }
}


/*Perform check on memory mapping
- Convert each object code to binary
- Calculate each object code and match with the code in the listing file
- If object codes do not match, output ERROR and exit.
*/
void checkMM() {

    //List of EXTDEFs
    refList.clear();
    splitLine(ListingFile[1].operand, ",");
    std::vector<std::string> DEFs;
    for (int x = 0; x < refList.size(); x++) {
        DEFs.push_back(refList[x].reference);
    }

    //List of EXTREFs
    refList.clear();
    splitLine(ListingFile[2].operand, ",");
    std::vector<std::string> REFs;
    for (int x = 0; x < refList.size(); x++) {
        REFs.push_back(refList[x].reference);
    }

    //Iterate through each line with an object code field
    string bin;
    for (int x = 0; x < ListingFile.size(); x++) {
        long int i = 0;
        if (ListingFile[x].objCode != "") {

            //Convert object code to binary
            while (ListingFile[x].objCode[i]) {
                switch (ListingFile[x].objCode[i]) {
                case '0':
                    bin.append("0000");
                    break;
                case '1':
                    bin.append("0001");
                    break;
                case '2':
                    bin.append("0010");
                    break;
                case '3':
                    bin.append("0011");
                    break;
                case '4':
                    bin.append("0100");
                    break;
                case '5':
                    bin.append("0101");
                    break;
                case '6':
                    bin.append("0110");
                    break;
                case '7':
                    bin.append("0111");
                    break;
                case '8':
                    bin.append("1000");
                    break;
                case '9':
                    bin.append("1001");
                    break;
                case 'A':
                case 'a':
                    bin.append("1010");
                    break;
                case 'B':
                case 'b':
                    bin.append("1011");
                    break;
                case 'C':
                case 'c':
                    bin.append("1100");
                    break;
                case 'D':
                case 'd':
                    bin.append("1101");
                    break;
                case 'E':
                case 'e':
                    bin.append("1110");
                    break;
                case 'F':
                case 'f':
                    bin.append("1111");
                    break;
                default:
                    cout << "please enter valid hexadecimal digit " << ListingFile[x].objCode[i];
                    exit(1);
                }
                i++;
            }

            //Parse operand to find references
            string operand = ListingFile[x].operand;
            operand.erase(std::remove(operand.begin(), operand.end(), '#'), operand.end());
            operand.erase(std::remove(operand.begin(), operand.end(), '('), operand.end());
            operand.erase(std::remove(operand.begin(), operand.end(), ')'), operand.end());
            std::vector<std::string> OPs;

            string word = "";
            for (auto c : operand)
            {
                if (c == '+' || c == '-' || c == ',')
                {
                    if (c == ',') {
                        OPs.push_back(word);
                        word = "";
                        continue;
                    }
                    OPs.push_back(word);
                    word = "";
                }
                word = word + c;

                if (c == operand[operand.length() - 1]) {
                    OPs.push_back(word);
                    word = "";
                }
            }
            OPs.erase(std::remove(OPs.begin(), OPs.end(), ""), OPs.end());

            std::stringstream stream;

            //Convert to signed binary
            string d = bin.substr(12, bin.length() - 12);
            if (d.at(0) == '1') {
                for (int i = 0; i < d.length() - 1; i++) {
                    if (d[i] == '0') {
                        d[i] = '1';
                    }
                    else {
                        d[i] = '0';
                    }
                }
                bool first = false;
                for (int i = d.length() - 1; i >= 0; i--) {
                    if (d[i] == '0' && !first) {
                        first = true;
                        d[i] = '1';
                        break;
                    }
                    else if (d[i] == '1' && !first) {
                        first = true;
                        d[i] = '0';
                        continue;
                    }
                    if (d[i] == '1' && first) {
                        d[i] = '0';
                        continue;
                    }
                    if (d[i] == '0' && first) {
                        d[i] = '1';
                        break;
                    }
                }
            }
            unsigned long n = std::bitset<12>(d).to_ulong();

            //Grab NIXBPE bits
            cout << "[" << x << "]" << endl;
            string nBit, iBit, xBit, bBit, pBit, eBit;
            nBit = bin[6];
            iBit = bin[7];
            xBit = bin[8];
            bBit = bin[9];
            pBit = bin[10];
            eBit = bin[11];

            cout << bin << endl;
            cout << "NIXBPE BITS:\n" << nBit << iBit << xBit << bBit << pBit << eBit << endl;

            int bounds = std::stoi(ListingFile[ListingFile.size() - 1].loc, 0, 16);

            //Immediate Addressing 
            //n=0, i=1
            if (nBit == "0" && iBit == "1") {
                cout << "immediate" << endl;
            }

            //Indirect Addressing
            //n=1, i=0
            if (nBit == "1" && iBit == "0") {
                cout << "Indirect" << endl;
            }

            //Simple Addressing
            //n=0, i=0 or n=1, i=1
            if ((nBit == "0" && iBit == "0") || (nBit == "1" && iBit == "1")) {
                cout << "Simple" << endl;
            }

            //-------------------------------------------------------------
            //FORMAT 3 ONLY

            //Direct Addressing
            //b=0, p=0
            if (bBit == "0" && pBit == "0") {
                cout << "Direct" << endl;
            }

            //PC-Relative Addressing
            //b=0, p=1
            string progCount;
            int TA, PC, disp;
            TA = 0;
            if (bBit == "0" && pBit == "1") {
                cout << "PC-Relative" << endl;

                //Add or subtract addresses in Operand to find the target address
                for (int y = 0; y < OPs.size(); y++) {
                    string tmp = OPs[y];
                    tmp.erase(std::remove(tmp.begin(), tmp.end(), '-'), tmp.end());
                    tmp.erase(std::remove(tmp.begin(), tmp.end(), '+'), tmp.end());
                    for (int z = 0; z < ListingFile.size(); z++) {
                        if (ListingFile[z].label == tmp) {
                            int loc = std::stoi(ListingFile[z].loc, 0, 16);
                            if (OPs[y].at(0) == '-') {
                                TA -= loc;
                            }
                            else {
                                TA += loc;
                            }
                            break;
                        }
                    }
                    //For the case of adding/subtract numbers
                    if (!(std::find(REFs.begin(), REFs.end(), tmp) != REFs.end() || std::find(DEFs.begin(), DEFs.end(), tmp) != DEFs.end()) && tmp.find_first_not_of("0123456789") == string::npos) {
                        int loc = std::stoi(tmp);
                        if (OPs[y].at(0) == '-') {
                            TA -= loc;
                        }
                        else {
                            TA += loc;
                        }
                    }
                }

                //Subtract PC from TA unless TA is 0;
                std::string tmp;
                progCount = ListingFile[x + 1].loc;
                if (TA == 0) {
                    disp = 0;
                    if (d.length() == 12) {
                        tmp = std::bitset<12>(disp).to_string();
                    }
                    if (d.length() == 20) {
                        tmp = std::bitset<20>(disp).to_string();
                    }
                }
                else {
                    PC = std::stoi(progCount, 0, 16);
                    disp = TA - PC;
                    if (disp < 0) {
                        if (d.length() == 12) {
                            tmp = std::bitset<12>(disp).to_string();
                        }
                        if (d.length() == 20) {
                            tmp = std::bitset<20>(disp).to_string();
                        }
                        for (int i = 0; i < d.length() - 1; i++) {
                            if (tmp[i] == '0') {
                                tmp[i] = '1';
                            }
                            else {
                                tmp[i] = '0';
                            }
                        }
                        bool first = false;
                        for (int i = tmp.length() - 1; i < tmp.length(); i++) {
                            if (tmp[i] == '0' && !first) {
                                first = true;
                                tmp[i] = '1';
                                break;
                            }
                            else if (tmp[i] == '1' && !first) {
                                first = true;
                                tmp[i] = '0';
                                continue;
                            }
                            if (tmp[i] == '1' && first) {
                                tmp[i] = '0';
                                continue;
                            }
                            if (tmp[i] == '0' && first) {
                                tmp[i] = '1';
                                break;
                            }
                        }
                    }
                    else {
                        if (d.length() == 12) {
                            tmp = std::bitset<12>(disp).to_string();
                        }
                        if (d.length() == 20) {
                            tmp = std::bitset<20>(disp).to_string();
                        }
                    }
                }

                //Compare binary forms of calculated address and object code address. If they do not match, ERROR and exit
                if (tmp != d) {
                    cout << "PC-Relative Addressing ERROR at line:" << x << endl;
                    exit(1);
                }


            }

            //Base-Relative Addressing
            //b=1, p=0
            string base = "";
            string baseLoc = "";
            TA = 0;
            int b;
            if (bBit == "1" && pBit == "0") {
                cout << "Base-Relative" << endl;

                //Add or subtract addresses in Operand to find the target address
                for (int y = 0; y < OPs.size(); y++) {
                    string tmp = OPs[y];
                    tmp.erase(std::remove(tmp.begin(), tmp.end(), '-'), tmp.end());
                    tmp.erase(std::remove(tmp.begin(), tmp.end(), '+'), tmp.end());
                    for (int z = 0; z < ListingFile.size(); z++) {
                        if (ListingFile[z].label == tmp) {
                            int loc = std::stoi(ListingFile[z].loc, 0, 16);
                            if (OPs[y].at(0) == '-') {
                                TA -= loc;
                            }
                            else {
                                TA += loc;
                            }
                            break;
                        }
                    }
                    //For the case of adding/subtract numbers
                    if (!(std::find(REFs.begin(), REFs.end(), tmp) != REFs.end() || std::find(DEFs.begin(), DEFs.end(), tmp) != DEFs.end()) && tmp.find_first_not_of("0123456789") == string::npos) {
                        int loc = std::stoi(tmp);
                        if (OPs[y].at(0) == '-') {
                            TA -= loc;
                        }
                        else {
                            TA += loc;
                        }
                    }
                }
                //Find the BASE address
                for (int y = 0; y < ListingFile.size(); y++) {
                    if (ListingFile[y].op == "BASE") {
                        base = ListingFile[y].operand;
                        for (int z = 0; z < ListingFile.size(); z++) {
                            if (ListingFile[z].label == base) {
                                baseLoc = ListingFile[z].loc;
                                break;
                            }
                        }
                        break;
                    }
                }

                //Subtract B from TA unless TA is 0;
                std::string tmp;
                if (TA == 0) {
                    disp = 0;
                    if (d.length() == 12) {
                        tmp = std::bitset<12>(disp).to_string();
                    }
                    if (d.length() == 20) {
                        tmp = std::bitset<20>(disp).to_string();
                    }
                }
                else {
                    b = std::stoi(baseLoc, 0, 16);
                    disp = TA - b;
                    if (disp < 0) {
                        if (d.length() == 12) {
                            tmp = std::bitset<12>(disp).to_string();
                        }
                        if (d.length() == 20) {
                            tmp = std::bitset<20>(disp).to_string();
                        }
                        for (int i = 0; i < d.length() - 1; i++) {
                            if (tmp[i] == '0') {
                                tmp[i] = '1';
                            }
                            else {
                                tmp[i] = '0';
                            }
                        }
                        bool first = false;
                        for (int i = tmp.length() - 1; i < tmp.length(); i++) {
                            if (tmp[i] == '0' && !first) {
                                first = true;
                                tmp[i] = '1';
                                break;
                            }
                            else if (tmp[i] == '1' && !first) {
                                first = true;
                                tmp[i] = '0';
                                continue;
                            }
                            if (tmp[i] == '1' && first) {
                                tmp[i] = '0';
                                continue;
                            }
                            if (tmp[i] == '0' && first) {
                                tmp[i] = '1';
                                break;
                            }
                        }
                    }
                    else {
                        if (d.length() == 12) {
                            tmp = std::bitset<12>(disp).to_string();
                        }
                        if (d.length() == 20) {
                            tmp = std::bitset<20>(disp).to_string();
                        }
                    }
                }

                //Compare binary forms of calculated address and object code address. If they do not match, ERROR and exit
                if (tmp != d) {
                    cout << "Base-Relative Addressing ERROR at line:" << x << endl;
                    exit(1);
                }
            }
            bin = "";
            cout << endl;
        }
    }
    cout << "-----------------------------------------------------------------------------" << endl;
}
//generate External Symbol table (ESTAB)
string genEstab(istream& f) {
    cout << "***ESTAB FILE***" << endl << endl;
    refList.clear();
    std::stringstream stream;
    // header estab
    string estab = "";
    //converts length of file from string to int and adding 3 for END length
    string endLength = ListingFile[ListingFile.size() - 1].loc;
    string startingAddr = ListingFile[0].loc;
    stringstream conv;
    stringstream conv2;
    conv << endLength;
    conv2 << startingAddr;
    int i;
    int j;
    conv >> std::hex >> i;
    conv2 >> std::hex >> j;
    int length = i + 3;
    int lenn = j + globalLen;
    globalLen += length;
    char hex_string[20];
    char hex_string2[20];
    char hex_string3[20];
    sprintf(hex_string, "%X", length);
    sprintf(hex_string2, "%X", globalLen);
    sprintf(hex_string3, "%X", lenn);
    string endLen = hex_string;
    string startAddr = hex_string3;
    //grab program name, initial starting address, and length of the program
    stream << ListingFile[0].label << std::setfill(' ') << std::setw(13 - ListingFile[0].label.length()) << " " << std::setfill('0') << std::setw(6) << startAddr << " " << std::setfill('0') << std::setw(6) << endLen << "\n";
    estab.append(stream.str());

    //label references
    for (int i = 0; i < ListingFile.size(); i++) {
        std::stringstream stream;
        stream << std::setfill('0') << std::setw(6) << std::hex << ListingFile[i].loc;
        if (ListingFile[i].op == "EXTDEF") {
            splitLine(ListingFile[i].operand, ",");
            for (int x = 0; x < refList.size(); x++) {
                stream.str("");
                //initial spacing for references
                stream << std::setfill(' ') << std::setw(12) << refList[x].reference << " ";
                for (int j = 0; j < ListingFile.size(); j++) {
                    if (ListingFile[j].label == refList[x].reference) {
                        /*
                        1. finds all the reference labels
                        2. grabs location of the labels
                        3. adds this location to the current address of the head label
                        4. conversion from string to int to hex
                        */
                        string labels = ListingFile[j].loc;
                        stringstream conv;
                        conv << labels;
                        int p;
                        conv >> std::hex >> p;
                        int def = lenn + p;
                        char hex[20];
                        sprintf(hex, "%X", def);
                        string location = hex;
                        stream << std::setfill('0') << std::setw(6) << location << "\n";
                    }
                }
                estab.append(stream.str());
            }
        }
        ListingFile[i].loc = stream.str();
    }
    cout << estab << endl;
    //global variable tracker to write ESTAB into .st file
    globalEstab += estab;

    cout << "-----------------------------------------------------------------------------" << endl;

    return globalEstab;
}
//Make Object file
void objFile() {
    cout << "***OBJECT FILE***" << endl << endl;
    std::stringstream stream;

    //Header
    string objF = "H";
    objF.append(ListingFile[0].label);
    stream << " " << ListingFile[0].loc;
    objF.append(stream.str());
    string lastLoc = ListingFile[ListingFile.size() - 1].loc;
    int num = std::stoi(lastLoc, 0, 16);
    num += 3;
    stream.str("");
    stream << setfill('0') << setw(6) << std::hex << num;
    objF.append(stream.str());

    //D
    objF.append("\nD");
    stream.str("");
    refList.clear();
    splitLine(ListingFile[1].operand, ",");
    for (int x = 0; x < refList.size(); x++) {
        stream.str("");
        objF.append(refList[x].reference);
        for (int y = 0; y < ListingFile.size(); y++) {
            if (refList[x].reference == ListingFile[y].label) {
                stream << " " << ListingFile[y].loc;
                objF.append(stream.str());
            }
        }
    }

    //R
    objF.append("\nR");
    stream.str("");
    refList.clear();
    splitLine(ListingFile[2].operand, ",");
    objF.append(refList[0].reference);
    for (int x = 1; x < refList.size(); x++) {
        stream.str("");
        stream << " " << refList[x].reference;;
        objF.append(stream.str());
    }

    /*Text Records
    - Output the starting address
    - Find the length: Iterate through each line of object code until a RESW, RESB, EQU, END, or =C'EOF' is reached or if the length is >=30
    - Output records: Iterate through each line of object code until a RESW, RESB, EQU, END, or =C'EOF' is reached or if the length is >=30
    */
    bool firstRecord = false;
    int startObjCodeIdx = 0;
    string lastObjCode = "";
    while (startObjCodeIdx <= ListingFile.size()) {
        stream.str("");

        if (startObjCodeIdx >= ListingFile.size() - 1) {
            break;
        }
        if (ListingFile[startObjCodeIdx].op == "RESW" || ListingFile[startObjCodeIdx].op == "RESB" || ListingFile[startObjCodeIdx].op == "EQU") {
            startObjCodeIdx++;
            continue;
        }

        if (ListingFile[startObjCodeIdx + 1].op == "END" || ListingFile[startObjCodeIdx + 1].op == "=C'EOF'" || startObjCodeIdx == ListingFile.size() - 1) {
            lastObjCode = ListingFile[ListingFile.size() - 1].loc;
        }

        //T^<start addr>^<length>
        objF.append("\nT");
        if (firstRecord == false) {
            for (int x = startObjCodeIdx; x < ListingFile.size(); x++) {
                if (ListingFile[x].objCode != "") {
                    startObjCodeIdx = x;
                    stream << setfill('0') << setw(6) << std::hex << ListingFile[x].loc;
                    objF.append(stream.str());
                    //firstRecord = true;
                    break;
                }
            }
        }

        //Find the length
        stream.str("");
        int length = 0;
        for (int x = startObjCodeIdx; x < ListingFile.size(); x++) {
            if (ListingFile[x].objCode.length() == 6) {
                length += 3;
            }
            if (ListingFile[x].objCode.length() == 8) {
                length += 4;
            }
            if (length > 30 || ListingFile[x].op == "RESW" || ListingFile[x].op == "RESB" || ListingFile[x].op == "=C'EOF'" || ListingFile[x].op == "EQU" || ListingFile[x].op == "END") {
                if (ListingFile[x].objCode.length() == 6 && ListingFile[x].op != "=C'EOF'") {
                    length -= 3;
                }
                if (ListingFile[x].objCode.length() == 8 && ListingFile[x].op != "=C'EOF'") {
                    length -= 4;
                }
                stream << setfill('0') << setw(2) << std::hex << length;
                objF.append(stream.str());
                lastObjCode = ListingFile[startObjCodeIdx].loc;
                break;
            }
        }


        //Output Object Codes
        stream.str("");
        string initObj = ListingFile[startObjCodeIdx].loc;
        int length2 = 0;
        for (int x = startObjCodeIdx; x < ListingFile.size(); x++) {
            if (ListingFile[x].objCode.length() == 6) {
                length2 += 3;
            }
            if (ListingFile[x].objCode.length() == 8) {
                length2 += 4;
            }
            if (length2 > 30 || ListingFile[x].op == "RESW" || ListingFile[x].op == "RESB" || ListingFile[x].op == "EQU") {

                int num = std::stoi(initObj, 0, 16);
                num += length;
                stream << setfill('0') << setw(6) << std::hex << num;
                lastObjCode = stream.str();
                break;
            }
            if (ListingFile[x].objCode != "") {
                objF.append(ListingFile[x].objCode);
                startObjCodeIdx++;
            }
        }
        startObjCodeIdx++;
    }

    /*Modification Records
    - Iterate through all operands utilizing an EXTDEF or EXTREF
    - Output location along with the operand(s)
    - All EXTDEFs are replaced with the program name
    - Multiple EXTDEFs in the same operand are cancelled out
    */
    bool objCodePrinted = false;
    for (int i = 3; i < ListingFile.size(); i++) {
        stream.str("");

        //format 4
        if (ListingFile[i].objCode.length() == 8) {

            //Parse operand to find references
            string operand = ListingFile[i].operand;
            operand.erase(std::remove(operand.begin(), operand.end(), '#'), operand.end());
            operand.erase(std::remove(operand.begin(), operand.end(), '('), operand.end());
            operand.erase(std::remove(operand.begin(), operand.end(), ')'), operand.end());
            std::vector<std::string> OPs;

            string word = "";
            for (auto c : operand)
            {
                if (c == '+' || c == '-' || c == ',')
                {
                    if (c == ',') {
                        OPs.push_back(word);
                        word = "";
                        continue;
                    }
                    OPs.push_back(word);
                    word = "";
                }
                word = word + c;

                if (c == operand[operand.length() - 1]) {
                    OPs.push_back(word);
                    word = "";
                }
            }

            OPs.erase(std::remove(OPs.begin(), OPs.end(), ""), OPs.end());

            bool first = true;
            for (int y = 0; y < OPs.size(); y++) {
                stream.str() = "";
                refList.clear();
                splitLine(ListingFile[2].operand, ",");
                bool first = true;
                std::vector<std::string> REFs;
                for (int x = 0; x < refList.size(); x++) {
                    REFs.push_back(refList[x].reference);
                }
                string tmp = OPs[y];
                tmp.erase(std::remove(tmp.begin(), tmp.end(), '-'), tmp.end());
                tmp.erase(std::remove(tmp.begin(), tmp.end(), '+'), tmp.end());

                //+ External Reference
                if (std::find(REFs.begin(), REFs.end(), tmp) != REFs.end()) {
                    objF.append("\nM");
                    int num = std::stoi(ListingFile[i].loc, 0, 16);
                    num++;
                    stream << setfill('0') << setw(6) << std::hex << num;
                    objF.append(stream.str());
                    stream.str("");
                    objF.append("05");
                    if (OPs[y].at(0) != '-' && first) {
                        objF.append("+");
                        first = false;
                    }
                    objF.append(OPs[y]);
                }
                else {
                    //+ Program name
                    objF.append("\nM");
                    int num = std::stoi(ListingFile[i].loc, 0, 16);
                    num++;
                    stream << setfill('0') << setw(6) << std::hex << num;
                    objF.append(stream.str());
                    stream.str("");
                    objF.append("05+");
                    objF.append(ListingFile[0].label);

                }
            }
        }

        //External References
        if (ListingFile[i].objCode.length() == 6) {
            string operand = ListingFile[i].operand;
            operand.erase(std::remove(operand.begin(), operand.end(), '#'), operand.end());
            operand.erase(std::remove(operand.begin(), operand.end(), '('), operand.end());
            operand.erase(std::remove(operand.begin(), operand.end(), ')'), operand.end());
            std::vector<std::string> OPs;
            OPs.clear();

            //Parse operand to find references
            string word = "";
            for (auto c : operand)
            {
                if (c == '+' || c == '-' || c == ',')
                {
                    if (c == ',') {
                        OPs.push_back(word);
                        word = "";
                        continue;
                    }
                    OPs.push_back(word);
                    word = "";
                    word = word + c;
                    continue;
                }
                word = word + c;

                if (c == operand[operand.length() - 1]) {
                    OPs.push_back(word);
                    word = "";
                }

            }

            OPs.erase(std::remove(OPs.begin(), OPs.end(), ""), OPs.end());

            stream.str("");
            refList.clear();
            splitLine(ListingFile[1].operand, ",");
            bool first = true;
            std::vector<std::string> REFs;
            for (int x = 0; x < refList.size(); x++) {
                REFs.push_back(refList[x].reference);
            }

            int count = 0;
            for (int a = 0; a < OPs.size(); a++) {
                string tmp = OPs[a];
                tmp.erase(std::remove(tmp.begin(), tmp.end(), '-'), tmp.end());
                tmp.erase(std::remove(tmp.begin(), tmp.end(), '+'), tmp.end());
                if (std::find(REFs.begin(), REFs.end(), tmp) != REFs.end()) {
                    OPs[a] = ListingFile[0].label;
                    count++;
                }
                if (count == 2) {
                    OPs.erase(std::remove(OPs.begin(), OPs.end(), ListingFile[0].label), OPs.end());
                }
            }

            for (int y = 0; y < OPs.size(); y++) {
                stream.str() = "";
                refList.clear();
                splitLine(ListingFile[2].operand, ",");
                bool first = true;
                REFs.clear();
                for (int x = 0; x < refList.size(); x++) {
                    REFs.push_back(refList[x].reference);
                }

                string tmp = OPs[y];
                tmp.erase(std::remove(tmp.begin(), tmp.end(), '-'), tmp.end());
                tmp.erase(std::remove(tmp.begin(), tmp.end(), '+'), tmp.end());

                //+ External Reference
                if (std::find(REFs.begin(), REFs.end(), tmp) != REFs.end()) {
                    objF.append("\nM");
                    int num = std::stoi(ListingFile[i].loc, 0, 16);
                    stream << setfill('0') << setw(6) << std::hex << num;
                    objF.append(stream.str());
                    stream.str("");
                    objF.append("06");
                    if (OPs[y].at(0) != '-' && first && OPs[y].at(0) != '+') {
                        objF.append("+");
                        first = false;
                    }
                    objF.append(OPs[y]);
                }
                else {
                    //+ Program name
                    stream.str() = "";
                    refList.clear();
                    splitLine(ListingFile[1].operand, ",");
                    std::vector<std::string> REFs;
                    for (int x = 0; x < refList.size(); x++) {
                        REFs.push_back(refList[x].reference);
                    }
                    if ((std::find(REFs.begin(), REFs.end(), tmp) != REFs.end() || tmp == ListingFile[0].label) && ListingFile[i].op == "WORD") {

                        objF.append("\nM");
                        int num = std::stoi(ListingFile[i].loc, 0, 16);
                        stream << setfill('0') << setw(6) << std::hex << num;
                        objF.append(stream.str());
                        stream.str("");
                        objF.append("06");
                        if (OPs[y].at(0) != '-' && first && OPs[y].at(0) != '+') {
                            objF.append("+");
                            first = false;
                        }
                        objF.append(ListingFile[0].label);
                    }
                }
            }
        }
    }

    //E
    objF.append("\nE");
    stream.str("");
    for (int i = 0; i < ListingFile.size(); i++) {
        if (ListingFile[i].objCode.length() == 6 || ListingFile[i].objCode.length() == 8) {
            stream << setfill('0') << setw(6) << std::hex << ListingFile[i].loc;
            objF.append(stream.str());
            break;
        }
    }

    for (int i = 0; i < objF.length(); i++) {
        if (objF[i] >= 'a' && objF[i] <= 'z') {
            objF[i] -= 32;
        }
    }

    //Output to file
    fileName = fileName.substr(0, fileName.find("."));
    fileName.append(".obj");
    ofstream objFile(fileName);
    objFile << objF;
    objFile.close();

    cout << objF << endl;
    cout << endl;
}

//Divides a string into tokens based on the delimiter given. Tokens are placed in the refList structure.
void splitLine(string s, const char* x) {
    s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
    s.erase(s.find_last_not_of("\n\r") + 1);
    int n = s.length();
    char currLine[n + 1];
    strcpy(currLine, s.c_str());
    char* token = strtok(currLine, x);
    while (token != NULL) {
        struct references r;
        r.reference = token;
        token = strtok(NULL, x);
        refList.push_back(r);
    }
}