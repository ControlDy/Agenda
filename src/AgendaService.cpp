#include "AgendaService.hpp"
#include "User.hpp"
#include "Meeting.hpp"
#include "Date.hpp"
#include "MyException.hpp"
#include <iostream>
using namespace std;
/**
 * constructor
 */
AgendaService::AgendaService() {startAgenda();}

/**
 * destructor
 */
AgendaService::~AgendaService() {quitAgenda();}

list<User> checkUsername(shared_ptr<Storage> &storage, string userName) {
	return storage->getInstance()->queryUser(
		[userName](const User &t_user)->bool {return t_user.getName() == userName;}
		);
}

/**
 * check if the username match password
 * @param userName the username want to login
 * @param password the password user enter
 * @return if success, true will be returned
 */
bool AgendaService::userLogIn(const std::string userName, const std::string password) {
	list<User> t_ul = checkUsername(m_storage, userName);
	if (t_ul.empty()) return false;
	User the_user = t_ul.front();
	return password == the_user.getPassword();
}

/**
 * regist a user
 * @param userName new user's username
 * @param password new user's password
 * @param email new user's email
 * @param phone new user's phone
 * @return if success, true will be returned
 */
// the userName must be only
bool AgendaService::userRegister(const std::string userName, const std::string password,
                  const std::string email, const std::string phone) {
	list<User> t_ul1 = checkUsername(m_storage, userName);
	if (!t_ul1.empty()) return false;

	User t_user(userName, password, email, phone);
	m_storage->getInstance()->createUser(t_user);
	list<User> t_ul2 = checkUsername(m_storage, userName);
	if (t_ul2.empty()) return false;
	return true;
}

/**
 * delete a user
 * @param userName user's username
 * @param password user's password
 * @return if success, true will be returned
 */
 // 删除 User 时，要把该User相关的所有会议都删除
bool AgendaService::deleteUser(const std::string userName, const std::string password) {
	int num = m_storage->getInstance()->deleteUser(
		[userName, password](const User &t_user)->bool {
			return (t_user.getName() == userName)
			&& (t_user.getPassword() == password);
		}
		);
	if (!num) return false;

	// delete all the records
	deleteAllMeetings(userName);
	m_storage->getInstance()->deleteMeeting( [userName](const Meeting &t_meeting)->bool {
		auto t_participator = t_meeting.getParticipator();
		for (auto it = t_participator.begin(); it != t_participator.end(); it++)
			if (*it == userName) return true;
		return false;		
	} );

	return true;
}

/**
 * list all users from storage
 * @return a user list result
 */
std::list<User> AgendaService::listAllUsers(void) const {
	return m_storage->getInstance()->queryUser( [](const User &t_user)->bool{return true;} );
}

bool isInList(string x, list<User> list) {
	for (auto it = list.begin(); it != list.end(); it++)
		if (x == it->getName()) return true;
	return false;
}

/**
 * create a meeting
 * @param userName the sponsor's userName
 * @param title the meeting's title
 * @param participator the meeting's participator
 * @param startData the meeting's start date
 * @param endData the meeting's end date
 * @return if success, true will be returned
 */
 // the title must be unique
 // the dates must be valid
 // enddate must not equal to or less than startdate
 // 会议的发起者和参与者都必须在 User 中
 // userName should not appear in participators
 // each participator should be unique
 // 在判断会议是否重叠时,允许会议开始时间是另一个会议的结束时间，即一个会议结束后马上开始另一个会议。
bool AgendaService::createMeeting(const std::string userName, const std::string title,
                   const std::string startDate, const std::string endDate,
                   const std::vector<std::string> participator) {
	auto user_list = listAllUsers();
	for (auto it = participator.begin(); it != participator.end(); it++) {
		for (auto it2 = participator.begin(); it2 != it; it2++)
			if (*it2 == *it)
				throw MyException("Each participator should be unique!");

		if (!isInList(*it, user_list))
			throw MyException("Each participator should also be a user!");

		if (*it == userName) 
			throw MyException("One of the participators is the sponsor!");
	}

	auto title_list = m_storage->getInstance()->queryMeeting( [title](const Meeting &t_meeting)->bool {
		if (title == t_meeting.getTitle()) return true;
		return false;	
	} );
	if (title_list.size()) throw MyException("The title has existed!");

	if ( !Date::isValid( Date::stringToDate(startDate) )
		|| !Date::isValid( Date::stringToDate(endDate) ) )
		throw MyException("Invalid time!");

	if (startDate >= endDate)
		throw MyException("The start time should be earlier than the latter!");

	auto date_list = meetingQuery(userName, startDate, endDate);
	for (auto it = date_list.begin(); it != date_list.end(); it++) {
		Date sd(startDate), ed(endDate);
		Date t_sd = it->getStartDate(), t_ed = it->getEndDate();
		if ( !(sd >= t_ed || ed <= t_sd) )
			throw MyException("The time interval of the meeting overlaps the others!");
	}

	if (!isInList(userName, user_list)) return false;
	
	Meeting t_meeting(userName, participator, startDate, endDate, title);
	m_storage->getInstance()->createMeeting(t_meeting);
	return true;
}

/**
 * search meetings by username and title (user as sponsor or participator)
 * @param uesrName the user's userName
 * @param title the meeting's title
 * @return a meeting list result
 */
std::list<Meeting> AgendaService::meetingQuery(const std::string userName,
                                const std::string title) const {
	return m_storage->getInstance()->queryMeeting( [userName, title](const Meeting &t_meeting)->bool {
		bool flag = false;
		auto t_participator = t_meeting.getParticipator();
		for (auto it = t_participator.begin(); it != t_participator.end(); it++)
			if (*it == userName) {
				flag = true;
				break;
			}
		return (userName == t_meeting.getSponsor() || flag)
		&& title == t_meeting.getTitle();		
	} );
}

/**
 * search a meeting by username, time interval (user as sponsor or participator)
 * @param uesrName the user's userName
 * @param startDate time interval's start date
 * @param endDate time interval's end date
 * @return a meeting list result
 */
 // 若查询过程中输入的日期不合法，直接返回空列表
 // 关于找出一个时间段内的会议，只要有时间点重叠，都要算上去,
 // 如查询区间为2016-07-08/12:00至2016-07-08/12:00时，时间为2016-07-08/11:00至2016-07-08/12:00的会议可以被查询到。
std::list<Meeting> AgendaService::meetingQuery(const std::string userName,
                                const std::string startDate,
                                const std::string endDate) const {
	if ( !Date::isValid( Date::stringToDate(startDate) )
		|| !Date::isValid( Date::stringToDate(endDate) ) )
		return list<Meeting>();

	return m_storage->getInstance()->queryMeeting( [this, userName, startDate, endDate](const Meeting &t_meeting)->bool {
		bool flag = false;
		auto t_participator = t_meeting.getParticipator();
		for (auto it = t_participator.begin(); it != t_participator.end(); it++)
				if (*it == userName) {
							flag = true; // the user is one of the participators
							break;
				}
		bool flag2 = false;
		Date sd(startDate), ed(endDate);
		Date t_sd = t_meeting.getStartDate(), t_ed = t_meeting.getEndDate();
		if ( !(sd > t_ed || ed < t_sd) ) return flag2 = true;

		return (userName == t_meeting.getSponsor() || flag) && flag2;
	} );
}

/**
 * list all meetings the user take part in (or sponse)
 * @param userName user's username
 * @return a meeting list result
 */
std::list<Meeting> AgendaService::listAllMeetings(const std::string userName) const {
	return m_storage->getInstance()->queryMeeting( [userName](const Meeting &t_meeting)->bool {
		bool flag = false;
		auto t_participator = t_meeting.getParticipator();
		for (auto it = t_participator.begin(); it != t_participator.end(); it++)
			if (*it == userName) {
				flag = true;
				break;
			}
		return (userName == t_meeting.getSponsor() || flag);		
	} );
}

/**
 * list all meetings the user sponsor
 * @param userName user's username
 * @return a meeting list result
 */
std::list<Meeting> AgendaService::listAllSponsorMeetings(const std::string userName) const {
	return m_storage->getInstance()->queryMeeting( [userName](const Meeting &t_meeting)->bool {
		return userName == t_meeting.getSponsor();		
	} );
}

/**
 * list all meetings the user take part in and sponsor by other
 * @param userName user's username
 * @return a meeting list result
 */
std::list<Meeting> AgendaService::listAllParticipateMeetings(
    const std::string userName) const {
	return m_storage->getInstance()->queryMeeting( [userName](const Meeting &t_meeting)->bool {
		auto t_participator = t_meeting.getParticipator();
		for (auto it = t_participator.begin(); it != t_participator.end(); it++)
			if (*it == userName) return true;
		return false;		
	} );
}

/**
 * delete a meeting by title and its sponsor
 * @param userName sponsor's username
 * @param title meeting's title
 * @return if success, true will be returned
 */
bool AgendaService::deleteMeeting(const std::string userName, const std::string title) {
	int num = m_storage->getInstance()->deleteMeeting(
		[userName, title](const Meeting &t_meeting)->bool {
			return (userName == t_meeting.getSponsor()) && (title == t_meeting.getTitle());
		} );
	if (!num) return false;
	return true;
}

/**
 * delete all meetings by sponsor
 * @param userName sponsor's username
 * @return if success, true will be returned
 */
bool AgendaService::deleteAllMeetings(const std::string userName) {
	int num = m_storage->getInstance()->deleteMeeting(
		[userName](const Meeting &t_meeting)->bool {
			return userName == t_meeting.getSponsor();
		} );
	if (!num) return false;
	return true;
}

/**
 * start Agenda service and connect to storage
 */
void AgendaService::startAgenda(void) {
	m_storage->getInstance();
}

/**
 * quit Agenda service
 */
void AgendaService::quitAgenda(void) {
	m_storage->getInstance()->sync();
}