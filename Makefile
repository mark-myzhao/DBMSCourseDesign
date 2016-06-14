SRC=src/b_node.cpp src/b_tree.cpp src/block_file.cpp \
src/count.cpp src/LItem.cpp src/main.cpp src/medrank.cpp 

OBJS=$(SRC:.cpp=.o)
DEBUG=-g
CXX=g++ -std=c++11 -g

all: $(OBJS)
	$(CXX) $(DEBUG) -o medrank $(OBJS)

b_node.o: src/b_node.h src/b_node.cpp
	$(CXX) $(DEBUG) b_node.cpp

b_tree.o: src/b_tree.h src/b_tree.cpp
	$(CXX) $(DEBUG) b_tree.cpp

block_file.o: src/block_file.h src/block_file.cpp 
	$(CXX) $(DEBUG) block_file.cpp

LItem.o: src/LItem.h src/LItem.cpp
	$(CXX) $(DEBUG) LItem.cpp

count.o: src/count.cpp
	$(CXX) $(DEBUG) count.cpp

medrank.o: src/medrank.h src/medrank.cpp
	$(CXX) $(DEBUG) medrank.cpp

main.o: src/main.cpp
	$(CXX) $(DEBUG) main.cpp

clean:
	-rm $(OBJS) medrank *.medrank
