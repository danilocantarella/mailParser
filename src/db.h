/*
This file is part of MailParser package.
Writen by
        - Cantarella Danilo (https://github.com/Flyer-90);
        - Randazzo Filippo (https://github.com/filirnd);
        - Siragusa Sebastiano (https://github.com/sebysira);

Full MailParser is released by GPL3 licence.
*/

#include "config.h"

MYSQL* connection(void);
int closeConnection(MYSQL*);
void initDB();
void insertMail();
void deleteMail();
void printToFile();
void printDomainsToFile();
void searchMail();
void makeLower(char *);

struct timeval start_time;								/* Struct to store initial timestamp */
char SYMBOL_TABLE[] = "0123456789abcdefghijklmnopqrstuvwxyz!#$%&'+-/=?^_{|}~";		/*Names of all tables*/


/* Connection to DB function */
MYSQL* connection() {

	MYSQL *conn;
	conn = mysql_init(NULL);							/*Init connection*/

	if (!mysql_real_connect(conn, server, user, password, "mailParser", 0, NULL, 0)) {  	/* Connect to database */
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}
	return conn;
}



/* Exec query function*/
int query(MYSQL* conn, char* str) {

	if (mysql_query(conn, str)) {							/*Query to database*/
		/*fprintf(stderr, "%s\n", mysql_error(conn));*/
	}
	/*else
	        printf("Executed query %s\n", str);*/
}



/* Close connection function */
int closeConnection(MYSQL* conn) {

	mysql_close(conn);								/*Close connection to db*/
	return 0;
}




/* This function control if DB exist, if not exist , the function creates DB */
void checkDB() {
	MYSQL *conn = mysql_init(NULL);
	if (mysql_real_connect(conn, server, user, password, NULL, 0, NULL, 0) == NULL) 	/*Open a connection without specifying a database*/
	{
	      fprintf(stderr, "%s\n", mysql_error(conn));
	      mysql_close(conn);
	      exit(1);
	}

	if (mysql_query(conn, "CREATE DATABASE IF NOT EXISTS mailParser")) 			/*try to create db if not exists*/
	{
	      fprintf(stderr, "%s\n", mysql_error(conn));
	      mysql_close(conn);
	      exit(1);
	}

	mysql_close(conn);
}


/* Function that intialize DB */
void initDB() {
        checkDB();
	MYSQL *conn;
	char statement[MAX];

	snprintf(statement, MAX, "select count(distinct `table_name`) from `information_schema`.`columns` where `table_schema` = \"mailParser\" ");
	conn = connection();
	if (!mysql_query(conn, statement)) {				/*Query to check how much tables are in db*/
		MYSQL_RES* result = mysql_store_result(conn);
		MYSQL_ROW row;
		row = mysql_fetch_row(result);				/*Fetching result from query*/
		if (atoi(row[0]) < (strlen(SYMBOL_TABLE) + 1) ) {	/*Check if there are all tables*/
			int i;
			for (i = 0; i < strlen(SYMBOL_TABLE); i++){ 	/*Create, if not exists, all tables in db*/
				snprintf(statement, MAX, "CREATE TABLE IF NOT EXISTS `%c` (mail varchar(255) NOT NULL, PRIMARY KEY (mail))", SYMBOL_TABLE[i]);
				conn = connection();
				query(conn, statement);
				closeConnection(conn);
			}

			conn = connection();				/*create special table for warning e-mails*/
			snprintf(statement, MAX, "CREATE TABLE IF NOT EXISTS warning (mail varchar(255) NOT NULL, PRIMARY KEY (mail))");
			query(conn, statement);
			closeConnection(conn);
			printf("\nTables created in database 'mailParser'.\n");
		}
	}
	else
	       fprintf(stderr, "%s\n", mysql_error(conn));
}

/* Function that search mail into DB*/
void searchMail() {
	char mail[MAX];
	printf("\nInsert mail to search: ");
	scanf("%s", mail);										/*Get input from user*/
	gettimeofday(&start_time,NULL); 								/*get current time*/
	makeLower(mail);										/*Mail toLowe*/
	char firstChar = mail[0];									/*get first charachter from char*/
	char statement[MAX];
	snprintf(statement, MAX, "SELECT * FROM `%c` where `mail` LIKE '%s%%'", firstChar, mail);	/*create select query*/
	MYSQL* conn;
	conn = connection();										/*connect to db*/
	if (!mysql_query(conn, statement)) {								/*exec select query*/
		MYSQL_RES* result = mysql_store_result(conn);
		int n = mysql_num_rows(result);								/*number or rows from db*/
		if(n > 0) {										/*if there is at least one result*/
			MYSQL_ROW row;
			while ((row = mysql_fetch_row(result))) {					/*read all lines*/
			    printf("Email %s found in table %c.\n", row[0], firstChar);		/*print all e-mails founded*/
			}
			printf("Found %d email.\n", n);
		}
		else
			printf("Email %s not found.\n", mail);
	}
	else
	       fprintf(stderr, "%s\n", mysql_error(conn));
	closeConnection(conn);
	executionTime(start_time);
}

/*Function that insert single mail into DB*/
void insertMail() {
	MYSQL *conn;
	char mail[MAX];
	int res, resSub, mailRes;
	regex_t regex, regex2;

	printf("\nInsert email: ");
	scanf("%s", mail);										/*Get input from user*/
	gettimeofday(&start_time,NULL); 								/*get current time*/

	res = regcomp(&regex, reg_mail, REG_EXTENDED);							/*Compile regular expression*/
	if (res) {
		fprintf(stderr, "Could not compile regex\n");
		return;
	}

	resSub = regcomp(&regex2, reg_domain, REG_EXTENDED);						/*Compile regular expression for domain*/
	if (resSub) {
		fprintf(stderr, "Could not compile resSub regex\n");
		return;
	}

	makeLower(mail);										/*e-mail toLower*/
	mailRes = matchMail(regex, regex2, mail, 1);							/*check if e-mail get from user is valid*/
	if(mailRes == 1) {
	    printf("Email %s insert in table %c.\n", mail, mail[0]);
	}
	else if(mailRes == -1) {
	    printf("Email not valid.\n");
	}
	else if(mailRes == 0) {
	    printf("Email %s insert in table warning.\n", mail);
	}
	else if(mailRes == 2) {
	    printf("Email %s not insert (duplicated).\n", mail);
	}
	regfree(&regex);
	regfree(&regex2);
	executionTime(start_time);									/*get total execution time*/
}




/*Function that deletes single mail from DB*/
void deleteMail() {
	char mail[MAX], statement[MAX];
	printf("\nInsert email to delete: ");
	scanf("%s", mail);										/*get user input*/
	gettimeofday(&start_time,NULL); 								/*get current time*/
	makeLower(mail);										/*e-mail toLower*/
	char firstChar = mail[0];
	snprintf(statement, MAX, "DELETE FROM `%c` WHERE mail= '%s'", firstChar, mail);			/*create delete query*/
	MYSQL* conn;
	conn = connection();										/*connect to db*/
	if (!mysql_query(conn, statement)) {								/*exec delete query*/
		if(mysql_affected_rows(conn) == 1) {							/*check if query deleted one e-mail*/
			printf("Email %s deleted from table %c.\n", mail, firstChar);
		}
		else
			printf("Email %s not found.\n", mail);
	}
	else
	       fprintf(stderr, "%s\n", mysql_error(conn));
	closeConnection(conn);
	executionTime(start_time);									/*get total execution time*/
}


/*Function that gets mails from DB and writes them into txt files with max 500 rows eachone */
void printToFile() {
	gettimeofday(&start_time,NULL); 								/*get current time*/
	MYSQL *conn;
	char table[MAX];
	int i, nFolders = 0, nFiles = 0, nMail = 0;
	mkdir("output/", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);					/*create output directory*/
	printf("\nExporting database to files...\n");
	for (i = 0; i < strlen(SYMBOL_TABLE); i++){
		snprintf(table, MAX, "SELECT `mail` FROM `%c`", SYMBOL_TABLE[i]);			/*query all tables in db*/
		conn = connection();
		if (!mysql_query(conn, table)) {							/*exec select query*/
			 MYSQL_RES* result = mysql_store_result(conn);
			 int n = mysql_num_rows(result);						/*number of rows from db*/
			 if(n > 0) {									/*if there are at least 1 row*/
				MYSQL_ROW row;
				char nameFolder[MAX];
				int counter = 0, nFile = 0;
				snprintf(nameFolder, MAX, "output/%c", SYMBOL_TABLE[i]);		/*set name of file*/
				mkdir(nameFolder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); 		/*create folder for every table*/
				snprintf(nameFolder, MAX, "output/%c/%c_%d.txt", SYMBOL_TABLE[i], SYMBOL_TABLE[i], nFile);
				FILE *output = fopen(nameFolder, "w+");					/*create output file*/
				nFolders++;
				nFiles++;
				nMail = nMail + n;
				while ((row = mysql_fetch_row(result))) {				/*read all rows from db*/
					if(counter == MAX_MAIL) {					/*if file is full, close it and create new file*/
						fclose(output);
						nFile++;						/*change number of file*/
						snprintf(nameFolder, MAX, "output/%c/%c_%d.txt", SYMBOL_TABLE[i], SYMBOL_TABLE[i], nFile);  /*prepare new filename*/
						output = fopen(nameFolder, "w+");			/*create new output file*/
						counter = 0;						/*reset line counter*/
						nFiles++;
					}
					if(counter < (MAX_MAIL - 1) && counter < (n-1-MAX_MAIL*nFile))
					  fprintf(output, "%s,\n", row[0]);
					else
					  fprintf(output, "%s,", row[0]);
					counter++;
				}
				fclose(output);

			}
			mysql_free_result(result);
		}
		else
			fprintf(stderr, "%s\n", mysql_error(conn));
		closeConnection(conn);
	}
	snprintf(table, MAX, "SELECT `mail` FROM `warning`");						/*special case for table warning*/

	conn = connection();
	if (!mysql_query(conn, table)) {
		  MYSQL_RES* result = mysql_store_result(conn);
		  int n = mysql_num_rows(result);
		  if(n > 0) {
			MYSQL_ROW row;
			char nameFolder[MAX];
			snprintf(nameFolder, MAX, "output/warning");
			mkdir(nameFolder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); 			/*create warning folder*/
			int counter = 0, nFile = 0;
			snprintf(nameFolder, MAX, "output/warning/warning_%d.txt", nFile);
			FILE *output = fopen(nameFolder, "w+");						/*create output file*/
			nFolders++;
			nFiles++;
			nMail = nMail + n;
			while ((row = mysql_fetch_row(result))) {					/*read all rows from db*/
				if(counter == MAX_MAIL) {						/*if file is full, close it and create new file*/
					fclose(output);
					nFile++;
					snprintf(nameFolder, MAX, "output/warning/warning_%d.txt", nFile);	/*prepare new filename*/
					output = fopen(nameFolder, "w+");				/*create new output file*/
					counter = 0;							/*reset line counter*/
					nFiles++;
				}
				if(counter < (MAX_MAIL - 1) && counter < (n-1-MAX_MAIL*nFile))
				  fprintf(output, "%s,\n", row[0]);
				else
				  fprintf(output, "%s,", row[0]);
				counter++;
			}
			fclose(output);

		}
		mysql_free_result(result);
	}
	else
		fprintf(stderr, "%s\n", mysql_error(conn));
	printf("Have been exported %d email in %d files in a total of %d folders.\n", nMail, nFiles, nFolders);
	executionTime(start_time);									/*get total execution time*/
}

/* Function that intialize domains table */
void initTableDomains() {
        checkDB();
	MYSQL *conn;
	int i;
	char c;
	char table[MAX], statement[MAX]; 
	char* domain;
	conn = connection();									/*prepare query to create domains table */
	snprintf(statement, MAX, "CREATE TABLE IF NOT EXISTS domains (domain varchar(255) NOT NULL, n_mails int NULL, time varchar(255) NULL, date_ping date NULL, valid int NULL,  PRIMARY KEY (domain))");
	query(conn, statement);
	closeConnection(conn);
	
	conn = connection();	
	snprintf(statement, MAX, "SELECT * FROM domains");				/*query to check if domains table is not empty */
	query(conn, statement);
	MYSQL_RES* result = mysql_store_result(conn);
	int n = mysql_num_rows(result);						/*number of rows from domains table*/
	if(n > 0) {								/*if there is at least one row, answer user if is necessary update domains table */
	      printf("Do you want update domains table before exec ping? (y/n) ");
	      scanf(" %c", &c);
	      if(c != 'y') 
		    return;
	      else {
		    printf("Updating domains table...\n");
		    MYSQL_ROW row;
		    while ((row = mysql_fetch_row(result))) {					/*read all rows from domains table*/
			   MYSQL *conn_d = connection();
			   snprintf(statement, MAX, "UPDATE `domains` SET `n_mails`= 0 WHERE `domain` = '%s'", row[0]);    /*reset number of emails for every domain*/
			   query(conn_d, statement);
			   closeConnection(conn_d);
		    }
	      }
	      mysql_free_result(result);
	}
	closeConnection(conn);	
	
	for (i = 0; i < strlen(SYMBOL_TABLE); i++){
		snprintf(table, MAX, "SELECT `mail` FROM `%c`", SYMBOL_TABLE[i]);			/*query all tables in db*/
		conn = connection();
		if (!mysql_query(conn, table)) {							/*exec select query*/
			 MYSQL_RES* result = mysql_store_result(conn);
			 int n = mysql_num_rows(result);						/*number of rows from db*/
			 if(n > 0) {									/*if there is at least 1 row*/
				printf("Extract domains from table %c\n", SYMBOL_TABLE[i]);
				MYSQL_ROW row;
				while ((row = mysql_fetch_row(result))) {				/*read all rows from db*/
				      domain = strrchr(row[0], '@');					/*extract domain part from email*/
				      domain = strndup(domain + sizeof(char), sizeof(char)*strlen(domain));	/*cut the @*/
				      snprintf(statement, MAX, "INSERT INTO domains (`domain`, `n_mails`, `valid`) VALUES ('%s', '%d', '%d')", domain, 1, 0);	/*Prepare insert query*/
				      MYSQL *conn_d = connection();
				      if(query(conn_d, statement)) {				/*if domain is already in table, update number of mails with this domain*/
					  snprintf(statement, MAX, "UPDATE `domains` SET `n_mails`=`n_mails`+ 1 WHERE `domain` = '%s'", domain);      
					  query(conn_d, statement);
				      }
				      closeConnection(conn_d);
				}
			}
			mysql_free_result(result);
		}
		else
			fprintf(stderr, "%s\n", mysql_error(conn));
		closeConnection(conn);
	}
}

/*Function that gets only mails that have pinged domain and save them into txt files with max 500 rows eachone */
void printDomainsToFile() {
	gettimeofday(&start_time,NULL); 								/*get current time*/
	MYSQL *conn;
	char table[MAX];
	char* domain;
	int i, nFolders = 0, nFiles = 0, nMail = 0;
	mkdir("outputPing/", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);					/*create output directory*/
	printf("\nExporting database to files...\n");

	for (i = 0; i < strlen(SYMBOL_TABLE); i++){
		snprintf(table, MAX, "SELECT `mail` FROM `%c`", SYMBOL_TABLE[i]);			/*query all tables in db*/
		conn = connection();
		if (!mysql_query(conn, table)) {							/*exec select query*/
			 MYSQL_RES* result = mysql_store_result(conn);
			 int n = mysql_num_rows(result);						/*number of rows from db*/
			 if(n > 0) {									/*if there are at least 1 row*/
				MYSQL_ROW row;
				char nameFolder[MAX];
				int counter = 0, nFile = 0;
				snprintf(nameFolder, MAX, "outputPing/%c", SYMBOL_TABLE[i]);		/*set name of file*/
				mkdir(nameFolder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); 		/*create folder for every table*/
				snprintf(nameFolder, MAX, "outputPing/%c/%c_%d.txt", SYMBOL_TABLE[i], SYMBOL_TABLE[i], nFile);
				FILE *output = fopen(nameFolder, "w+");					/*create output file*/
				nFolders++;
				nFiles++;
				while ((row = mysql_fetch_row(result))) {				/*read all rows from db*/
					domain = strrchr(row[0], '@');					/*extract domain part from email*/
				    domain = strndup(domain + sizeof(char), sizeof(char)*strlen(domain));	/*cut the @*/
					MYSQL *conn_domain = connection();
					snprintf(table, MAX, "SELECT `valid` FROM `domains` WHERE `domain` = \"%s\" ", domain);		/*query to check if domain is pinged*/
					if (!mysql_query(conn_domain, table)) {							/*exec select query*/
						MYSQL_RES* result_domain = mysql_store_result(conn_domain);
			 			MYSQL_ROW row_domain = mysql_fetch_row(result_domain);
			 			if(atoi(row_domain[0]) == 0) {				/*check if domain not reply to ping*/
			 				closeConnection(conn_domain);
			 				continue;
			 			}
			 			closeConnection(conn_domain);
			 		}
			 		nMail++;
					if(counter == MAX_MAIL) {					/*if file is full, close it and create new file*/
						fclose(output);
						nFile++;						/*change number of file*/
						snprintf(nameFolder, MAX, "outputPing/%c/%c_%d.txt", SYMBOL_TABLE[i], SYMBOL_TABLE[i], nFile);  /*prepare new filename*/
						output = fopen(nameFolder, "w+");			/*create new output file*/
						counter = 0;						/*reset line counter*/
						nFiles++;
					}
					if(counter < (MAX_MAIL - 1) && counter < (n-1-MAX_MAIL*nFile))
					  fprintf(output, "%s,\n", row[0]);
					else
					  fprintf(output, "%s,", row[0]);
					counter++;
				}
				fclose(output);

			}
			mysql_free_result(result);
		}
		else
			fprintf(stderr, "%s\n", mysql_error(conn));
		closeConnection(conn);
	}

	printf("Have been exported %d email in %d files in a total of %d folders.\n", nMail, nFiles, nFolders);
	executionTime(start_time);									/*get total execution time*/
}


/* Function that makes charapter to lower case*/
void makeLower(char *input)
{
	while (*input != '\0')  /* "while (*input)" would also work */
	{
		*input = tolower(*input);
		input++;
	}
}

/*
This file is part of MailParser package.
Writen by
        - Cantarella Danilo (https://github.com/Flyer-90);
        - Randazzo Filippo (https://github.com/filirnd);
        - Siragusa Sebastiano (https://github.com/sebysira);

Full MailParser is released by GPL3 licence.
*/
