# The Guild
A small terminal-based RPG (kind of) game. You play as a guild manager that must send adventurers on missions to earn money and not starve. There is no win condition, but you can lose by having 100 gold coins (currency ; how original, I know) of debt.
IMPORTANT : Only works on Linux/WSL (tested on debian and ubuntu).
## Installation
Go to your favorite git repo folder (or wherever you want really) and run:
```shell
$ git clone https://github.com/IkWane/the-guild.git
```
You also need to install ncurses:
```shell
$ sudo apt install libncurses-dev
```
Note: ```libncurses-dev``` is the package name on apt for debian/ubuntu. If you have a different distribution, the name might also be different.
## Usage
Go to the project root and run:
```shell
$ ./build
```
To build the project, and when you want to play run:
```shell
$ ./run
```
