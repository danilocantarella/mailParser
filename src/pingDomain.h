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

void pingDomains() {
    gettimeofday(&start_time,NULL); 	
    initTableDomains();
    pingTableDomains();
    executionTime(start_time);
}

void pingTableDomains() {
        MYSQL *conn;
	int n;
	char table[MAX];
	char statement[MAX];
	char* res_ping;
	char* time;
        snprintf(table, MAX, "SELECT `domain` FROM `domains`");						

	conn = connection();
	if (!mysql_query(conn, table)) {
		  MYSQL_RES* result = mysql_store_result(conn);
		  n = mysql_num_rows(result);
		  if(n > 0) {
			MYSQL_ROW row;
			while ((row = mysql_fetch_row(result))) {					/*read all rows from db*/
			      res_ping = ping(row[0]);
			      if(strcmp(res_ping, "Error") == 0) {
				   MYSQL *conn_d = connection();
			           snprintf(statement, MAX, "UPDATE `domains` SET `time` = NULL, `date_ping`= CURDATE(), `valid` = '%d' WHERE `domain` = '%s'", 0, row[0]);      
			           query(conn_d, statement);
			           closeConnection(conn_d);
				   printf("ping %s not possible.\n", row[0]); 
				   continue;
			      }
				 
			      time = strrchr(res_ping, '=');	
			      time = strndup(time + sizeof(char), sizeof(char)*strlen(time));
			      MYSQL *conn_d = connection();
			      snprintf(statement, MAX, "UPDATE `domains` SET `time`= '%s', `date_ping`= CURDATE(), `valid` = '%d' WHERE `domain` = '%s'", time, 1, row[0]);      
			      query(conn_d, statement);
			      closeConnection(conn_d);
			      printf("ping %s with success.\n", row[0]); 
			}
			

		}
		mysql_free_result(result);
	}
	else
		fprintf(stderr, "%s\n", mysql_error(conn));
	printf("Have been ping a total of %d domains.\n", n);
}

char* ping(char* domain) {
    FILE *pf;
    char data[DATA_SIZE];
    char command[MAX];
    char* result = malloc(MAX * sizeof(char));
    result[0] = '\0';
    int i = 0;
    sprintf(command, "ping -c 1 -w 5 %s", domain);
    pf = popen(command,"r");  // Setup our pipe for reading and execute our command.
    if(!pf){
      fprintf(stderr, "Could not open pipe for output.\n");
      return result;
    }
 

  while (fgets(data, sizeof(data)-1, pf) != NULL) {  /* Read the output a line at a time - output it. */
    //printf("-- %s", data);
    if(i == 1)  
      strcat(result, data);	
    i++;
  }
    
    if (pclose(pf) != 0)
        return "Error";
        //fprintf(stderr," Error: Failed to close command stream \n");
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
