#include <iostream>
#include <string.h>
#include <cstdlib>
using namespace std;

typedef unsigned int uint;

const int NO_ERROR = 0;
const int ERROR_UNKNOWN_ERROR = -1;
const int ERROR_INVALID_CODE_LENGTH = -2;
const int ERROR_UNKNOWN_KEYWORD = -3;
const int ERROR_UNCLOSED_LOOP = -4;
const int ERROR_BAD_ACCESS = -5;
const uint BLOCK_SIZE = 3;
const uint CODE_MAX_LENGTH = 1024 * BLOCK_SIZE;
const uint MEM_MAX_LENGTH = 30000; // bf standart

char userkey;
int result;
string code; // code
uint codelen; // code length in chnars
char *mem; // memory
int memptr; // current mem cell position
int R; // result register
bool P; // predicate register

void BadEnd(string errmsg) {
    cout << endl << errmsg << endl << "Press any key for continue\n";
    cin.get();
}

// console input
int Input(string &buff) {
    cin >> buff;
    return buff.size();
}

bool IsValidCodeSize(const int size) {
    return (size % BLOCK_SIZE == 0);
}

void Init() {
    R = 0;
    P = false;
    mem = new char[MEM_MAX_LENGTH];
    memptr = 0;
}

void ClearMem() {
    for (int i=0;i<MEM_MAX_LENGTH;i++) {
        mem[i] = 0;
    }
}

bool LazyTest() {
    string keywords[] = {
        // bf legacy
        "dec", // decrement => bf -
        "inc", // increment => bf +
        "lft", // left => bf <
        "rht", // right => bf >
        "out", // output => bf .
        "inp", // input => bf ,
        "nin", // next if NULL => bf [
        "bim", // go if more => bf ]
        // extension
        "add", // R = a + b
        "sub", // R = a - b
        "mul", // R = a * b
        "div", // R = a / b
        "mod", // R = a % b
        "get", // cin >> R
        "res", // a = R
        "eql", // P = (a == b)
        "ife", // if (!P) goto next+1
        "end", // exit
        "fnl", // a = 0
        "set", // R = a
        "nou", // cout << (int)a
        "and", // R = a & b
        "orr", // R = a | b
        "xor", // R = a ^ b
        "les", // P = (a > b)
        "mor", // P = (a < b)
        "cls", // system("cls")
        "rnd", // a = -127 + rnd % 255 
        "oul", // cout << a << endl
        "inn", // cin >> (int)a
    };
    int kwlen = 30; // shia_labeouf_magic.gif
    string sub = new char[BLOCK_SIZE];
    bool res = true;
    for (int i=0;i<codelen;i+=BLOCK_SIZE) {
        sub = code.substr(0, BLOCK_SIZE);
        bool iseq = false;
        for (int j=0;j<kwlen;j++) {
            if (sub == keywords[j]) {
                iseq = true;
                break;
            }
        }
        if (!iseq) {
            res = false;
            break;
        }
    }
    sub.clear();
    for (int i=0;i<kwlen;i++) {
        keywords[i].clear();
    }
    return res;
}

int Interpret() {
    cout << endl;
    string sub;
    int lvl;
    for (int com=0;com<codelen;com+=BLOCK_SIZE) {
        sub = code.substr(com,BLOCK_SIZE);
        // bf functions
        if (sub=="inc") {
            mem[memptr]++;
        } else if (sub=="dec") {
            mem[memptr]--;
        } else if (sub=="lft") {
           if (--memptr < 0) memptr = MEM_MAX_LENGTH-1;
        } else if (sub=="rht") {
            if (++memptr >= MEM_MAX_LENGTH) memptr = 0;
        } else if (sub=="out") {
            cout << mem[memptr];
        } else if (sub=="inp") {
            cin >> mem[memptr];
        } else if (sub=="nin") {
            if (mem[memptr]!=0) continue;
            com+=BLOCK_SIZE;
            if (com >= CODE_MAX_LENGTH) {
                BadEnd("Unclosed loop");
                return ERROR_UNCLOSED_LOOP;
            }
            while (lvl != 0) {
                string temp = code.substr(com,BLOCK_SIZE);
                if (temp == "nin") lvl++;
                else if (temp == "bim") lvl--;
                temp.clear();
                com+=BLOCK_SIZE;
            }
        } else if (sub=="bim") {
            if (mem[memptr]==0) continue;
            com-=BLOCK_SIZE;
            if (com < 0) {
                BadEnd("Unclosed loop");
                return ERROR_UNCLOSED_LOOP;
            }
            while (lvl != 0) {
                string temp = code.substr(com,BLOCK_SIZE);
                if (temp == "nin") lvl--;
                else if (temp == "bim") lvl++;
                temp.clear();
                com+=BLOCK_SIZE;
            }
        }
        // custom functions
        else if (sub=="add") { // set R register as additional this and previus cells
            if (memptr == 0) {
                R = (int)mem[MEM_MAX_LENGTH-1] + mem[memptr];
            } else {
                R = (int)mem[memptr-1] + mem[memptr];
            }
        }
        else if (sub=="sub") { // set R register as subscrible this and previus cells
            if (memptr == 0) {
                R = (int)mem[MEM_MAX_LENGTH-1] - mem[memptr];
            } else {
                R = (int)mem[memptr-1] - mem[memptr];
            }
        }
        else if (sub=="mul") { // set R register as multiple this and previus cells
            if (memptr == 0) {
                R = (int)mem[MEM_MAX_LENGTH-1] * mem[memptr];
            } else {
                R = (int)mem[memptr-1] * mem[memptr];
            }
        } 
        else if (sub=="div") { // set R register as divide this and previus cells
            if (memptr == 0) {
                R = (int)mem[MEM_MAX_LENGTH-1] / mem[memptr];
            } else {
                R = (int)mem[memptr-1] / mem[memptr];
            }
        } 
        else if (sub=="mod") { // set R register as modulo this and previus cells
            if (memptr == 0) {
                R = (int)mem[MEM_MAX_LENGTH-1] % mem[memptr];
            } else {
                R = (int)mem[memptr-1] % (int)mem[memptr];
            }
        } 
        else if (sub=="get") { // set R register as user input integer
            cin >> R;
        } 
        else if (sub=="res") { // set this cells value from R register
            if (R < CHAR_MIN) mem[memptr] = CHAR_MIN;
            else if (R > CHAR_MAX) mem[memptr] = CHAR_MAX;
            mem[memptr] = R;
        }
        else if (sub=="eql") { // set P as this and previus cells equals
            if (memptr == 0) {
                P = mem[MEM_MAX_LENGTH-1] == mem[memptr];
            } else {
                P = mem[memptr-1] == mem[memptr];
            }
        }
        else if (sub=="ife") { // if P register is true ignore next command
            if (!P) {
                if (com+BLOCK_SIZE >= codelen) {
                    BadEnd("Bad access");
                    return ERROR_BAD_ACCESS;
                } else {
                    com+=BLOCK_SIZE;
                }
            }
        } 
        else if (sub=="end") { // end interpret without errors
            return NO_ERROR;
        } 
        else if (sub=="fnl") { // set cell as 0
            mem[memptr] = 0;
        } 
        else if (sub=="set") { // set R register value from this cell
            R = mem[memptr];
        } 
        else if (sub=="nou") { // print this cell value like num
            cout << (int)mem[memptr];
        } 
        else if (sub=="and") { // set R register as logical "and" this and previus cells
            if (memptr == 0) {
                R = (int)mem[MEM_MAX_LENGTH-1] & mem[memptr];
            } else {
                R = (int)mem[memptr-1] & mem[memptr];
            }
        } 
        else if (sub=="orr") { // set R register as logical "or" this and previus cells
            if (memptr == 0) {
                R = (int)mem[MEM_MAX_LENGTH-1] | mem[memptr];
            } else {
                R = (int)mem[memptr-1] | mem[memptr];
            }
        } 
        else if (sub=="xor") { // set R register as logical "xor" this and previus cells
            if (memptr == 0) {
                R = (int)mem[MEM_MAX_LENGTH-1] ^ mem[memptr];
            } else {
                R = (int)mem[memptr-1] ^ mem[memptr];
            }
        }
        else if (sub=="les") { // set P as this cell less previus cell
            if (memptr == 0) {
                P = mem[MEM_MAX_LENGTH-1] > mem[memptr];
            } else {
                P = mem[memptr-1] > mem[memptr];
            }
        }
        else if (sub=="mor") { // set P as this cell more previus cell
            if (memptr == 0) {
                P = mem[MEM_MAX_LENGTH-1] < mem[memptr];
            } else {
                P = mem[memptr-1] < mem[memptr];
            }
        }
        else if (sub=="cls") { // clear console screen
            system("cls");
        } 
        else if (sub=="rnd") { // set this cell as random value
            mem[memptr] = -127 + (rand() % 255);
        } 
        else if (sub=="oul") { // print this cell char with endline
            cout << mem[memptr] << endl;
        } 
        else if (sub=="inn") { // set this cell int value as user input
            int i=0;
            cin >> i;
            if (i < CHAR_MIN) i = CHAR_MIN;
            else if (i > CHAR_MAX) i = CHAR_MAX;
            mem[memptr] = i;
        } 
        else {
            // ignore
        }
    }
    cout << endl;
    return NO_ERROR;
}

void ClearData() {
    code.clear();
    delete[] mem;
}

int main() {
    start:
        cout << "Press enter your Ter code\n";
        codelen = Input(code);
	if (!IsValidCodeSize(codelen)) {
            BadEnd("Invalid code length");
            result = ERROR_INVALID_CODE_LENGTH;
            goto exitdialog;
	}
        Init();
        if (!LazyTest()) {
            BadEnd("Unknown keyword");
            result = ERROR_UNKNOWN_KEYWORD;
            goto exitdialog;           
        }
        ClearMem();
        result = Interpret();
        ClearData();
    exitdialog:
        if (result == 0) {
            cout << "Success! Do you have restart? Press <y> or <n>\n";
        } else {
            cout << "Error " << (result * -1) << "! Do you have restart? Press <y> or <n>\n";
        }
	cin >> userkey;
        userkey = tolower(userkey);
        if (userkey == 'y') {
            goto start;
        } else if (userkey == 'n') {
            return result;
        } else {
            goto exitdialog;
        }
	return result; // no way
}
