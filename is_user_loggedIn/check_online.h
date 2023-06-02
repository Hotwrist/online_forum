/* Copyright(c) 2023 Igbe Chijioke
*  All Rights Reserved
*/

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>

#include <iomanip>
#include <string>
#include <cstdlib>

#include <iostream>
#include <stdio.h>
#include <sstream>

// The MySQL server is hosted locally. To connect to it,
// it uses 127.0.0.1 and port 3306 
#define DB_HOST_URL "tcp://127.0.0.1:3306"


using namespace std;
using namespace sql;

bool is_online(string username);
