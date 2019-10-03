#include <fstream>
#include <codecvt>
#include <iostream>
#include <vector>
#include <string>
#include <locale>
#include <algorithm>
#include <iterator>
#include <set>
#include <ctime>

using namespace std;

int shingleLen; // длина шингла

string tolower_ru(string str){
  string low [] = {"а", "б", "в", "г", "д", "е", "ё", "ж", "з", "и", "й", "к", "л", "м", "н", "о", "п", "р", "с", "т", "у", "ф"," х", "ц", "ч", "ш", "щ", "ъ", "ы", "ь", "э", "ю", "я"};
  string up [] = {"А", "Б", "В", "Г", "Д", "Е", "Ё", "Ж", "З", "И", "Й", "К", "Л", "М", "Н", "О", "П", "Р", "С", "Т", "У", "Ф", "Х", "Ц", "Ч", "Ш", "Щ", "Ъ", "Ы", "Ь", "Э", "Ю", "Я"};
  for (int i = 0; i < 33; i++){
    int point = str.find(up[i]);
    while (point != -1){
      str.replace(point, up[i].length(), low[i]);
      point = str.find(up[i]);
    }
  }
  locale loc;
  string result = "";
  for (string::size_type i=0; i<str.length(); ++i)
    result += tolower(str[i], loc);
  return result;
}

unsigned int CRC32(string source){
  unsigned char *buf = reinterpret_cast<unsigned char*>(const_cast<char*>(source.c_str()));
  unsigned long len = source.length();
	unsigned long crc_table[256];
	unsigned long crc;
	for (int i = 0; i < 256; i++)
	{
		crc = i;
		for (int j = 0; j < 8; j++)
			crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;
		crc_table[i] = crc;
	};
	crc = 0xFFFFFFFFUL;
	while (len--)
	  crc = crc_table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);
	return crc ^ 0xFFFFFFFFUL;
}

string readFile(const string& fileName) {
    ifstream f(fileName);
    f.seekg(0, ios::end);
    size_t size = f.tellg();
    string s(size, ' ');
    f.seekg(0);
    f.read(&s[0], size);
    return s;
}

vector<string> canonize(string source){
  char stop_symbols[] = ".,!?:;-\n\r()";
  string stop_words[] = {
    "это", "как", "так", 
    "и", "в", "над",
    "к", "до", "не",
    "на", "но", "за",
    "то", "с", "ли",
    "а", "во", "от",
    "со", "для", "о",
    "же", "ну", "вы",
    "бы", "что", "кто",
    "он", "она"
  }; // Вводятся быть в нижнем регистре
  source = tolower_ru(source);
  source += " "; // Для корректного разделения на слова
  for (char i : stop_symbols){
    int point = source.find(i);
    while (point != -1){
      source.replace(source.find(i), 1, " ");
      point = source.find(i);
    }
  }
  for (string i : stop_words){
    i = " "+i+" ";
    int point = source.find(i);
    while (point != -1){
      source.replace(point, i.length(), " ");
      point = source.find(i);
    }
  }
  vector<string>result;
  string buffer = ""; 
  for(int i = 0; i < source.size(); i++){
    if(source[i] != ' '){
      buffer += source[i];     
    }
    else if (buffer.size() > 0){
      result.push_back(buffer);
      buffer = "";
    }
  }
  return result;
}

multiset<unsigned int> genshingle(vector<string> source){
  multiset<unsigned int> result;
  for (int i = 0; i <= source.size() - shingleLen; i++){
    string temp = source[i];
    for (int j = i+1; j < i + shingleLen; j++)
      temp += " " + source[j];
    result.insert(CRC32(temp));
  }
  return result;
}

double compaire(multiset<unsigned int>cmp1, multiset<unsigned int>cmp2){
  multiset<unsigned int>intersection;
  set_intersection(cmp1.begin(), cmp1.end(), cmp2.begin(), cmp2.end(), inserter(intersection, intersection.begin()));
  return 200.0*intersection.size()/(cmp1.size() + cmp2.size());
}

int main() {
  clock_t time_req;
	time_req = clock();
  shingleLen = 3; //(слов в сообщении) >= длина шингла > 0
  setlocale(LC_ALL, "Russian");
  string text1 = readFile("text1.txt");
  string text2 = readFile("text2.txt");
  cout << compaire(genshingle(canonize(text1)), genshingle(canonize(text2))) << " %\n";
  time_req = clock() - time_req;
	cout << "Потрачено: " << (float)time_req/CLOCKS_PER_SEC << " секунд" << "\n";
}