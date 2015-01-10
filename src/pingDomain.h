/*
This file is part of MailParser package.
Writen by
        - Danilo Cantarella (https://github.com/Flyer-90);
        - Sebastiano Siragusa (https://github.com/sebysira);
        - Filippo Randazzo (https://github.com/filirnd);

Full MailParser is released by GPL3 licence.
*/


#include "config.h"
#include "db.h"

void pingDomains();
void pingTableDomains();
char* ping(char*);

/*Function that manage ping operations */
void pingDomains() {
    gettimeofday(&start_time,NULL);
    initTableDomains();
    pingTableDomains();
    executionTime(start_time);
}

/*Function that ping every domain in domains table */
void pingTableDomains() {
	MYSQL *conn;
	int n;
	char table[MAX], statement[MAX];
	char* res_ping;
	char* time;
        snprintf(table, MAX, "SELECT `domain` FROM `domains`");						/*query to read all domains*/

	conn = connection();
	if (!mysql_query(conn, table)) {
		  MYSQL_RES* result = mysql_store_result(conn);
		  n = mysql_num_rows(result);								/*number of domains*/
		  if(n > 0) {										/*if there is at least one domain*/
			MYSQL_ROW row;
			while ((row = mysql_fetch_row(result))) {					/*read all rows from domains table*/
			      printf("ping: %s... ", row[0]);
			      res_ping = ping(row[0]);							/*ping domain and store the result*/
			      if(strcmp(res_ping, "Error") == 0) {					/*if ping failed*/
				   MYSQL *conn_d = connection();
			           snprintf(statement, MAX, "UPDATE `domains` SET `time` = NULL, `date_ping`= CURDATE(), `valid` = '%d' WHERE `domain` = '%s'", 0, row[0]);
			           query(conn_d, statement);						/*update row domain setting date of ping and valid = 0*/
			           closeConnection(conn_d);
				   printf("failed.\n");
				   continue;
			      }

			      time = strrchr(res_ping, '=');						/*extract time ping from result*/
			      time = strndup(time + sizeof(char), sizeof(char)*7);			/*cut = and whitespace*/
			      MYSQL *conn_d = connection();
			      snprintf(statement, MAX, "UPDATE `domains` SET `time`= '%s', `date_ping`= CURDATE(), `valid` = '%d' WHERE `domain` = '%s'", time, 1, row[0]);
			      query(conn_d, statement);							/*update row domain setting time, date of ping and valid = 1*/
			      closeConnection(conn_d);
			      printf("success with time = %s\n", strtok(time, "\n"));
			}
		}
		mysql_free_result(result);
	}
	else
		fprintf(stderr, "%s\n", mysql_error(conn));
	printf("Have been ping a total of %d domains.\n", n);
}

/*Function that execute ping*/
char* ping(char* domain) {
    FILE *pf;
    int i = 0;
    char data[DATA_SIZE], command[MAX];
    char* result = malloc(MAX * sizeof(char));								/*string to store ping result*/
    result[0] = '\0';

    sprintf(command, "ping -c 1 -w 5 %s", domain);
    pf = popen(command,"r");  										/*setup pipe for reading and execute command*/
    if(!pf){
         fprintf(stderr, "Could not open pipe for output.\n");
         return result;
    }

    while (fgets(data, sizeof(data)-1, pf) != NULL) {  							/*read the output a line at a time */
	 if(i == 1)  											/*if second line of result*/
	    strcat(result, data);									/*store line in result string*/
	 i++;
    }

    if (pclose(pf) != 0)
        return "Error";											/*return error string if ping failed*/
    return result;
}

/*
This file is part of MailParser package.
Writen by
        - Danilo Cantarella (https://github.com/Flyer-90);
        - Sebastiano Siragusa (https://github.com/sebysira);
        - Filippo Randazzo (https://github.com/filirnd);

Full MailParser is released by GPL3 licence.
*/
