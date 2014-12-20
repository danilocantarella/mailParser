/*
This file is part of MailParser package.
Writen by
        - Danilo Cantarella (https://github.com/Flyer-90);
        - Sebastiano Siragusa (https://github.com/sebysira);
        - Filippo Randazzo (https://github.com/filirnd);

Full MailParser is released by GPL3 licence.
*/


#include "config.h"
#include "timerCalculator.h"
#include "db.h"

void processDirectory();
int processFile(char*, int);
void insertFromFile();
void deleteFromFile();

/*Function to process all files in one directory*/
void processDirectory() {
	char path[MAX];
	struct timeval start_time;							/*Struct to store initial timestamp*/
	int nFile = 0, nInsert = 0;
	printf("\nInsert your path: ");
	scanf("%s", path);								/*Get input from user*/
	gettimeofday(&start_time,NULL); 						/*Get current time*/
	DIR* dir;
	struct dirent *dirent;
	dir = opendir(path);								/*Open directory*/
	if (dir == NULL) {
		printf("Cannot open directory\n");
	}
	else {
		while ((dirent = readdir(dir)) != NULL) {								/*Read all files in directory*/
			if ((strcmp(dirent->d_name, ".") == 0) || (strcmp(dirent->d_name, "..") == 0)) 			/*Ignore special directory . and ..*/
				continue;
			char *last;							/*Char to store file's extension*/
			last = strrchr(dirent->d_name, '.');				/*Get token after last . in name file*/
			if (last != NULL) {
				last = strndup(last + sizeof(char), sizeof(char)*strlen(last)); 			/*cut the . from extension*/
				if (strcmp(last, "txt") == 0 || strcmp(last, "doc") == 0 || strcmp(last, "rtf") == 0 	/*process only known files*/
				  || strcmp(last, "csv") == 0) {
					printf("Processing file %s...\n", dirent->d_name);
					char statement[MAX];								/*Complete file's path*/
					snprintf(statement, MAX, "%s/%s", path, dirent->d_name);
					nInsert = nInsert + processFile(statement, 1);					/*Process file*/
					nFile++;
				}
			}
		}
	}
	closedir(dir);
	printf("Have been processed %d files.\n", nFile);
	printf("Have been inserted %d email.\n", nInsert);
	executionTime(start_time);
}

/*Function to read and process one file*/
int processFile(char* filename, int mode) {
	FILE *input;
	MYSQL* conn;									/*Connection to db*/
	regex_t regex;
	regex_t regex2;
	int res, resSub, mailRes, lenght, counter = 0;
	char line[MAX_LENGHT];
	char *token;

	input = fopen(filename, "r");							/*Open file in read mode*/

	if (input == NULL){
		printf("Error: unable reading input file\n");
		return;
	}

	res = regcomp(&regex, reg_mail, REG_EXTENDED);					/*Compile regular expression*/
	if (res) {
		fprintf(stderr, "Could not compile regex\n");
		return;
	}

	resSub = regcomp(&regex2, reg_domain, REG_EXTENDED);				/*Compile regular expression*/
	if (resSub) {
		fprintf(stderr, "Could not compile resSub regex\n");
		return;
	}

	while (1) {
		if(fscanf(input, "%s", line) == EOF) 					/*while don't read end of file*/
		     break;

		token = strtok(line, " ,;");						/*Split for space , and ;*/
		while (token != NULL) {							/*while there are tokens*/
			makeLower(token);						/*toLower token*/
			mailRes = matchMail(regex, regex2, token, mode);		/*check if token is a valid e-mail*/
			if(mailRes == 1 || mailRes == 0)
			   counter++;
			token = strtok(NULL, " ,;");					/*go to next token*/
		}

	}
	fclose(input);
	regfree(&regex);
	regfree(&regex2);
	return counter;
}

/*Function that gives one file and call function that insert mails into DB*/
void insertFromFile() {
	char path[MAX];
	struct timeval start_time;							/*Struct to store initial timestamp*/
	printf("\nInsert file's path: ");
	scanf("%s", path);								/*Get input from user*/
	gettimeofday(&start_time, NULL); 						/*Get current time*/
	printf("Processing file %s.\n", path);
	int res = processFile(path,1);							/*Process file*/
	printf("Have been inserted %d email.\n", res);
	executionTime(start_time);
}

/*Function that gives one file and process it and call function that delete mails from DB*/
void deleteFromFile() {
	char path[MAX];
	struct timeval start_time;							/*Struct to store initial timestamp*/
	printf("\nInsert file's path: ");
	scanf("%s", path);								/*Get input from user*/
	gettimeofday(&start_time, NULL); 						/*Get current time*/
	printf("Processing file %s...\n", path);
	int res = processFile(path,2);							/*Process file*/
	printf("Have been deleted %d email...\n", res);
	executionTime(start_time);
}


/*
This file is part of MailParser package.
Writen by
        - Danilo Cantarella (https://github.com/Flyer-90);
        - Sebastiano Siragusa (https://github.com/sebysira);
        - Filippo Randazzo (https://github.com/filirnd);

Full MailParser is released by GPL3 licence.
*/
