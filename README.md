# mahjong
# # 服务器端
g++  ./sleep/sleep.cpp ./people/people.cpp ./my_socket/my_socket.cpp fangjian/fangjian.cpp ./mahjong/mahjong.cpp  server.cpp  -std=c++11  -lpthread  -o server

# # 客户端
g++  client.cpp -o client
