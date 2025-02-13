/***********************************************************
 * 
 * FILE: sql_db_creator.c
 *
 * CONTENT: Program that creates a mysql db named Northwind
 *          and its schema
 * 
 ***********************************************************/

/* Includes */
#include <stdio.h> /* For scanf, printf and fprintf */
#include <stdlib.h> /* I still don't remember what I need this for */
#include <unistd.h> /* Lib that let you use sleep in Unix/Linux */
// #include <windows.h> /* Lib that let you use sleep in Windows */
#include <mysql/mysql.h> /* We include mysql.h, you need to have installed mysql and mysql libmysqlclient-dev */
#include <string.h> /* We use strings here */
#include "sql_db_creator.h" /* Header file that stores the prototypes */


/* Definition for max query, max buffer and max password */
#define MAX_Q 1024
#define MAX_B 4092
#define MAX_P 25

const char *path_schema = "./Northwind.db.sql"; /* Path to .db.sql script for data, default can be current dir */

/* Variables to connect to MySQL, change them for your credentials */
char *server = "localhost";
char *user = "root";
char password[MAX_P]; /* Max array for a password */

/* Main function */
int main(void)
{
    MYSQL *conn; /* Pointer for connector to MySQL */

    clear_terminal();

    /* Process to get password */
    printf("Enter root password:\n"); /* We ask for password */
    scanf("%24[^\n]", password); /* This format tells scanf to ignore all blanks after the string */
    clear_terminal();
    
    /* Variable for query, not used in the db creation process */
    /* char query[MAX_Q]; */

    /* We initialize connector with NULL */
    conn = mysql_init(NULL);
    
    /* Function that checks if we connected succesfully, old way */
    /*check_connection(conn);*/
    
    /* New way to check connection */
    if (check_connection(conn) == EXIT_FAILURE)
    {
        printf("Connection failed.\n");
        return EXIT_FAILURE;
    }

    print_menu(conn);
    mysql_close(conn);
    return EXIT_SUCCESS;

}

/* Funct to exect_sql qrys and store them */
MYSQL_RES *execute_sql(MYSQL *conn, const char *query)
{
    /* If we receive anything other than 0 from the query then we print error message */
    if (mysql_query(conn, query) != 0)
    {
        fprintf(stderr, "\nError executing query %s\n%s\n", query, mysql_error(conn));
        /* Change to return what is stored in conn when executed */
        return mysql_store_result(conn);
    }

    /* We return what is stored in our connector */
    return mysql_store_result(conn);
}

/* Func to check the connection */
int check_connection(MYSQL *conn)
{
    /* We check that the connector is not NULL when it was returned previously */
    if (conn == NULL)
    {
       printf("mysql_init() failed\n");
       return EXIT_FAILURE;
    }

    /* Condition to see if connection was correct, if NULL it prints error and closes conn */
    if (mysql_real_connect(conn, server, user, password, NULL, 0, NULL, 0) == NULL)
    {
       printf("mysql_real_connect() failed.\nError: %s\n", mysql_error(conn));
       mysql_close(conn);
       return EXIT_FAILURE;
    }

    printf("Connection successful to MySQL Server - Northwind.db\n"); /* If we made it through, we print success */
}

/* Func to create the db */
void create_database(MYSQL *conn)
{
    /* We call our execute_sql func with the qry to exec */
    execute_sql(conn, "CREATE DATABASE IF NOT EXISTS Northwind;");
    printf("Database 'Northwind' created or already existing\n");
}

/* Func to select the created db, mysql need this to work with only one db */
int select_database(MYSQL *conn)
{
    /* Condition to select Northwind db in MySQL */
    if (mysql_select_db(conn, "Northwind") != 0)
    {
        fprintf(stderr, "\nError selecting the database: %s\n", mysql_error(conn));
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    printf("Database 'Northwind' selected\n");
}

/* Func that reads and external .db file for the schema */
int read_schema(MYSQL *conn) 
{
    FILE *file; /* Pointer for script file */
    char buffer[MAX_B]; /* Buffer to store whole script */

    /************** Opening script file for schema **************/
    file = fopen(path_schema, "r"); /* We open the file schema in read mode */
    if (file == NULL) /* Basic check for NULL error */
    {
        fprintf(stderr, "\nError opening script file %s\n", path_schema);
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
}

/* Function to describe a database with a specific query */
void describe_database(MYSQL *conn, const char *query)
{
    /* We create a result variable that stores the result of execute_sql */
    MYSQL_RES *result = execute_sql(conn, query);

    /* If the result is NULL then we print error */
    if (result == NULL)
    {
        fprintf(stderr, "\nError getting the result.\n");
    }
    /* Else we get a result */
    else
    {
        /* Apparently we need a row datatype for parsing rows */
        MYSQL_ROW row;
        printf("\nTables in Northwind:\n");
        /* This loop parses every row with mysql_fetch_row until it is NULL */
        while ((row = mysql_fetch_row(result)) != NULL)
        {
            printf("- %s\n", row[0]);  // Print table name
        }
 
        /* We need to free the result */
        mysql_free_result(result);
    }
}

void print_table (MYSQL *conn, char *table)
{
	/* Query to describe the table */
    char describe_query[MAX_Q];
    /* This snprintf will make our describe_query start with DESCRIBE and add what our row says  */
    snprintf(describe_query, sizeof(describe_query), "DESCRIBE %s;", table);

    MYSQL_RES *desc_result = execute_sql(conn, describe_query);

    /* If we get a result from desc_result */
    if (desc_result != NULL)
    { 
        MYSQL_ROW desc_row;
        printf("  Columns:\n");
        while((desc_row = mysql_fetch_row(desc_result)) != NULL)
        {
            printf("    - %s (%s)\n", desc_row[0], desc_row[1]); /* This prints name of field and its data type */
        }
        mysql_free_result(desc_result);
    }
    else
    {
        fprintf(stderr, "\n  Error describing table %s\n", table);
    }

}

int print_menu (MYSQL *conn)
{
    int menu_option;

    do
    {
        printf("\nMenu:\n1.- Create Northwind db / Select it.\n2.- Execute SQL Script for Northwind Schema.\n3.- Describe Northwind db.\n4.- Describe a table.\n5.- Query SELECT * on table.\n6.- Clear terminal.\n7.- Exit.\n");

        scanf("%d", &menu_option);

        switch(menu_option)
        {
            /* Case to create-use Northwind db  */
            case 1:
                /* Function to create the db */
                create_database(conn);
                /* Function to select the db in mysql */
                select_database(conn);
                pause_for_a_while(2);
                clear_terminal();
                print_menu(conn);
                break;
            /* Case to execute the create schema based script */    
            case 2:
                /* Function to read the db schema from another .db file */
                read_schema(conn);
                pause_for_a_while(2);
                clear_terminal();
                print_menu(conn);
                break;
            /* Case to describe Northwind db */
            case 3:
				clear_terminal();
                /* Function to describe the db, this just calls execute_sql with the qry as second parameter */
                describe_database(conn, "SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = 'Northwind';");
                print_menu(conn);
                break;
            /* Case to describe a table from the db */
            case 4:
				/* Here we describe db to select table to read records from */
                describe_database(conn, "SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = 'Northwind';");
				char table[MAX_P]; /* Variable for table name */
				printf("Table to select data from:\n");
                scanf("%s", table);
				/* Function that describes information about the table */
				print_table(conn, table);
                print_menu(conn);
                break;
            /* Case for a SELECT * FROM table */
			case 5:
                /* Here we describe db to select table to read records from */
                describe_database(conn, "SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = 'Northwind';");
                //char table[MAX_P]; /* Variable for table name */
                printf("Table to select data from:\n");
                scanf("%s", table);
                clear_terminal();
				/* Function to read SELECT query */
                read_records(conn, table);
                print_menu(conn);
				break;
            /* Case for cleaning terminal */
            case 6:
                clear_terminal();
                print_menu(conn);
                break;
            /* Case to exit from file */
            case 7:
                /* End connection successfully */
                mysql_close(conn);
                clear_terminal();
                return EXIT_SUCCESS;
                break;
        }
    }
    while (menu_option < 1 || menu_option > 6);
}

void clear_terminal()
{

    #ifdef _WIN32
        system("cls");  // For Windows
    #else
        system("clear");  // For Linux or macOS
    #endif
}

void pause_for_a_while(int seconds) 
{
    #ifdef _WIN32
        Sleep(seconds * 1000);  // For Windows, in miliseconds
    #else
        sleep(seconds);  // For Linux/macOS
    #endif
}

void read_records(MYSQL *conn, char *table) 
{
    char query[MAX_Q];
    snprintf(query, sizeof(query), "SELECT * FROM %s;", table);
    MYSQL_RES *result = execute_sql(conn, query);

    if (result != NULL) 
    {
        int num_fields = mysql_num_fields(result);
        MYSQL_FIELD *fields = mysql_fetch_fields(result);
        
        /* Find the maximun on each column */
        int max_lengths[num_fields];
        for (int i = 0; i < num_fields; i++) 
        {
            max_lengths[i] = strlen(fields[i].name);  /* Initialize with the fields name */
        }

		/* We travel to each row to find the longest field on each field */
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result)) != NULL) 
        {
            for (int i = 0; i < num_fields; i++) 
            {
                if (row[i] != NULL) 
                {
                    int len = strlen(row[i]);
                    if (len > max_lengths[i]) 
                    {
                        max_lengths[i] = len;
                    }
                }
            }
        }

        /* Print the name of each column aligned */
        for (int i = 0; i < num_fields; i++) 
        {
            printf("%-*s  ", max_lengths[i], fields[i].name);  /* Allign to the left for the max length of field name plus two blank spaces */
        }
        printf("\n");

		/* Back to the beginning to print registries */
        mysql_data_seek(result, 0);  /* Restablish pointer back to first regirter */
        while ((row = mysql_fetch_row(result)) != NULL) 
        {
            for (int i = 0; i < num_fields; i++) 
            {
                if (row[i] != NULL) 
                {
                    printf("%-*s  ", max_lengths[i], row[i]);  /* Print aligned */
                } 
                else 
                {
                    printf("%-*s  ", max_lengths[i], "NULL");  /* Manage NULL */
                }
            }
            printf("\n");
        }

        mysql_free_result(result);
    }
}
