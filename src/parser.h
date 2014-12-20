/*
This file is part of MailParser package.
Writen by
        - Danilo Cantarella (https://github.com/Flyer-90);
        - Sebastiano Siragusa (https://github.com/sebysira);
        - Filippo Randazzo (https://github.com/filirnd);

Full MailParser is released by GPL3 licence.
*/

#include "config.h"
#include "fileManager.h"

int matchMail(regex_t, regex_t, char*, int);

/*Function to check if a char* is a valid e-mail*/
int matchMail(regex_t regex, regex_t regex2, char* token, int mode) {			/*mode = 0 for insert email, mode = 1 for delete email*/
	MYSQL* conn;									/*Connection to db*/
	int res, resSub, lenght;
	bool warning = false;
	size_t begin, end;								/*Store match's indices*/
	regmatch_t matches[1];								/*Used from regex for match's indices*/
	char *last, *nameMail;								/*Store domain part and local part of e-mail*/
	char statement[MAX], msgbuf[N]; 						/*Store query and errors*/

	res = regexec(&regex, token, 1, matches, 0);					/*Execute regular expression*/
	if (!res) {									/*If there is match*/
		lenght = strlen(token);							/*Match's lenght*/
		begin = matches[0].rm_so;						/*Match's initial index*/
		end = matches[0].rm_eo;							/*Match's final index */
		token = strndup(token + begin, end);					/*Take only the match*/

		last = strrchr(token, '.');						/*take token after last . in the match*/
		last = strndup(last + sizeof(char), sizeof(char)*strlen(last));		/*cut the . from last*/

		resSub = regexec(&regex2, last, 1, matches, 0);				/*Execute regular expression to match only domain part*/

		if (!resSub) {								/*If there is match*/
			lenght = strlen(last);						/*Match's lenght*/
			begin = matches[0].rm_so;					/*Match's initial index*/
			end = matches[0].rm_eo;						/*Match's final index */
			last = strndup(last + begin, end);				/*Take only the match*/

			if (end - begin == lenght){					/*If match if correct (example: .come in place of .com)*/
				char first = *token;					/*Take first char of token*/
				if(mode == 1)
				  snprintf(statement, MAX, "INSERT INTO `%c` VALUES ('%s')", first, token);	/*Prepare insert query*/
				else
				  snprintf(statement, MAX, "DELETE FROM `%c` WHERE mail= '%s'", first, token);
				if (strlen(token) >= LENGHT_MAIL){				/*Check for long e-mail (it may be warning)*/
					char copyToken[strlen(token)];			/*Temp char to store a copy of token*/
					strcpy(copyToken, token);			/*Copy token to not modify it*/
					nameMail = strtok(copyToken, "@");		/*Split token for @*/
					lenght = strlen(nameMail);			/*E-mail's local part lenght*/
					if (lenght >= LENGHT_DOMAIN) {				/*If long local part -> Warning*/
						if(mode == 1)
						  snprintf(statement, MAX, "INSERT INTO warning VALUES ('%s')", token);	/*Prepare insert query in table warning*/
						else
						  snprintf(statement, MAX, "DELETE FROM warning WHERE mail= '%s'", token);
						warning = true;				/*Actual e-mail is a warning*/
					}
				}

				conn = connection();					/*Connect to db*/
				if(!query(conn, statement)) {				/*Exec query*/
				    closeConnection(conn);
				    if(warning)
				          return 0;					/*Return 0 if e-mail is warning*/
				    return 1;						/*Return 1 for normal valid e-mail*/
				}
				else {
				  closeConnection(conn);
				  return 2;						/*Return 2 if duplicate e-mail*/
				}
			}
			else
			      return -1;						/*Return -1 if not valid e-mail*/

		}
		else if (resSub == REG_NOMATCH) {					/*No match*/
			return -1; 							/*Return -1 if not valid e-mail*/
		}
		else {
			regerror(resSub, &regex2, msgbuf, sizeof(msgbuf));		/*Error on exec regex*/
			return -2; 							/*Return -2 for error on regexec*/
		}
	}
	else if (res == REG_NOMATCH) {
		return -1; 								/*Return -1 if not valid e-mail*/
	}
	else {
		regerror(res, &regex, msgbuf, sizeof(msgbuf));				/*Error on exec regex for domain part*/
		return -2;								/*Return -2 for error on regexec*/
	}
	return -2;
}

/*
This file is part of MailParser package.
Writen by
        - Danilo Cantarella (https://github.com/Flyer-90);
        - Sebastiano Siragusa (https://github.com/sebysira);
        - Filippo Randazzo (https://github.com/filirnd);

Full MailParser is released by GPL3 licence.
*/

