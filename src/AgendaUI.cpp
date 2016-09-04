#include "AgendaUI.hpp"
#include <iomanip>
#include <cstdlib>
#include "SignalHandler.hpp"
#include "MyException.hpp"
using namespace std;
#define SCREEN_WIDTH 100

int status;
// 1: log in
// 0: log out

void InvalideInput() {
	cout << "\n[error] Invalid input! Please try again!\n";
}

void Start(string userName) {
	printAgenda(); 
	cout << "\nPress \"[Ctrl] + Z\" for helps\n\n";
	printLine(SCREEN_WIDTH);  
	if (status) cout << "\n\nAgenda@" << userName << " : # ";
	else cout << "\n\nAgenda : ~$ "; 
} 

AgendaUI::AgendaUI() { 
	m_userName = m_userPassword = "";
}

void AgendaUI::OperationLoop(void) {
	SignalHandler m_handler;
	m_handler.Capture();
	m_handler.GetHelp();
	startAgenda();
	Start(m_userName);
	while (1) {
		if ( !executeOperation(getOperation()) ) InvalideInput();
		Start(m_userName);
	}
}

/**
 * constructor
 */
void AgendaUI::startAgenda(void) {
	status = 0;
}

/**
 * catch user's operation
 * @return the operation
 */
std::string AgendaUI::getOperation() {
	string operation;
	cin >> operation;
	return operation;
}

/**
 * execute the operation
 * @return if the operationloop continue
 */
bool AgendaUI::executeOperation(std::string t_operation) {
	if (!status) {
		if (t_operation == "l") userLogIn();
		else if (t_operation ==  "r") userRegister();
		else if (t_operation == "q") quitAgenda();
		else return false;
	} else {
		if (t_operation == "o") userLogOut();
		else if (t_operation == "dc") deleteUser();
		else if (t_operation == "lu") listAllUsers();
		else if (t_operation == "cm") createMeeting();
		else if (t_operation == "la") listAllMeetings();
		else if (t_operation == "las") listAllSponsorMeetings();
		else if (t_operation == "lap") listAllParticipateMeetings();
		else if (t_operation == "qm") queryMeetingByTitle();
		else if (t_operation == "qt") queryMeetingByTimeInterval();
		else if (t_operation == "dm") deleteMeetingByTitle();
		else if (t_operation == "da") deleteAllMeetings();
		else return false;
	}
	return true;
}

/**
 * user Login
 */
void AgendaUI::userLogIn(void) {
	cout << "\n[log in] [user name] [password]\n[log in] ";
	string userName, userPassword;
	cin >> userName >> userPassword;
	if (m_agendaService.userLogIn(userName, userPassword)) {
		cout << "[log in] succeed!\n";
		m_userName = userName;
		m_userPassword = userPassword;
		status = 1;
	} else	cout << "[error] log in fail!\n";
}

/**
 * user regist
 */
void AgendaUI::userRegister(void) {
	cout << "\n[register] [user name] [password] [email] [phone]\n[register] ";
	string userName, userPassword, email, phone;
	cin >> userName >> userPassword >> email >> phone;
	if (m_agendaService.userRegister(userName, userPassword, email, phone))
		cout << "[register] succeed!\n";
	else cout << "[error] register fail!\n";
}

/**
 * quit the Agenda
 */
void AgendaUI::quitAgenda(void) {exit(0);}

/**
 * user logout
 */
void AgendaUI::userLogOut(void) {
	status = 0;
}

/**
 * delete a user from storage
 */
void AgendaUI::deleteUser(void) {
	if (m_agendaService.deleteUser(m_userName, m_userPassword)) {
		cout << "\n[delete agenda account] succeed!\n";
		status = 0;
	}
}

/**
 * list all users from storage
 */
void AgendaUI::listAllUsers(void) {
	cout << "\n[list all users]\n";
	cout << setw(10) << setiosflags(ios::left) << "\nname"
	<< setw(15) << "email" << "phone\n";
	list<User> allUsers = m_agendaService.listAllUsers();
	for (auto it = allUsers.begin(); it != allUsers.end(); it++) {
		cout << setw(10) << setiosflags(ios::left) << it->getName()
		<< setw(15) << it->getEmail() << it->getPhone() << endl;
	}
}

/**
 * user create a meeting with someone else
 */
void AgendaUI::createMeeting(void) {
	cout << "\n[create meeting] " << "[the number of participators]\n"
	<< "[create meeting] ";
	int num;
	cin >> num;
	vector<string> participators;
	for (int i = 0; i < num; i++) {
		cout << "[create meeting] " << "[please enter the participator "
		<< i+1 << " ]\n" << "[create meeting] ";
		string participator;
		cin >> participator;
		participators.push_back(participator);
	}
	cout << "[create meeting] " << "[title] [start time(yyyy-mm-dd/hh:mm)] "
	<< "[end time(yyyy-mm-dd/hh:mm)]\n" << "[create meeting] ";
	string title, startTime, endTime;
	cin >> title >> startTime >> endTime;
	cout << "[create meeting] ";
	try {
		if (m_agendaService.createMeeting(m_userName, title, startTime, endTime, participators))
			cout << "succeed!\n";
		else cout << "error!\n";
	}
	catch (MyException e) {
		cout << "[error] " << e.what() << endl;
		cout << "\nPlease try again!\n";
		createMeeting();
	}
}

/**
 * list all meetings from storage
 */
void AgendaUI::listAllMeetings(void) {
	cout << "\n[list all meetings]\n";
	list<Meeting> allMeeting = m_agendaService.listAllMeetings(m_userName);
	printMeetings(allMeeting);
}

/**
 * list all meetings that this user sponsored
 */
void AgendaUI::listAllSponsorMeetings(void) {
	cout << "\n[list all sponsor meetings]\n";
	list<Meeting> allSponsorMeeting = m_agendaService.listAllSponsorMeetings(m_userName);
	printMeetings(allSponsorMeeting);
}

/**
 * list all meetings that this user take part in
 */
void AgendaUI::listAllParticipateMeetings(void) {
	cout << "\n[list all participator meetings]\n";
	list<Meeting> allParticipateMeeting = m_agendaService.listAllParticipateMeetings(m_userName);
	printMeetings(allParticipateMeeting);
}

/**
 * search meetings by title from storage
 */
void AgendaUI::queryMeetingByTitle(void) {
	cout << "\n[query meetings] " << "[title]:\n"
	<< "[query meetings] ";
	string title;
	cin >> title;
	list<Meeting> byTitle = m_agendaService.meetingQuery(m_userName, title);
	printMeetings(byTitle);
}

/**
 * search meetings by timeinterval from storage
 */
void AgendaUI::queryMeetingByTimeInterval(void) {
	cout << "\n[query meetings] " << "[start time(yyyy-mm-dd/hh:mm)] "
	<< "[end time(yyyy-mm-dd/hh:mm)]\n"
	<< "[query meetings] ";
	string startTime, endTime;
	cin >> startTime >> endTime;
	list<Meeting> byTime = m_agendaService.meetingQuery(m_userName, startTime, endTime);
	cout << "[query meeings]\n";
	printMeetings(byTime);
}

/**
 * delete meetings by title from storage
 */
void AgendaUI::deleteMeetingByTitle(void) {
	cout << "[delete meeting] " << "[title]\n"
	<< "[delete meeting] ";
	string title;
	cin >> title;
	if (m_agendaService.deleteMeeting(m_userName, title))
		cout << "[delete meeting by title] succeed!\n";
	else cout << "[error] delete meeting fail!\n";
}

/**
 * delete all meetings that this user sponsored
 */
void AgendaUI::deleteAllMeetings(void) {
	if (m_agendaService.deleteAllMeetings(m_userName))
		cout << "[delete all meetings] succeed!\n";
	else cout << "[error] delete all meetings fail!\n";
}

/**
 * show the meetings in the screen
 */
void AgendaUI::printMeetings(std::list<Meeting> t_meetings) {
	if (t_meetings.empty()) {
		cout << "\n[error] There is no such record!\n";
		return;
	}

	cout << endl;
	cout << setiosflags(ios::left) << setw(SCREEN_WIDTH/5) << "title"
	<< setw(SCREEN_WIDTH/5) << "sponsor" 
	<< setw(SCREEN_WIDTH/5) << "start time" 
	<< setw(SCREEN_WIDTH/5) << "end time" 
	<< setw(SCREEN_WIDTH/5) << "participators" << endl;

	for (auto it = t_meetings.begin(); it != t_meetings.end(); it++) {
		cout << setw(SCREEN_WIDTH/5) << it->getTitle()
		<< setw(SCREEN_WIDTH/5) << it->getSponsor()
		<< setw(SCREEN_WIDTH/5) << Date::dateToString(it->getStartDate())
		<< setw(SCREEN_WIDTH/5) << Date::dateToString(it->getEndDate());
		auto participator_list = it->getParticipator();
		string participators = "";
		for (auto it2 = participator_list.begin(); it2 != participator_list.end(); it2++) {
			participators += *it2;
			auto it3 = it2;
			if (++it3 == participator_list.end()) break;
			participators += ',';
		}
		cout << setw(SCREEN_WIDTH/5) << participators << endl;
	}
}
