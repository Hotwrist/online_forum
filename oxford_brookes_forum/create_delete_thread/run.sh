# Igbe Chijioke
# This bash script compiles the index.cpp in this folder to index.cgi
g++ -Wall -I/usr/include/cppconn -o create_thread.cgi create_thread.cpp ../../is_user_loggedIn/check_online.cpp -lcrypto -L/usr/lib -lmysqlcppconn  -lcgicc
