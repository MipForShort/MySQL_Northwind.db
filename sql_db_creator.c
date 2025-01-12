/* Includes */
#include <stdio.h> /* For scanf, printf and fprintf */
#include <stdlib.h>
#include <mysql/mysql.h> /* We include mysql.h, you need to have installed mysql and mysql libmysqlclient-dev */
#include <string.h> /* We use strings here */

/* Definition for max query, max buffer and max password */
#define MAX_Q 1024
#define MAX_B 4092
#define MAX_P 25

const char *path_schema = "./Northwind.db.sql"; /* Path to .db.sql script for data, default can be current dir */

/* Variables to connect to MySQL, change them for your credentials */
char *server = "localhost";
char *user = "root";
char password[MAX_P]; /* Max array for a password */

void execute_sql(MYSQL *conn, const char *query);/* Prototype to execute query */

/* Main function */
int main(void)
{
    MYSQL *conn; /* Pointer for connector to MySQL*/
    FILE *file; /* Pointer for script file*/

    printf("Enter root password:\n"); /* We ask for password */
    scanf("%24[^\n]", password); /* This format tells scanf to ignore all blanks after the string */
    
    /* Variable for query, not used in the db creation process */
    /* char query[MAX_Q];  */
    char buffer[MAX_B];

    /* We initialize connector, and if it fails enters if for printing error */
    conn = mysql_init(NULL);

    if (conn == NULL)
    {
       printf("mysql_init() failed\n");
       return EXIT_FAILURE;
    }

    /* Condition to see if connection was correct, if NULL it prints error and closes conn */
    if (mysql_real_connect(conn, server, user, password, NULL, 0, NULL, 0) == NULL)
    {
       printf("mysql_real_connect() failed. Error: %s\n", mysql_error(conn));
       mysql_close(conn);
       return EXIT_FAILURE;
    }

    printf("Connection successful to MySQL Server\n"); /* If we made it through, we print success */

    /* First query to create db if it does not exists*/
    execute_sql(conn, "CREATE DATABASE IF NOT EXISTS Northwind;");
    printf("Database 'Northwind' created or existing\n");

    /* Condition to select Northwind db in MySQL */
    if (mysql_select_db(conn, "Northwind") != 0)
    {
        fprintf(stderr, "Error selecting the database: %s\n", mysql_error(conn));
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    printf("Database 'Northwind' selected\n");

    /************** Opening script file for schema **************/
    file = fopen(path_schema, "r"); /* We open the file schema in read mode */
    if (file == NULL) /* Basic check for NULL error */
    {
        fprintf(stderr, "Error opening script file %s\n", path_schema);
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    printf("File '%s' opened correctly\n", path_schema);

    /* Read all scripts in one buffer */
    size_t size = fread(buffer, 1, sizeof(buffer) - 1, file);
    buffer[size] = '\0'; /* Make sure buffer ends in NULL */

    fclose(file);

    /* Separate ; for querys */
    char *query_start = buffer; /* Variable for the start of query */
    char *query_end; /* Variable for the end of query */
    /* While query_end returns the start of query and ; and if that is not NULL */
    while ((query_end = strstr(query_start, ";")) != NULL)
    {
        *query_end = '\0'; /* End query string */
        /* Execute query */
        execute_sql(conn, query_start);
        query_start = query_end + 1; /* Next block */
    }

    printf("Schema Script loaded successfully\n");


    /* End connection successfully */
    mysql_close(conn);
    return EXIT_SUCCESS;
}


void execute_sql(MYSQL *conn, const char *query)
{
    /* If we receive anything other than 0 from the query then we print error message */
    if (mysql_query(conn, query) != 0)
    {
        fprintf(stderr, "Error executing query %s\n%s\n", query, mysql_error(conn));
        return;
    }
}
