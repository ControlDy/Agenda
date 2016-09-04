#include "Date.hpp"
#include <cmath>
#include <sstream>
#include <cctype>
#include <vector>
using namespace std;

Date::Date() {
	m_year = m_month = m_minute = m_hour = m_day = 0;
}

Date::Date(int t_year, int t_month, int t_day, int t_hour, int t_minute):
	m_year(t_year), m_month(t_month), m_day(t_day),
	m_hour(t_hour), m_minute(t_minute) {}

Date::Date(string dateString) {
	Date t_Date = stringToDate(dateString);
	m_year = t_Date.getYear();
	m_month = t_Date.getMonth();
	m_minute = t_Date.getMinute();
	m_hour = t_Date.getHour();
	m_day = t_Date.getDay();
}

int Date::getYear(void) const {return m_year;}
void Date::setYear(const int t_year) {m_year = t_year;}

int Date::getMonth(void) const {return m_month;}
void Date::setMonth(const int t_month) {m_month = t_month;}

int Date::getDay(void) const {return m_day;}
void Date::setDay(const int t_day) {m_day = t_day;}

int Date::getHour(void) const {return m_hour;}
void Date::setHour(const int t_hour) {m_hour = t_hour;}

int Date::getMinute(void) const {return m_minute;}
void Date::setMinute(const int t_minute) {m_minute = t_minute;}

int MaxDays(int year, int month) {
	if (month == 2) {
		if ((year%4 == 0 && year%100 != 0) || year%400 == 0)
			return 29;
		else return 28;
	} else {
		if (month == 4 || month == 6 || month == 9 || month == 11)
			return 30;
		else return 31;
	}
}

bool Date::isValid(const Date t_date) {
	if (t_date.getYear() < 1000 || t_date.getYear() > 9999) return false;
	if (t_date.getMinute() > 59 || t_date.getMinute() < 0) return false;
	if (t_date.getHour() > 23 || t_date.getHour() < 0) return false;
	if (t_date.getMonth() > 12 || t_date.getMonth() <= 0) return false;
	if (t_date.getDay() > MaxDays(t_date.getYear(), t_date.getMonth())
		|| t_date.getDay() <= 0) return false;
	return true;
}

int ToInt(const char *s, int n) {
	int ret = 0;
	for (int i = 0; i < n; i++) {
		ret = (s[n-i-1]-'0')*pow(10, i) + ret;
	}
	return ret;
}

/**
* @brief convert a string to date, if the format is not correct return
* 0000-00-00/00:00
* @return a date
*/
Date Date::stringToDate(const string t_dateString) {
	if (t_dateString.length() != 16 || t_dateString[4] != '-'
		|| t_dateString[7] != '-' || t_dateString[10] != '/'
		|| t_dateString[13] != ':') return Date(0, 0, 0, 0, 0);
	for (int i = 0; i < t_dateString.length(); i++) {
		if (i == 4 || i == 7 || i == 10 || i == 13) continue;
		if (!isdigit(t_dateString[i])) return Date(0, 0, 0, 0, 0);
	}
	const char *str = t_dateString.c_str();
	return Date( ToInt(str, 4), ToInt(str+5, 2), ToInt(str+8, 2),
		ToInt(str+11, 2), ToInt(str+14, 2) );
}

// 对于不满 10 的数字，应该补 0
string ToString(int x) {
	stringstream ss;
	ss << x;
	if (x < 10) return "0" + ss.str();
	return ss.str();
}
/**
* @brief convert a date to string, if the format is not correct return
* 0000-00-00/00:00
*/
string Date::dateToString(Date t_date) {
	if (!isValid(t_date)) return "0000-00-00/00:00";
	return ToString(t_date.getYear()) + "-" + ToString(t_date.getMonth()) +
	"-" + ToString(t_date.getDay()) + "/" + ToString(t_date.getHour()) +
	":" + ToString(t_date.getMinute());
}

Date & Date::operator=(const Date &t_date) {
	m_year = t_date.getYear();
	m_month = t_date.getMonth();
	m_minute = t_date.getMinute();
	m_hour = t_date.getHour();
	m_day = t_date.getDay();
	return *this;
}

vector<int> toVector(const Date &t_date) {
	std::vector<int> ret;
	ret.push_back(t_date.getYear());
	ret.push_back(t_date.getMonth());
	ret.push_back(t_date.getDay());
	ret.push_back(t_date.getHour());
	ret.push_back(t_date.getMinute());
	return ret;
}

bool Date::operator==(const Date &t_date) const {
	return toVector(*this) == toVector(t_date);
}

bool Date::operator>(const Date &t_date) const {
	return toVector(*this) > toVector(t_date);
}

bool Date::operator<(const Date &t_date) const {
	return toVector(*this) < toVector(t_date);
}

bool Date::operator>=(const Date &t_date) const {
	return toVector(*this) >= toVector(t_date);
}

bool Date::operator<=(const Date &t_date) const {
	return toVector(*this) <= toVector(t_date);
}
