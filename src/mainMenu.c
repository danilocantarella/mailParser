/*
This file is part of MailParser package.
Writen by
        - Danilo Cantarella (https://github.com/Flyer-90);
        - Sebastiano Siragusa (https://github.com/sebysira);
        - Filippo Randazzo (https://github.com/filirnd);

Full MailParser is released by GPL3 licence.
*/


#include "config.h"
#include "parser.h"

/* gcc -o mailParser $(mysql_config --cflags) mainMenu.c $(mysql_config --libs)      compile with it*/

/* Main function of project that implements Menu choise and functions call */
int main() {

	char choice;

	printf( "___  ___      _ _______\n"
		"|  \\/  |     (_) | ___ \\    \n"
		"| .  . | __ _ _| | |_/ /_ _ _ __ ___  ___ _ __ \n"
		"| |\\/| |/ _` | | |  __/ _` | '__/ __|/ _ \\ '__|\n"
		"| |  | | (_| | | | | | (_| | |  \\__ \\  __/ |   \n"
		"\\_|  |_/\\__,_|_|_\\_|  \\__,_|_|  |___/\\___|_| \n");


    initDB(); 				/*create database and tables*/
	do{
		printf("\n _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ \n"
		         "|            Menu             |\n"
			 "|                             |\n"
		         "|  1. Insert from directory   |\n"
			 "|  2. Insert email manually   |\n"
			 "|  3. Insert from file        |\n"
			 "|  4. Search email            |\n"
			 "|  5. Delete email            |\n"
			 "|  6. Delete from file        |\n"
			 "|  7. Export db               |\n"
			 "|  8. Ping domains            |\n"
			 "|  0. Exit                    |\n"
			 "|_ _ _ _ _ _ _ _ _ _ _ _ _ _ _|\n"
		         );
		printf("\nInsert your choice: ");
		scanf("%s", &choice);

		switch (choice - CHARGAP) {

		case 0: printf("Exiting...\n");		/*Exit from program*/
			break;

		case 1: processDirectory();		/*Process directory given from user*/
			break;

		case 2: insertMail();			/*Insert 1 e-mail in database*/
			break;

		case 3: insertFromFile();      /*Insert mails from only file*/
			break;

		case 4: searchMail();			/*Search e-mails on database*/
			break;

		case 5: deleteMail();			/*Delete 1 e-mail from database*/
			break;

		case 6: deleteFromFile();     /* Delete mails from only file*/
			break;

		case 7: printToFile();			/*Export database on files (max 500 e-mails for file)*/
			break;

		case 8: pingDomains();			/**/
			break;

		default:printf("Choice error!\n");	/*Wrong input from user*/
			break;
		}
	} while (choice - CHARGAP != 0); /* Rescale choise to number  */

	exit(EXIT_SUCCESS);
}


/*
This file is part of MailParser package.
Writen by
        - Danilo Cantarella (https://github.com/Flyer-90);
        - Sebastiano Siragusa (https://github.com/sebysira);
        - Filippo Randazzo (https://github.com/filirnd);

Full MailParser is released by GPL3 licence.
*/
