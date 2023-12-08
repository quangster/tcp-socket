CXX = g++

all: TCP_Server TCP_Client

TCP_Client: TCP_Client.cpp
	$(CXX) -o TCP_Client TCP_Client.cpp 

TCP_Server: TCP_Server.cpp
	$(CXX) -o TCP_Server TCP_Server.cpp 

clean:
	rm -rf TCP_Client TCP_Server