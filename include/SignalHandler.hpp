#ifndef SIG_HDL
#define SIG_HDL

#include <csignal>
#include <iostream>
using namespace std;

extern int status;

#define SCREEN_WIDTH 100

void printLine(int n) {
	for (int i = 0; i < n; i++) cout << '-';
}

void printAgenda() {
	cout << endl;
	printLine( (SCREEN_WIDTH-6)/2 );
	cout << "Agenda";
	printLine( (SCREEN_WIDTH-6)/2 );
	cout << endl;
}

void showTips() {
	if (!status) {
		// log out
		printAgenda();
		cout << "Action :\nl\t- log in Agenda by user name and password\n"
		<< "r\t- register an Agenda account\n"
		<< "q\t- quit Agenda\n";
		printLine(SCREEN_WIDTH);
		cout << endl;
	} else {
		// log in
		printAgenda();
		cout << "Action :\n"
		<< "o\t- log out Agenda\ndc\t- delete Agenda account\n"
		<< "lu\t- list all Agenda user\ncm\t- create a meeting\n"
		<< "la\t- list all meetings\nlas\t- list all sponsor meetings\n"
		<< "lap\t- list all participate meetings\nqm\t- query meeting by title\n"
		<< "qt\t- query meeting by time interval\ndm\t- delete meeting by title\n"
		<< "da\t- delete all meetings\n";
		printLine(SCREEN_WIDTH);
		cout << endl;
	}
}

void handler(int sig) {
	cout << "\n\n[tips] You have given the order to terminate the programme.\n"
	<< "[tips] But your records will still be saved.\n\n";
	exit(0);
}

void handler2(int sig) {
	showTips();
}

class SignalHandler {
 public:
 	void Capture() {
		signal(SIGINT, handler);
	}
	void GetHelp() {
		signal(SIGTSTP, handler2);
	}
};

#endif

