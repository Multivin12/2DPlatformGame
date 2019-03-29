#pragma once
#include <iostream>
#include <string>		

using namespace std;

class Duration
{
public:

	int hours;
	int minutes;
	int seconds;


	Duration();
	void convertFromSeconds(int secs);
	friend ostream& operator<<(ostream& os, const Duration& dt);
	~Duration();
};

