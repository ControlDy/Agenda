#include "Storage.hpp"
#include "Path.hpp"
#include <fstream>
#include <iostream>
using namespace std;

Storage::Storage(): m_dirty(false) {
	readFromFile();
}

/**
*   read file content into memory
*   @return if success, true will be returned
*/
bool Storage::readFromFile(void) {
	ifstream user_file(Path::userPath), meeting_file(Path::meetingPath);
	if (user_file.is_open() && meeting_file.is_open()) {

		m_userList.clear();
		m_meetingList.clear();

		while (!user_file.eof()) {
			string Name, Password, Email, Phone;
			getline(user_file, Name, ',');
			if (Name == "") break;
			getline(user_file, Password, ',');
			getline(user_file, Email, ',');
			getline(user_file, Phone, '\n');
			User t_user( Name.substr(1, Name.length()-2),
				Password.substr(1, Password.length()-2),
				Email.substr(1, Email.length()-2),
				Phone.substr(1, Phone.length()-2) );
			m_userList.push_back(t_user);
		}
		user_file.close();

		while (!meeting_file.eof()) {
			string Sponsor, Participators, StartDate, EndDate, Title;
			getline(meeting_file, Sponsor, ',');
			if (Sponsor == "") break;
			getline(meeting_file, Participators, ',');
			getline(meeting_file, StartDate, ',');
			getline(meeting_file, EndDate, ',');
			getline(meeting_file, Title, '\n');

			vector<string> t_participators;
			int i = 1;
			while (1) {
				int start = i, end;
				while (Participators[i] != '&' && Participators[i] != '\"') i++;
				end = i;
				t_participators.push_back( Participators.substr(start, end-start) );
				if (Participators[i] == '\"') break;
				else i++;
			}

			Meeting t_meeting( Sponsor.substr(1, Sponsor.length()-2),
				t_participators,
				Date( StartDate.substr(1, StartDate.length()-2) ),
				Date( EndDate.substr(1, EndDate.length()-2) ),
				Title.substr(1, Title.length()-2) );
			m_meetingList.push_back(t_meeting);
		}
		meeting_file.close();

		return true;

	} else return false;
}

/**
*   write file content from memory
*   @return if success, true will be returned
*/
bool Storage::writeToFile(void) {
	ofstream user_file(Path::userPath), meeting_file(Path::meetingPath);
	if (user_file.is_open() && meeting_file.is_open()) {

		for (auto it = m_userList.begin();it != m_userList.end(); it++) {
			user_file << "\"" << it->getName() << "\",\""
			<< it->getPassword() << "\",\"" << it->getEmail()
			<< "\",\"" << it->getPhone() << "\"\n";
		}
		user_file.close();

		for (auto it = m_meetingList.begin();it != m_meetingList.end(); it++) {
			meeting_file << "\"" << it->getSponsor() << "\",\"";

			vector<string> t_vector(it->getParticipator());
			for (auto it2 = t_vector.begin(); it2 != t_vector.end(); it2++) {
				meeting_file << *it2;
				auto it3 = it2;
				if (++it3 != t_vector.end()) meeting_file << "&";
			}

			meeting_file << "\",\"" << Date::dateToString( it->getStartDate() )
			<< "\",\"" << Date::dateToString( it->getEndDate() )
			<< "\",\"" << it->getTitle() << "\"\n";
		}
		meeting_file.close();

		return true;

	} else return false;
}

shared_ptr<Storage> Storage::m_instance;

shared_ptr<Storage> Storage::getInstance(void) {
	if (!m_instance.get()) {
		Storage *t_ptr = new Storage();
		m_instance.reset(t_ptr);
	}
	return m_instance;
}

Storage::~Storage() {sync();}

// CRUD for User & Meeting
// using C++11 Function Template and Lambda Expressions

/**
* create a user
* @param a user object
*/
void Storage::createUser(const User & t_user) {
	int pre_num = m_userList.size();
	m_userList.push_back(t_user);
	if (pre_num != m_userList.size()) m_dirty = true;
}

/**
* query users
* @param a lambda function as the filter
* @return a list of fitted users
*/
list<User> Storage::queryUser(function<bool(const User &)> filter) const {
	list<User> ret;
	for (auto it = m_userList.begin(); it != m_userList.end(); it++)
		if (filter(*it)) ret.push_back(*it);
	return ret;
}

/**
* update users
* @param a lambda function as the filter
* @param a lambda function as the method to update the user
* @return the number of updated users
*/
int Storage::updateUser(function<bool(const User &)> filter,
	function<void(User &)> switcher) {
	int count = 0;
	for (auto it = m_userList.begin(); it != m_userList.end(); it++)
		if (filter(*it)) {
			switcher(*it);
			count++;
		}
	if (count) m_dirty = true;
 	return count;
}

/**
* delete users
* @param a lambda function as the filter
* @return the number of deleted users
*/
int Storage::deleteUser(function<bool(const User &)> filter) {
	int count = 0;
	for (auto it = m_userList.begin(); it != m_userList.end();)
		if (filter(*it)) {
			it = m_userList.erase(it);
			count++;
		} else it++;
	if (count) m_dirty = true;
	return count;
}

/**
* create a meeting
* @param a meeting object
*/
void Storage::createMeeting(const Meeting & t_meeting) {
	int pre_num = m_meetingList.size();
	m_meetingList.push_back(t_meeting);
	if (pre_num != m_meetingList.size()) m_dirty = true;
}

/**
* query meetings
* @param a lambda function as the filter
* @return a list of fitted meetings
*/
list<Meeting> Storage::queryMeeting(function<bool(const Meeting &)> filter) const {
	list<Meeting> ret;
	for (auto it = m_meetingList.begin(); it != m_meetingList.end(); it++)
		if (filter(*it)) ret.push_back(*it);
	return ret;		
}

/**
* update meetings
* @param a lambda function as the filter
* @param a lambda function as the method to update the meeting
* @return the number of updated meetings
*/
int Storage::updateMeeting(function<bool(const Meeting &)> filter,
    function<void(Meeting &)> switcher) {
	int count = 0;
	for (auto it = m_meetingList.begin(); it != m_meetingList.end(); it++)
		if (filter(*it)) {
			switcher(*it);
			count++;
		}
	if (count) m_dirty = true;
	return count;
}

/**
* delete meetings
* @param a lambda function as the filter
* @return the number of deleted meetings
*/
int Storage::deleteMeeting(function<bool(const Meeting &)> filter) {
	int count = 0;
	for (auto it = m_meetingList.begin(); it != m_meetingList.end();)
		if (filter(*it)) {
			it = m_meetingList.erase(it);
			count++;
		} else it++;
	if (count) m_dirty = true;
	return count;
}

/**
* sync with the file
*/
bool Storage::sync(void) {
	if (m_dirty == false) return true;
	return writeToFile();
}