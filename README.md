# FindFriendBot
    FindFriendBot is a find friend robot, you can add it by using tok/tok client, it's convenient to find friend by a few commands.
    
#### Commands List
    
    /help: Get a list of commands
    /start: Authorize the bot to recommend you to others
    /get: Get friends from Bot
    /set: Set up "what are you into"
    /info: Print your current information
    /stop: Let the bot stop recommending you to others
    /readme: Read more informations about FinfFriendsBot
    
#### TOK for Tox
    We built a new client of Tox for android and iOS. It is a better fit for FindFriendBot with a better experience.
    Tok website: https://www.tok.life
    
#### How to build?
    Support mac and linux, We work on centos7 (3.10.0-327.el7.x86_64) and mac (mojave 10.14.4) well.
    The project depend on third library, such as toxcore, boost, tcmalloc, mysql-connector-c++;

###### install libsodium
```
$git clone https://github.com/jedisct1/libsodium.git
$cd libsodium && git checkout 1.0.16
$./autogen.sh && ./configure
$make && make install
```

or install from yum(centos):
```
yum install epel-release -y
yum install libsodium -y
yum install libsodium-devel.x86_64 -y
```

    
###### install toxcore
```
$git clone https://github.com/InsightIM/c-toxcore.git
$cd c-toxcore && checkout dev
$mkdir build && cd build
$cmake .. && make -j && make install
```
    More detail about install toxcore: https://github.com/InsightIM/c-toxcore/blob/master/INSTALL.md

###### install boost
```
$wget https://dl.bintray.com/boostorg/release/1.69.0/source/boost_1_69_0.tar.gz
$tar zxvf boost_1_69_0.tar.gz &&cd boost-1.69.0
$./bootstrap.sh     
$./b2 install
```

###### install mysql-connector-c++
```
$wget https://cdn.mysql.com//Downloads/Connector-C++/mysql-connector-c++-8.0.13-linux-glibc2.12-x86-64bit.tar.gz
$tar zxvf mysql-connector-c++-8.0.13-linux-glibc2.12-x86-64bit.tar.gz
$ln -s /source/mysql-connector-c++-8.0.13-linux-glibc2.12-x86-64bit/ /usr/local/mysql-connector-c++
``` 

###### install gperftools
```
$git clone https://github.com/gperftools/gperftools.git
$cd gperftools && git checkout gperftools-2.7
$./autogen.sh
$./configure && make && make install
```

###### install rapidjson 
```
$git clone https://github.com/Tencent/rapidjson.git
$cd rapidjson && mkdir build && cd build
$cmake .. && make && make install
```

###### build FindFriendBot
```
$git clone https://github.com/InsightIM/FindFriendBot.git
$cd FindFriendBot
$mkdir build && cd build
$cmake .. && make -j
```

#### Run FindFriendBot
	Before you run, you should install mysql and load script from doc/db_tok.sql, and then run
```
$./FindFriendBot --file=../conf/config.ini
```

    Have fun!


    
  
    
