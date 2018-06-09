#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

using namespace std;

int make_int(char ch)//перетворення символу в ціле число
{
	int tmp = (int)ch;
	if (tmp < 0) tmp += 256;
	return tmp;
}

string* resize(string* arr, int* sz)//зміна розміру масиву
{
	string* tmp = new string[*sz * 2];
	for (int i = 0; i < *sz; i++)
		tmp[i] = arr[i];
	delete[] arr;
	*sz *= 2;
	return tmp;
}

void upt(int* counter, int* lth)//оновлення лічильника
{
	if (*counter == (int)pow(2, *lth - 1))
	{
		(*lth)++;
		*counter = 0;
	}
	(*counter)++;
}
string cut_item(string *str)//вирізання з рядка 1 байта
{
	string tmp = (*str).substr(0, 8);
	(*str).erase(0, 8);
	return tmp;
}
string make_bin(int val, int lth)//перетворення десяткового числа в двійкове
{
	if (lth == 0) return "";
	string tmp = "00000000";
	while (tmp.size() < lth) tmp += tmp;
	int i = lth - 1;
	while (val > 0)
	{
		if (val % 2) tmp[i] = '1';
		else tmp[i] = '0';
		val /= 2;
		i--;
	}
	return tmp.substr(0, lth);
}
int to_ten(string bin)//перетворення двійкового в десякове
{
	string tmp;
	int val = 0;
	for (int i = 0; i <= bin.size(); i++)
	{
		if (bin[i] != '0')
			val += (int)pow(2, (int)bin.size() - i - 1);
	}
	return val;
}
void compress(char out[], char in[])
{
	ofstream a;
	ifstream f;
	f.open(in, ios::binary);
	a.open(out, ios::binary);
	map<string, int> dict = { {"", 0} };
	char ch;
	string sub;
	string bin;
	int lth = 0;
	int counter = 0;
	a << in << "\r\n";
	while (f.get(ch))
	{
		sub += ch;
		if (!dict[sub])
		{
			dict[sub] = dict.size()-1;
			string sym = make_bin(make_int(ch), 8);
			sub.pop_back();
			bin += make_bin((dict[sub]), lth) + sym;

			while (bin.size() > 8)
				a << (char)to_ten(cut_item(&bin));
			sub.clear();
			upt(&counter, &lth);
		}
	}
	if (sub != "")
		bin += make_bin(dict[sub], lth);
	while (bin.size() > 8)
		a << (char)to_ten(cut_item(&bin));
	int sz = bin.size();
	a << (char)to_ten(bin) << sz;
	f.close();
	a.close();
	remove(in);
}
void decompress(char out[])
{
	ofstream output;
	ifstream input;
	input.open(out, ios::binary);
	string name;
	int pos = 0;
	char ch;
	while (input.get(ch))
	{
		if (input.peek() == '\n')
		{
			pos = (int)input.tellg() + 1;
			break;
		}
		name += ch;
	}
	output.open(name, ios::binary);
	string* dict;
	int dict_size = 10;
	dict = new string[dict_size];
	dict[0] = "";
	int dict_index = 1;
	int counter = 0;
	int lth = 0;
	string bin;
	input.seekg(0);
	input.seekg(-1, input.end);
	input.get(ch);
	int val = ch - '0';
	int last = (int)(input.tellg()) - 1;
	input.seekg(pos);
	while (input.get(ch))
	{
		if ((int)input.tellg() != last)
		{
			bin += make_bin(make_int(ch), 8);
		}
		else
		{
			bin += make_bin(make_int(ch), 8).erase(0, 8 - val);
			break;
		}
		if (bin.size() >= 8 + lth)
		{
			string num = bin.substr(0, lth);
			bin.erase(0, lth);
			string sym = cut_item(&bin);
			string res = dict[to_ten(num)] + (char)to_ten(sym);
			if (dict_index + 1 == dict_size)
				dict = resize(dict, &dict_size);
			dict[dict_index] = res;
			dict_index++;
			output << res;
			upt(&counter, &lth);
		}
	}
	if (bin.size() >= 8 + lth)
	{
		string num = bin.substr(0, lth);
		bin.erase(0, lth);
		string sym = cut_item(&bin);
		string res = dict[to_ten(num)] + (char)to_ten(sym);
		if (dict_index + 1 == dict_size)
			dict = resize(dict, &dict_size);
		dict[dict_index] = res;
		dict_index++;
		output << res;
	}
	if (bin != "")
		output << dict[to_ten(bin)];
	input.close();
	output.close();
	remove(out);
	delete[] dict;
}
int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		cout << "invalid input" << endl;
		return 0;
	}
	if (!strcmp(argv[1], "--compress")) compress(argv[2], argv[3]);
	else if (!strcmp(argv[1],"--decompress")) decompress(argv[2]);
	else cout << "error" << endl;
	return 0;
}