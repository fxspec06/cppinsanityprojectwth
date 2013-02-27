/*
	CHAPTER: 8
	PROGRAM: Class Listing Report
	AUTHOR: Bryan Leasot
	DATE: 2.14.12
	DESCRIPTION: A Class Listing Report is to be prepared.
		Write the C++ program to prepare this report.
		Pseudocode Specification must be used when designing the program.
		Use Test Data CIS225HW1DA.TXT

	REQUIRED CONCEPTS: Parallel Arrays.
		CONTROL BREAKS, GETLINE, WHILE(inFile), .LENGTH, .FIND, .SUBSTR, OUTFILE <<.
		User defined functions And any other C++ statement to produce the required results.

	This program is an enhancement of Program #1.
		1. Read the input file creating 8 arrays [Semester, Course, Section, Instructor, DOW, Time, Room and Building].
		2. Use these arrays in parallel to generate the same output as program 1. 3. Provide the user a prompt to select a range of semesters to be printed. [Start – Stop] if the start-stop semester are the same then print only that semester. [Remember that the order in which the data is loaded may effect your output]
		3. Provide the user a prompt to select a range of semesters to be printed. [Start – Stop] if the start-stop semester are the same then print only that semester. [Remember that the order in which the data is loaded may effect your output]

*/


/*
	loopReturnVal = 1
	WHILE loopReturnVal != -1 && loopReturnVal != 0
		loopReturnVal = PrintReport()
			Open Input File
			IF Input File Not Found THEN
				MSG “Unable to locate File”
				Return with error code -1
			ENDIF
			Open Output File
			Call CreateArrays
			set loopIndex = 0
			While NOT InputFile.EOF (End Of File)
				Read Line
				Call ExtractFields
			WEND
			RangeOfSemesters = AskUserForRangeOfSemesters
			ask beginning range
			ask ending range
			set numLoops
			DO loop array length
				convert to char array
				compare boundaries
					if the current semester string is within the boundaries of the user's defined beginningRange and endingRange
						then continue with the current loop and print whatever detail and course lines are necessary, just like before
						keep track of how many of each
							IF First_Record THEN
								Current_Instructor = Instructor
								Current_Semester = SemesterCode
								(note that First_Record must be set to False after printing the detail line… You need to keep track of the first row in each semester and first course detail line for control break suppression)
							ENDIF
							IF current_Semester <> Semester THEN
								Call SemesterFooter
							ENDIF
							IF Current_Instructor <> Instructor THEN
								Call InstructorFooter
							ENDIF
							Call DetailLine
						Call SemesterFooter
						Call InstructorFooter
				FEND
			WHILE !RunAgain
			RETURN LoopControlCode 0
		INPUT RunAgain
		IF RunAgain != true
			EXIT PROGRAM
	WEND
	DISPLAY "Nothing to do"
	EXIT
*/

#include "stdafx.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <regex>
#include <iomanip>

using namespace std;

const string INPUT_FILE_NAME = "CIS225HW1DA.txt";
const string OUTPUT_FILE_NAME = "CH9_Output.txt";
const int LENGTH_OF_FILE = 208;
const int PAGE_SIZE = 14;
bool PAGES = false;

// 1 for file, 2 for screen, 0 for both
int outputType = 1;

string SEMESTER_CODES[2][5] = {
	{"10", "20", "30", "40", "50"},
	{"Summer I", "Summer II", "Fall", "Intersession", "Spring"}
};
string BUILDING_CODES[2][5] = {
	{"A ", "AL", "AC", "CS", "S "},
	{"Administration Building", "Ahlfors Hall", "Athletic Center", "Child Study Center", "Surprenant Hall"}
};


struct courseType {
	string ID;
	string section;
};
struct meetingTimeType {
	string dow;
	string time;
};
struct locationType {
	string building,
		buildingName,
		room;
};

struct classType {
	string instructor;
	string instructordate;
	string semesterName;
	
	int semesterCode;
	
	courseType course; //ID, section
	meetingTimeType meeting; // dow, time 
	locationType location; // building, room
};


/*
	need to define input / output files
*/

void extractFields(string inputLine, classType classes[], int index);
void reportHeader(ofstream &outFile, int currentPage);
void courseFooter(string currentCourse, string currentSemester, int courseSections, ofstream &outFile);
void semesterFooter(string currentSemester, int semesterSections, ofstream &outFile);
string instructorFooter(string currentInstructor, int instructorSections, ofstream &outFile);
void detailLine(string inputLine, ofstream &outFile);
void detailLine(string, ofstream &outFile, bool);

// addons
void cut(string &tocut, int cutlength);
void lengthen (string &tolengthen, int length);
void lengthen (string &tolengthen, int length, bool before);
string getSpecialLine(string type);
void reportFooter(ofstream &outFile);
string findInTable(string tofind, string tableName);
void copyTo(string destination[], string source[], int length);
int coutPossibleSemesters(classType[]);
string coutPossibleInstructors(classType classes[]);
//void sortArray(int arrayToSort[]);
void sortByInstructor(classType arrayToSort[]);
void selectAndSwap(classType a[], int els);
//void selectAndSwap(string a[], int els);
void asterisks();
string getFilenameExtention(string filename);

// so i don't have to do this EVERY TIME..
string to_string(int i) {
	return to_string(static_cast<long double>(i));
}
int to_int(string s);
void replace (string &str, string find, string replace) {
	while (str.find(find) >= 0) {
		int i = str.find(find);
		int l = find.length();

		string beginning = str.substr(0, i);
		string ending = str.substr(i+l);
		str = beginning + replace + ending;
	}
}

// extention to main()
int loop();

int main() {
	
	int returnValue = 1;

	while ( returnValue > 0 ) {

		char outputPref = ' ';
		while (outputPref != 'a' && outputPref != 'b' && outputPref != 'c') {
			cout << "QCC Report Generator";
			asterisks();
			cout << "How should the report outputs be presented?\n\ta| file\n\tb| screen\n\tc| both\ninput: ";
			cin >> outputPref;
			cout << endl;
		}
		switch (outputPref) {
		case 'a':
			outputType = 1;
			cout << "Output to file confirmed...";
			break;
		case 'b':
			cout << "Output to screen confirmed...";
			outputType = 2;
			break;
		case 'c':
			cout << "Output to both file and screen confirmed...";
			outputType = 0;
			break;
		}

		// run the report
		returnValue = loop();

		if (returnValue == 1) {
			char yn = ' ';
			while (yn != 'y' && yn != 'n') {
				cout << "Would you like to run again?\n\ty|yes\n\tn|no\ninput: ";
				cin >> yn;
			}
			if (yn == 'n') returnValue = 0;
			else {
				cout << endl;
				yn = 'n';
				while (yn != 'y' && yn != 'n') {
					cout << "Turn paging on (beta)?\n\ty|yes\n\tn|no\ninput: ";
					cin >> yn;
				}
				PAGES = (yn == 'y');
				cout << endl;
			}
		}
		asterisks();
	}


	cout << endl << "Nothing left to do. Terminating..." << endl;

	//RETURN
	system("pause");
	return returnValue;
}

// alternative to main
// runs the report all the way through
int loop() {
	
	

	string name,
		inputLine,
		outputLine,
		currentCourse,
		currentInstructor;

	int lastCompletedIndex = -1;


	ifstream inFile;
	ofstream outFile;

	for (int processIncrementor = 0; processIncrementor < 2; processIncrementor++)  {
		
		asterisks();

		cout << "Opening input file..." << endl;

		bool firstRecord = true;
		bool firstLine = true;
		bool ignoreExtraNewLines = false;

		int currentSemester = 0;
		int courseSections = 0,
			semesterSections = 0,
			instructorSections = 0;

		//Open input file
		inFile.open(INPUT_FILE_NAME);

		/*
			IF Input File Not Found THEN
				MSG “Unable to locate File”
				Return with error code -1
			ENDIF
		*/
		if (!inFile) {
			cout << endl << "\nERROR!\nInput file not found!\nPlease add CIS225HW1DA.txt into program directory and re-run.\nError code: -1\n" << endl;
			return -1;
		}

		cout << "Input file exists." << endl;
			
		cout << endl;

		string outFileName = OUTPUT_FILE_NAME;

		if (outputType != 2) {
			cout << "Opening output file..." << endl;

			char yn = ' ';
			while (yn != 'y' && yn != 'n') {
				cout << "Output file defaults to: " + outFileName + ".\n\nWould you like to enter a different name for this specific report?\n\ty|yes\n\tn|no\ninput: ";
				cin >> yn;
				cout << endl;
			}
			if (yn == 'y') {
				cout << "\nEnter name for output file: ";
				cin >> outFileName;
				if ( getFilenameExtention(outFileName) == "" || outFileName.find(".") < 0 ) {
					cout << endl << "No filename extention found. Appending \".txt\" extention to filename...";
					outFileName += ".txt";
					cout << endl << "Program will output to: \"" << outFileName << "\"." << endl;
				}
				cout << endl << endl;
			}

			// Open Output File
			outFile.open(outFileName);

			cout << "Success." << endl;
		} else {
			cout << "No file output. Reports will print to screen..." << endl;
			//outFile.open(outFileName);
		}
			
		cout << "Preparing report..." << endl;

		asterisks();

		int currentLine = 0,
			currentPage = 1;

		//if (PAGES) cout << endl << "NOTICE: Paging is turned ON. Page length is set to default size of " << PAGE_SIZE << " lines." << endl;
		//else cout << endl << "Paging is off. Report will print continously." << endl;
		//cout << "To turn paging on or off, re-run the report after it is finished and enter 'y' or 'n' when asked to enable or disable paging." << endl << endl;
	
		char semesterInput = ' ';
		char instructorInput = ' ';
	
			

		if (processIncrementor == 0) {
			do {
				cout << "Semester Report\n\ta|select individual semester\n\tb|show all semesters\n\tc|skip\ninput: ";
				cin >> semesterInput;
			} while (semesterInput != 'a' && semesterInput != 'b' && semesterInput != 'c');
			//processIncrementor = 1;
		} else if (processIncrementor == 1) {
			do {
				cout << "Instructor Report\n\ta|select individual instructor\n\tb|show all instructors\n\tc|skip\ninput: ";
				cin >> instructorInput;
			} while (instructorInput != 'a' && instructorInput != 'b' && instructorInput != 'c');
			//processIncrementor = 0;
		}

		// skip
		if (semesterInput == 'c' || instructorInput == 'c') {
			inFile.close();
			outFile.close();
			string inputType = "semester report";
			if (processIncrementor == 1) inputType = "instructor report";
			cout << endl << "Skipping " << inputType << "...";
			asterisks();
			continue;
		}

		/*
			new stuff
		*/


		
			
		

		//Call CreateArrays
		classType classes[LENGTH_OF_FILE];

		// set loopIndex = 0
		int loopIndex = 0;

		//While NOT InputFile.EOF (End Of File)
		while (inFile) {

			//Read Line
			getline(inFile, inputLine);

			//Call ExtractFields
			extractFields(inputLine, classes, loopIndex++);

		//WEND
		}

		//RangeOfSemesters = AskUserForRangeOfSemesters

		int beginningRange=0,
			endingRange=999999999;

		// ask beginning range
		/*cout << "Please enter the beginning string for the range of semesters between " << classes[LENGTH_OF_FILE-2].semesterCode << " and " << classes[1].semesterCode << ": ";
		cin >> beginningRange;
		cout << endl;

		// ask ending range
		cout << "Please enter the ending string for the range of semesters between " << classes[LENGTH_OF_FILE-2].semesterCode << " and " << classes[1].semesterCode << ": ";
		cin >> endingRange;
		cout << endl;*/
		int semesterToPrint = 0;
		string instructorToPrint = "";

		sortByInstructor(classes);

			
		asterisks();

		if (processIncrementor == 0) {
				cout << "***Semester Report" << endl;
				//sort(classes);
				if (semesterInput == 'a') {
					// individual semester
					cout << "Generating semester report for individual semester...";
					semesterToPrint = coutPossibleSemesters(classes);

				}
				if (semesterInput == 'b') {
					// all semesters
					cout << "Generating semester report for all semesters...";
				}

			} else if (processIncrementor == 1) {
				cout << "***Instructor Report" << endl;
				if (instructorInput == 'a') {
					cout << "Generating instructor report for individual instructor...";
					instructorToPrint = coutPossibleInstructors(classes);

						
				}
				if (instructorInput == 'b') {
					cout << "Generating instructor report for all instructors...";
				}
			}


		cout << "\n\nPrinting report...";

		if (outputType != 2) asterisks();

		// set numLoops
		int numLoops = -1;
		bool lastLine = true;
		int completedLines = 0;

		//loop array length
		while (  (numLoops++ < LENGTH_OF_FILE-1) ) {

			
			bool completeLoop = false;
			bool halfLoop = false;

			bool isWithinBeginningBounds = false,
				isWithinEndingBounds = false;

			// compare boundaries
			isWithinBeginningBounds = (beginningRange <= classes[numLoops].semesterCode);
			isWithinEndingBounds = (classes[numLoops].semesterCode <= endingRange);



			if (processIncrementor == 0) {
				if (semesterInput == 'a') {
					// individual semester
					// true if currentSemester == user chosen semester
					completeLoop = (semesterToPrint == classes[numLoops].semesterCode);

					if ( !completeLoop && completedLines+1 < numLoops )  {
						//completedLines = LENGTH_OF_FILE-1;
							
					} else if ( !completeLoop ) {
						lastLine = true;
					}// else if (!completeLoop && completedLines > 0) lastLine = true;

					//if (classes[numLoops].instructor != classes[lastCompletedIndex].instructor) ignoreExtraNewLines = false;

				}
				if (semesterInput == 'b') {
					// all semesters
					// true if semester is within range
					completeLoop = (numLoops >= 0);
					if (completedLines >= LENGTH_OF_FILE-2) lastLine = true;
					//if (numLoops < 2) ignoreExtraNewLines = true;
				}
			} else if (processIncrementor == 1) {
					
					
				if (instructorInput == 'a') {
					// individual instructor
					// true if currentInstructor == user chosen instructor
					completeLoop = (instructorToPrint == classes[numLoops].instructor);

					if ( completedLines < LENGTH_OF_FILE && ((!completeLoop && classes[numLoops].instructor != classes[lastCompletedIndex].instructor) || completedLines == 0 ) ) {
						lastLine = false;
						if (completedLines != 0) completedLines = LENGTH_OF_FILE;
					} else if (!completeLoop && completedLines > 0) lastLine = true;

						
						
					// special case for last line
					// if the instructor is different,
					// then fall through one last time...

				}
				if (instructorInput == 'b') {
					// all instructors
					// print every one
					completeLoop = (isWithinBeginningBounds && isWithinEndingBounds);
				}
			}

			// if the current semester string is within the boundaries of the user's defined beginningRange and endingRange
			if ( (completeLoop == true) || (lastLine == false) ) {
					
				lastLine = (!completeLoop);
					
				
				// then continue with the current loop and print whatever detail and course lines are necessary, just like before
				//* code from before!!!!


				// used for paging mechanism
				// currently beta
				if (PAGES && ++currentLine == PAGE_SIZE) {
					// page break time
					currentLine = 0;
					reportFooter(outFile);
					detailLine("", outFile);
					string breakDetail = "*** Page Break ***";
					lengthen(breakDetail, 45, true);
					detailLine(breakDetail, outFile);
					detailLine("", outFile);
					firstRecord = true;
					currentPage++;
				}

				// keep track of how many of each
				courseSections++;
				semesterSections++;
				instructorSections++;

				// the output line
				outputLine = "";

				// MAJOR BREAK
				/*IF First_Record THEN
					Current_Instructor = Instructor
					Current_Semester = SemesterCode
					(note that First_Record must be set to False after printing the detail line… You need to keep track of the first row in each semester and first course detail line for control break suppression)
				ENDIF*/
				if (firstRecord) {
					// called whenever there's a new "page"
					//currentSemester = semesterCode[numLoops];
					//currentCourse = courseID[numLoops];
					firstRecord = false;
					courseSections = 0;
					semesterSections = 0;
					reportHeader(outFile, currentPage);
					ignoreExtraNewLines = true;
				}

				string instructorFooterLine = "";
				// MINOR BREAK
				/*IF Current_Instructor <> Instructor THEN
					Call InstructorFooter
				ENDIF*/
				if (currentInstructor != classes[numLoops].instructor) {
					// course is NOT the same as before
					if (instructorSections != 0 && currentInstructor != "" && !ignoreExtraNewLines) instructorFooterLine = instructorFooter(currentInstructor, instructorSections, outFile);

					//if 

					instructorSections = 0;
					// semester is NOT the same as before, either
					if (currentInstructor != classes[numLoops].instructor) outputLine += classes[numLoops].instructor;
					currentInstructor = classes[numLoops].instructor;
						
					if (!firstLine) currentSemester = 0;
					ignoreExtraNewLines=false;
				}

				//if (lastCompletedIndex > 0 && classes[lastCompletedIndex].instructor != currentInstructor) instructorFooterLine = "";
					
				lengthen(outputLine, 14);
					
				// MINOR BREAK
				/*IF Current_Instructor <> Instructor THEN
					Call InstructorFooter
				ENDIF*/
				if (currentSemester != classes[numLoops].semesterCode) {
					// course is NOT the same as before
					if ( (courseSections != 0 && currentSemester != 0)) courseFooter(currentCourse, classes[numLoops-1].semesterName, courseSections, outFile);
					currentCourse = classes[numLoops].course.ID;
					courseSections = 0;
					// semester is NOT the same as before, either
					if (currentSemester != classes[numLoops].semesterCode) outputLine += classes[numLoops].semesterName;
					//else if (classes[lastCompletedIndex].instructor == classes[numLoops].instructor) outputLine += classes[numLoops].semesterName;


					//lengthen(outputLine, 27);
					//lengthen(outputLine += currentCourse + "-" + classes[numLoops].course.section, 38);
						
				} else {
					// print semester code WHEN:
					// semester is the same as before AND the instructor is different
					// semester is different from before AND the instructor is the same AND it's the first instance of a course
					//if ( (currentSemester == classes[numLoops].semesterCode && currentInstructor != classes[numLoops].instructor) ||
						//(currentSemester != classes[numLoops].semesterCode && currentInstructor == classes[numLoops].instructor && courseSections == 0) ) outputLine += classes[numLoops].semesterName;
				}// else lengthen(outputLine += currentCourse + "-" + classes[numLoops].course.section, 38);
					
				lengthen(outputLine, 27);
				lengthen(outputLine += currentCourse + "-" + classes[numLoops].course.section, 38);
					
				/*IF current_Semester <> Semester THEN
					Call SemesterFooter
				ENDIF*/

				if (currentSemester != classes[numLoops].semesterCode) {
					// semester is different!
			//@FIXME need to print semester footer for instructors who only ever show up once
					if ( (semesterSections != 0 && currentSemester != 0) && completedLines > 0 ) semesterFooter(classes[numLoops-1].semesterName, semesterSections, outFile);
					//else if ((lastCompletedIndex != -1)) cout << (classes[numLoops].instructor != classes[lastCompletedIndex].instructor) << " " << lastCompletedIndex << " " <<numLoops<<endl;
					currentSemester = classes[numLoops].semesterCode;
					semesterSections = 0;

					// ignore extra chars at the end and beginning of pages
					if ( !(numLoops == (LENGTH_OF_FILE-1) ) && !ignoreExtraNewLines) outputLine = "\n\n" + outputLine;

					currentCourse = classes[numLoops].course.ID;
				}

				if (instructorFooterLine != "" && completedLines > 0) detailLine(instructorFooterLine, outFile);

				//lengthen(outputLine += classes[numLoops].course.section, 33);
				//lengthen(outputLine += classes[numLoops].instructor, 47);
					
				lengthen(outputLine += classes[numLoops].meeting.dow, 48);
				lengthen(outputLine += classes[numLoops].meeting.time, 63);
				lengthen(outputLine += classes[numLoops].location.room + " " + classes[numLoops].location.buildingName, 84);
				//lengthen(outputLine += classes[numLoops].location.buildingName, 94);

				//Call DetailLine
				if ( !(numLoops == (LENGTH_OF_FILE-1) || lastLine != false) && numLoops > 0 ) {
					detailLine(outputLine, outFile);
					completedLines++;
				}
				ignoreExtraNewLines = false;
				lastCompletedIndex = numLoops;
					

				//* end of code from before!!!
			}
			// else don't do this, skip this line
			else {
				firstLine = false;
				//currentInstructor = "";
				//currentSemester = 0;
				//courseSections = 0;
				//courseSections--;
				//semesterSections--;
				continue;
			}

		/*
			end of new stuff
		*/


		}
		//FEND
	

		//cout << "Details output success!\nprinting footer..." << endl;

		// prints fancy spacing tabs
		reportFooter(outFile);

		asterisks();

		// outFile
		if (outputType == 1) cout << endl << "Closing files...\nOutput to file success!\nLook for " + outFileName + " in your program directory." << endl;
		// cout
		if (outputType == 2)  cout << endl << "\nClosing files...\nOutput to screen success!" << endl;
		// both
		if (outputType == 0)  cout << endl << "\nClosing files...\nOutput to screen and file success!\nLook for " + outFileName + " in your program directory." << endl;

		//cout << "Closing files..." << endl;

		inFile.close();
		outFile.close();

		asterisks();

		if (processIncrementor == 0) cout << "Preparing to generate instructor report...";
		if (processIncrementor == 1) cout << "";

		//cout << endl << "Report printed to output file successfully!\nLook for " + outFileName + " in your program directory.\n" << endl;
	}

	


	return 1;
}

// similar to std::string's to_string, also similar to stoi() function, except it actually works
// i wrote this, because this is the only way it will ever be done.
int to_int(string s) {
	int i = 0;
	int currentIteration = 0;
	for each(char c in s) {
		int cton = 0;
		switch (c) {
		case '0':cton=0;break;
		case '1':cton=1;break;
		case '2':cton=2;break;
		case '3':cton=3;break;
		case '4':cton=4;break;
		case '5':cton=5;break;
		case '6':cton=6;break;
		case '7':cton=7;break;
		case '8':cton=8;break;
		case '9':cton=9;break;
		default:cton=0;break;
		}
		i = i + (cton * pow(10.0, static_cast<double>(s.length() - currentIteration-1)) );
		currentIteration++;
	}
	return i;
}
// takes a line from the file and inserts the correct values into the variable references passed
void extractFields(string inputLine, classType classes[], int index) {
	
		//SemesterCourseIDSection|SemesterCode|CourseID|Section|Instructor|DOW|time|Building|Room
		//201230CIS 10501|201230|CIS 105|01|Trivedi, c|--T-R--|11:00a-12:15p|A |378|

	//cout << "Parsing line: \n" << inputLine << endl;
	string remainingParse = inputLine,
		vars[10];
	int whereAt = 0;

	// semesterCode is always 6 digits long
	classes[index].semesterCode = to_int(remainingParse.substr(0, whereAt+=6));
	
	// shrink what's left
	cut(remainingParse, to_string(classes[index].semesterCode).length());
	
	// extract values to an array quickly by delimiter '|'
	for (int i = 0; i < 9; i++) {
		int x = remainingParse.find('|');
		if (x == -1) continue;
		// put into the array
		vars[i] = remainingParse.substr(0, x);
		// shrink what's left again
		cut(remainingParse, vars[i].length() + 1);
	}
	
	// now put the values back into the references where they belong
	classes[index].semesterCode = to_int(vars[1]);

	if (vars[1].length() > 1) {
		string semesterShort = vars[1].substr(4,2),
			actualYear = vars[1].substr(0,2);
		actualYear += (semesterShort == "50") ? to_string( (to_int(vars[1].substr(2,2)) + 1) ) : vars[1].substr(2,2);

		//cout << "year: " << vars[1] << ", actual year: " << actualYear << ", substr: " << to_int(vars[1].substr(2,2)) << endl;

		classes[index].semesterName = findInTable(semesterShort, "semester") + " " + actualYear;
	} else classes[index].semesterName = vars[1];
	
	classes[index].course.ID = vars[2];
	classes[index].course.section = vars[3];
	classes[index].instructor = vars[4].substr(0);
	classes[index].instructordate = vars[4].substr(0) + to_string(classes[index].semesterCode);
	classes[index].meeting.dow = vars[5];
	classes[index].meeting.time = vars[6];
	//replace(classes[index].meeting.time, "p", "PM");
	//replace(classes[index].meeting.time, "a", "AM");

	classes[index].location.building = vars[7];
	classes[index].location.buildingName = findInTable(vars[7], "building");
	if (classes[index].location.buildingName == "") classes[index].location.buildingName = "Online";
	classes[index].location.room = vars[8];
	
	// and done.
	return;
}

// prints a report header
void reportHeader(ofstream &outFile, int currentPage) {
	string firstline = "",
		header1 = "Quinsigamond Community College [2013]",
		header2 = "CIS225-70 Instructor Course Schedule Listing [Bryan Leasot]",
		addon = "PAGE: ZZZ" + to_string(currentPage);

	// add space before & after
	lengthen(header1, 28, true);
	lengthen(header1, 80);
	// add page #
	header1 += addon;

	lengthen(header2, 16, true);

	// output them all
	//detailLine(getSpecialLine("tab"), outFile);
	//detailLine(getSpecialLine("whitespace"), outFile);
	detailLine(header1, outFile);
	detailLine(header2, outFile);

	// build the very first line w/ column names
	lengthen(firstline += "Instructor", 14);
	lengthen(firstline += "Semester", 28);
	lengthen(firstline += "Course", 38);
	//lengthen(firstline += "Instructor", 51);
	lengthen(firstline += "DOW", 53);
	lengthen(firstline += "Time", 64);
	lengthen(firstline += "Room/Building", 80);
	//lengthen(firstline += "Building", 94);

	// and show it
	detailLine("\n" + firstline + "", outFile);
	return;
}

// prints footer for the report
void reportFooter(ofstream &outFile) {
	//detailLine(getSpecialLine("whitespace"), outFile);
	//detailLine(getSpecialLine("tab"), outFile);
}
// prints course footer (1st ctrl break)
void courseFooter(string currentCourse, string currentSemester, int courseSections, ofstream &outFile) {
	return;
	string adverb = (courseSections == 1) ? "is " : "are ",
		plural = (courseSections == 1) ? "" : "s",
		outLine = "";

	lengthen(outLine += "In semester " + currentSemester + " there " + adverb + to_string(courseSections) + " section" + plural + " of " + currentCourse + ".", 27, true);
	//outFile << "\n" << outLine;
	detailLine("\n"+outLine, outFile, false);
	return;
}
// prints semester footer (2nd ctrl break)
void semesterFooter(string currentSemester, int semesterSections, ofstream &outFile) {
	string adverb = (semesterSections == 1) ? "is " : "are ",
		plural = (semesterSections == 1) ? "." : "s.",
		outLine = "";

	lengthen(outLine += "In semester " + currentSemester + " there " + adverb + to_string(semesterSections) + " total section" + plural + "", 14, true);
	outLine = "\n" + outLine;

	detailLine(outLine, outFile, false);
	
	return;
}
string instructorFooter(string currentInstructor, int instructorSections, ofstream &outFile) {
	string adverb = (instructorSections == 1) ? "is " : "are ",
		plural = (instructorSections == 1) ? "." : "s.",
		outLine = "";

	outLine += "\n\nInstructor " + currentInstructor + " has " + to_string(instructorSections) + " total section" + plural + "";
	return outLine;
};
// outputs a line to the file
void detailLine(string inputLine, ofstream &outFile) {
	
	return detailLine(inputLine, outFile, true);
}
// outputs a line to the file
void detailLine(string inputLine, ofstream &outFile, bool addEndl) {
	// outfile 
	if (outputType != 2) {
		outFile << inputLine;
		if (addEndl == true) outFile << endl;
	}
	// cout
	if (outputType != 1) {
		cout << inputLine << endl;
		if (addEndl == true) cout << endl;
	}

	//outFile << inputLine << endl;
	return;
}
// copies entire contents of string array to destination
void copyTo(string destination[], string source[], int length) {
	for (int i = 0; i < length; i++) destination[i] = source[i];
}
//@* extras
// returns the variable associated with the first parameter in the 2nd column of the table
string findInTable(string tofind, string tableName) {
	string searchIn[5],
		returnVal[5];

	if (tableName == "building") {
		copyTo(searchIn, BUILDING_CODES[0], 5);
		copyTo(returnVal, BUILDING_CODES[1], 5);
	} else if (tableName == "semester") {
		copyTo(searchIn, SEMESTER_CODES[0], 5);
		copyTo(returnVal, SEMESTER_CODES[1], 5);
	}

	for (int i = 0; i < 5; i++) if (searchIn[i] == tofind) return returnVal[i];
	
	//cout << "finding.... " << tofind << " from table, notfound.=("<<endl;
	
	return tofind;
}
// cuts array reference to size
void cut(string &tocut, int cutlength) {
	//cout << "cut!\n";
	if (tocut == "") return;
	if (cutlength < 0) cutlength = 0;
	tocut = tocut.substr(cutlength);
	//cout << "cuttin!\n";
}
// extends the end of a string with whitespace chars to specified size
void lengthen (string &tolengthen, int length) {
	for (int i = tolengthen.length(); i >= 0; --i) if (tolengthen.substr(i, i + 1) == "\n") length += 2;
	if (tolengthen.length() < length) for (int i = 0; i < tolengthen.length() - length; i++) tolengthen += " ";
}
// extends the beginning of a string with whitespace chars to specified size
void lengthen (string &tolengthen, int length, bool before) {
	if (!before || before == false) return lengthen(tolengthen, length);
	string s = "";
	for (int i = 0; i < length; i++) s += " ";
	s += tolengthen;
	tolengthen = s;
}
// returns special spacing line
// accepts "tab" or "whitespace" for args
string getSpecialLine(string type) {
	string tabline = "",
		whitespaceline = "";
	for (int tab = 1; tab <= 11; tab++) {
		string t = to_string(static_cast<long double>(tab));
		lengthen(tabline, tabline.length() + 10 - t.length());
		tabline += t;

		for (int whitespace = 1; whitespace < 11; whitespace++)
			whitespaceline += (whitespace < 10) ? to_string(static_cast<long double>(whitespace)) : "0";
	}
	if (type == "tab") return tabline;
	if (type == "whitespace") return whitespaceline+"1234";
	return "";
}
bool arrayContains(string arr[], string con) {
	for (int i = 0; i < 100; i++ ) if (arr[i] == con) return true;
	return false;
}
int coutPossibleSemesters(classType classes[]){
	string shown[100];
	int numshown = 0;

	cout << "\n\nSEMESTERS:\n";

	for (int i = 0; i < LENGTH_OF_FILE; i++) {
		if (arrayContains(shown, to_string(classes[i].semesterCode)) || classes[i].semesterCode == 0) continue;
		else {
			cout << to_string(numshown) << "| " << classes[i].semesterCode << endl;;
			shown[numshown++] = to_string(classes[i].semesterCode);
		}
	}
	cout << to_string(numshown);


	int semesterToPrint = 0;
	cout << "Enter index of semester for which you wish to print: ";
	cin >> semesterToPrint;

	return to_int(shown[semesterToPrint]);

};
string coutPossibleInstructors(classType classes[]) {
	string shown[100];
	int numshown = 0;

	cout << "\n\nINSTRUCTORS:\n";

	for (int i = 0; i < LENGTH_OF_FILE; i++) {
		
		if (arrayContains(shown, classes[i].instructor)) continue;
		else {
			cout << to_string(numshown) << "| " << classes[i].instructor << endl;;
			shown[numshown++] = classes[i].instructor;
		}
	}
	

	int instructorToPrint;
	cout << "Enter index of instructor for which you wish to view: ";
	cin >> instructorToPrint;

	return shown[instructorToPrint];
};

int correctDigits = 0;
/*void sortArray(int arrayToSort[]) {
	// Imperative
	correctDigits = 0;

	int length = 100;

	//if (verbose) System.out.println("Sorting........");
	//if (verbose) System.out.println("Old:");
	//this.printArray(arrayToSort);

	// call selectAndSwap using a loop, each time decreasing the number of
	// els by 1
	int els = length;
	while (--els > 0)
		selectAndSwap(arrayToSort, els);
	// ...................................
	//if (verbose) System.out.println("New:");
	//this.printArray(arrayToSort); // should be sorted now
	//if (verbose) System.out.println("Sort complete.\n");
}

void selectAndSwap(int a[], int els) {
	// select the greatest element and swap with the one at els
	// ....................
	int length = 100;

	int bounds = length - correctDigits++,
		greatestIndex = 0,
		swap;
	// find index of greatest number, the fast way
	while (--bounds > 0) if (a[greatestIndex] < a[bounds]) greatestIndex = bounds;
	// back it up
	swap = a[els];
	//if (this.debug) System.out.println("swapping " + a[greatestIndex] + " at index " + greatestIndex + " with " + swap + " at index " + els);
	// perform the swap
	a[els] = a[greatestIndex];
	a[greatestIndex] = swap;
	// ................ done.
}
*/

void sortByInstructor(classType classes[]) {
	// Imperative
	correctDigits = 0;
	int length = LENGTH_OF_FILE;

	// -------------------
	// call selectAndSwap using a loop, each time decreasing the number of
	// els by 1
	int els = length;
	while (--els > 0)
		selectAndSwap(classes, els);
	// ...................................
	
	
	
}

void selectAndSwap(classType a[], int els) {
	int length = LENGTH_OF_FILE;
	// sorts alphabetically
	// select the greatest element and swap with the one at els
	// ....................
	int bounds = length - correctDigits++,
		greatestIndex = 0;
	classType swap;
	// find index of greatest number, the fast way
	// sets new greatest index IF the string at bounds index is already in dictionary order
	// 0 is equal, -1 is dict order, 1 is out of order
	
	while (--bounds > 0) if (strcmp(a[greatestIndex].instructordate.c_str(), a[bounds].instructordate.c_str()) < 0) greatestIndex = bounds;
	// back it up
	swap = a[els];
	//if (this.debug) System.out.println("swapping " + a[greatestIndex] + " at index " + greatestIndex + " with " + swap + " at index " + els);
	// perform the swap
	a[els] = a[greatestIndex];
	a[greatestIndex] = swap;
	// ................ done.
}
void asterisks () {
	cout << endl << endl << setfill('*') << setw(80) << "\n" << endl;
}

//http://stackoverflow.com/questions/51949/how-to-get-file-extension-from-string-in-c
string getFilenameExtention(string fullname) {
	std::string filename(fullname);
	std::string::size_type idx;

	idx = filename.rfind('.');

	if(idx != std::string::npos)
	{
		return filename.substr(idx+1);
	}
	else
	{
		// No extension found
		return filename.substr(idx+1);
	}
}


/*
void sort(string strings[]) {
	// Imperative
	correctDigits = 0;
	int length = 100;

	// -------------------
	// call selectAndSwap using a loop, each time decreasing the number of
	// els by 1
	int els = length;
	while (--els > 0)
		selectAndSwap(strings, els);
	// ...................................
	
} // end of sort

void selectAndSwap(string a[], int els) {
	int length = 100;
	// sorts alphabetically
	// select the greatest element and swap with the one at els
	// ....................
	int bounds = length - correctDigits++,
		greatestIndex = 0;
	string swap;
	// find index of greatest number, the fast way
	// sets new greatest index IF the string at bounds index is already in dictionary order
	// 0 is equal, -1 is dict order, 1 is out of order
	
	while (--bounds > 0) if (strcmp(a[greatestIndex].c_str(), a[bounds].c_str()) < 0) greatestIndex = bounds;
	// back it up
	swap = a[els];
	//if (this.debug) System.out.println("swapping " + a[greatestIndex] + " at index " + greatestIndex + " with " + swap + " at index " + els);
	// perform the swap
	a[els] = a[greatestIndex];
	a[greatestIndex] = swap;
	// ................ done.
}*/
