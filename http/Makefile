
DIR_INC = ./
DIR_SRC = ./
DIR_OBJ = ./obj
DIR_BIN = ./
DIR_LINK = ./link

CC = gcc
CXX = g++
CFLAGS = -g -D_REENTRANT -Wall -std=c++11  -I${DIR_INC}
LDFLAGS = -lpthread -lmysqlclient -lpython2.7 

#wildcard 对 c文件进行展开
SRCS = $(wildcard ${DIR_SRC}/*.c) $(wildcard ${DIR_SRC}/*.cpp) 
#去除路径信息
dir=$(notdir $(SRCS))
# 把后缀.c .cpp替换成 .o
# 
OBJS =  $(patsubst %.c, ${DIR_OBJ}/%.o, $(patsubst %.cpp, ${DIR_OBJ}/%.o,$(dir))  )


TAR_SERVER = server
TAR_CLIENT = client

BIN_TAR_SERVER = ${DIR_BIN}/${TAR_SERVER}
BIN_TAR_CLIENT = ${DIR_BIN}/${TAR_CLIENT}

BIN_TARGET = ${DIR_BIN}/${TAR_SERVER} ${DIR_BIN}/${TAR_CLIENT}



TARGET = ${BIN_TAR_SERVER}

all: $(TARGET)

${BIN_TAR_SERVER} : ${DIR_OBJ}/server.o ${OBJS} ${DIR_LINK}/*.o
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS) 
#${BIN_TAR_CLIENT} :  ${DIR_OBJ}/client.o $(filter-out ${DIR_OBJ}/server.o,$(OBJS))
#	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS) 



${DIR_OBJ}/%.o : ${DIR_SRC}/%.c
	$(CC) -c $(CFLAGS)  $^ -o $@
${DIR_OBJ}/%.o : ${DIR_SRC}/%.cpp
	$(CXX) -c $(CFLAGS) $^ -o $@    

.PHONY : clean

clean :
	rm -f ${OBJS}
	rm -f ${TARGET}
	@echo "clean done"

# install:
#         mv Excute excute; cp -f ordermisd ../bin/;



