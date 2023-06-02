# Igbe Chijioke
# This bash script compiles the index.cpp in this folder to index.cgi
g++ -Wall -I/usr/include/cppconn -o create_post.cgi create_post.cpp ../../is_user_loggedIn/check_online.cpp -lcrypto -L/usr/lib -lmysqlcppconn  -lcgicc
