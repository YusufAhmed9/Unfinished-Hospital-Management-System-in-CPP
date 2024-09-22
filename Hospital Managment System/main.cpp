#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<sstream>

using namespace std;

class Date {
};

class Reservation {
private:
	string reservationID;
	string hospitalID;
	string reservationType;
	Date reservationDay;
};

class User {
private:
	string username;
	string password;

public:
	User():
		username("Username"), password("Password"){
	}

	User(string username, string password) :
		username(username), password(password) {
	}

	void setUsername(const string& username) {
		this->username = username;
	}

	void setPassword(const string& password) {
		this->password = password;
	}

	string getUsername() const {
		return username;
	}

	string getPassword() const {
		return password;
	}

	virtual bool userType() const{ 
		// A function to tell if the
		// The user is a patient '1'
		// or an admin '0'
		return false;
	}
};

class Admin : public User {
private:
public:
	Admin(User user) : User(user) {}

	bool userType() const override {
		return false;
	}
};

class Patient : public User {
private:
	vector<Reservation>reservations;
public:
	Patient(User user) : User(user) {}

	void reserve(Reservation reservation) {
		reservations.push_back(reservation);
	}

	bool userType() const override {
		return true;
	}
};

class Database {
private:
	const int loginChoice = 1;
	const int signUpChoice = 2;
	const int usernameInSheetEnd = 2;
	const int usernameInSheetStart = 1;
	const int passwordInSheetStart = 2;
	const int passwordInSheetEnd = 3;
	const string sheetName = "Data.csv";


	int countRowsInFile(const string& fileName) const {
		int row = 0;
		string temp;
		ifstream in(fileName);
		while (getline(in, temp))
			++row;
		return row;
	}

	bool fileIsEmpty(const string& fileName) const {
		return countRowsInFile(fileName) == 1;
	}

	void loadToFile(const string& fileName, User* user) const {
		// Id format : User type(A admin, P patient) + Row number
		string temp;
		temp.push_back(user->userType() ? 'P' : 'A');
		temp.push_back(countRowsInFile(fileName) + '0');

		ofstream file(fileName, ios::app);
		file << temp << ',';
		file << user->getUsername() << ',';
		file << user->getPassword() << ',';
		file << user->userType() << ',';
		file << '\n';

		file.close();
	}

	bool userInfoChecking(const string& fileName,const string& username) const {
		string temp;
		ifstream file(fileName);

		//Using a string to iterate over the created sheet
		while (getline(file, temp)) {
			//To count commas since it is comma seperated
			int commacnt = 0;
			string usernameCheck;
			
			for (char& s : temp) {
				commacnt += s == ',';

				if (commacnt == usernameInSheetStart and s != ',')
					usernameCheck.push_back(s);

				if (commacnt == usernameInSheetEnd)
					break;
			}

			if (usernameCheck == username)//Username was found the created sheet
				return true;
		}

		return false;
	}
	
	bool userInfoChecking(const string& fileName,const string& username, const string& password) const {
		string temp;
		ifstream file(fileName);

		//Using a string to iterate over the created sheet
		while (getline(file, temp)) {
			//To count commas since it is comma seperated
			int commacnt = 0;
			string usernameCheck;
			string passwordCheck;
			
			for (char& s : temp) {
				commacnt += s == ',';

				if (commacnt == usernameInSheetStart and s != ',')
					usernameCheck.push_back(s);

				if (commacnt == passwordInSheetStart and s != ',')
					passwordCheck.push_back(s);

				if (commacnt == passwordInSheetEnd)
					break;
			}

			if (usernameCheck == username and passwordCheck == password)//Username was found the created sheet
				return true;
		}

		return false;
	}

	void login() {
		cout << "***********************\n\n\Login\n\n***********************\n\n";
		string username;
		string password;
		while (true) {
			cout << "Username: ";
			cin >> username;
			cout << "Password: ";
			cin >> password;
			if (!userInfoChecking(sheetName, username, password)) {
				cout << "Username and Password do not match\n";
				continue;
			}
			break;
		}
	}

	void signUp() {
		cout << "***********************\n\n\tSign Up\n\n***********************\n\n";
		User user;
		string tempUsername;
		string tempPassword;
		string tempConfirmPassword;
	
		while (true) {
			cout << "Username: ";
			cin >> tempUsername;
			
			if (tempUsername.empty() or userInfoChecking(sheetName, tempUsername)) {
				cout << "Please enter another user name\n";
				continue;
			}

			user.setUsername(tempUsername);
			break;
		}

		while (true) {
			cout << "Password: ";
			cin >> tempPassword;

			cout << "Confirm Password: ";
			cin >> tempConfirmPassword;
			
			if (tempPassword != tempConfirmPassword) {
				cout << "Passwords don't match\n";
				continue;
			}

			if (tempPassword.size() < 8) {
				cout << "The password you've enterd is weak\n";
				continue;
			}
			user.setPassword(tempPassword);
			break;
		}

		User* user1;

		//The user is an admin "Because it is the first user"
		if (fileIsEmpty(sheetName))
			user1 = new Admin(user);

		//The user is a patient
		else
			user1 = new Patient(user);

		loadToFile(sheetName, user1);

		delete user1;
	}

public:
	void mainInterface() {
		int type;
		cout << "**********************************************************\n\n";
		cout << "\tWelcome to Hospital Management System(HMS)\n\n";
		cout << "**********************************************************\n\n";
		cout << "1. Login\n"
			<< "2. Sign Up\n\n";
		
		cout << "Enter choice: ";
			cin >> type;
			
			while (type != loginChoice and type != signUpChoice) {
				cout << "Error: please enter a valid option\n";
				cout << "Welcome to HMS\n"
					<< "1. Login\n"
					<< "2. Sign Up\n";
				cin >> type;
			}

			if (type == loginChoice)
				login();
			else
				signUp();
	}
};

int main() {
	Database DB;
	DB.mainInterface();
	return 0;
}