#ifndef SQL_DB_CREATOR_H
#define SQL_DB_CREATOR_H

#include <mysql/mysql.h> /* We include mysql.h, you need to have installed mysql and mysql libmysqlclient-dev */

/* Function prototypes */
MYSQL_RES *execute_sql(MYSQL *conn, const char *query); /* Prototype to execute query */
int check_connection(MYSQL *conn); /* Prototype to check connection */
void create_database(MYSQL *conn); /* Prototype to create the database */
int select_database(MYSQL *conn); /* Prototype to select the database in mysql */
int read_schema(MYSQL *conn); /* Prototype to read schema from a .db file */
void describe_database(MYSQL *conn, const char *query); /* Prototype to describe the db */
int print_menu(MYSQL *conn); /* Prototype to print the menu */
void clear_terminal(); /* Prototype to clear window */
void pause_for_a_while(int seconds); /* Prototype to wait n amount of seconds */
void read_records(MYSQL *conn, char *table);  /* Prototype to SELECT * FROM 'TABLE' */

#endif
