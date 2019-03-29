#include "Duration.h"



Duration::Duration()
{
	hours = 0;
	minutes = 0;
	seconds = 0;
}

/*
	Method to convert the number of seconds elapsed into a duration
 */
void Duration::convertFromSeconds(int secs) {

	seconds = secs % 60;

	secs -= seconds;

	secs /= 60;

	minutes = secs % 60;

	secs -= minutes;

	secs /= 60;

	hours = secs;
}

ostream& operator<<(ostream& os, const Duration& d)
{
	string secString;
	string minString;
	string hourString;

	if (d.seconds < 10) {
		secString = "0" + std::to_string(d.seconds);
	}
	else {
		secString = std::to_string(d.seconds);
	}

	if (d.minutes < 10) {
		minString = "0" + std::to_string(d.minutes);
	}
	else {
		minString = std::to_string(d.minutes);
	}

	if (d.hours < 10) {
		hourString = "0" + std::to_string(d.hours);
	}
	else {
		hourString = std::to_string(d.hours);
	}

	os << hourString << ':' << minString << ':' << secString;
	return os;
}

Duration::~Duration()
{
}
