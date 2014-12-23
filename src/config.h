/*
This file is part of MailParser package.
Writen by
        - Danilo Cantarella (https://github.com/Flyer-90);
        - Sebastiano Siragusa (https://github.com/sebysira);
        - Filippo Randazzo (https://github.com/filirnd);

Full MailParser is released by GPL3 licence.
*/


#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include <mysql.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdbool.h>

#define N 100
#define CHARGAP 48
#define MAX 256
#define MAX_MAIL 500
#define MAX_LENGHT 1024
#define LENGHT_MAIL 40
#define LENGHT_DOMAIN 25
#define reg_mail "[a-zA-Z0-9!#$%&'*+/=?^_`{|}~-]+(\\.[a-zA-Z0-9!#$%&'*+/=?^_`{|}~-]+)*@([a-zA-Z0-9-]([a-zA-Z0-9-]*[a-z0-9-])?\\.)+[a-z0-9]([a-z0-9]*[a-z0-9])?"
#define reg_domain "(com|net|org|edu|biz|gov|mil|info|name|me|tv|us|mob|academy|ad|ae|ag|ai|al|am|ar|at|au|aw|ba|be|bg|bh|bm|bn|bo|br|bs|bw|by|bz|ca|ch|ci|cl|cn|co|company|cr|cu|cy|cz|de|dk|dm|do|ec|ee|eg|es|et|fi|fj|fm|fo|fr|gb|ge|gi|gl|gr|gt|gu|gy|hk|hn|hr|hu|id|ie|il|in|ir|is|it|jm|jo|jp|ke|kg|kr|kw|kz|lb|lc|li|lk|lt|lu|lv|ma|mc|md|mk|mn|mo|mt|mu|museum|mv|mx|my|na|nc|ng|ni|nl|no|np|nu|nz|om|pa|pe|pf|pg|ph|pk|pl|pt|py|qa|ro|ru|sa|se|sg|si|sk|su|sv|th|tn|tr|tt|tw|tz|ua|ug|uk|us|uy|uz|va|ve|vi|vn|ws|ye|yu|za|zm|zw)"


/* config you mysql database */

#define server "localhost"
#define user "your_username"
#define password "your_password"


/*
This file is part of MailParser package.
Writen by
        - Danilo Cantarella (https://github.com/Flyer-90);
        - Sebastiano Siragusa (https://github.com/sebysira);
        - Filippo Randazzo (https://github.com/filirnd);

Full MailParser is released by GPL3 licence.
*/
