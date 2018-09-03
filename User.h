#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <time.h>
#include <Windows.h>
#include<iomanip>
#include <algorithm>

using namespace std;


string getDate();
void gotoXY(int x, int y);

class User {

	string m_name;
	string m_date;
	int m_score;
	int m_rank;

public:
	//constructors
	User() {}
	User(string name, int score) : m_name(name), m_score(score) {
		m_date = getDate();
		m_rank = 0;
	}
	User(int rank, string name, int score, string date) : m_rank(rank), m_name(name), m_score(score), m_date(date) {}

	//getters and setters
	void setRank(int rank) { m_rank = rank; }
	int getScore() { return (m_score); }

	//unique methods

	void displayInfo(int x, int y) {
		gotoXY(x, y);
		cout << setw(7) << m_rank;
		cout << setw(22) << m_name;
		cout << setw(8) << m_score;
		cout << setw(10) << m_date;
	}

	void saveInfo(ofstream * file) {
		*file << "\n" << m_rank << " " << m_name << " " << m_score << " " << m_date;
	}

};