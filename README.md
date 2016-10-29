bearparser
==========

Minimalistic Disasembler<br/>
WARNING: this is an early beta version, some elements are unfinished!<br/>
Please report any bugs and remarks to: hasherezade@op.pl<br/>

Requires:
+ Qt4 Core<br/>
+ Qt4 Gui<br/>
+ bearparser 
+ udis86
+ cmake http://www.cmake.org/<br/>

To build it, you must clone to main directory:
+ bearpaser (https://github.com/hasherezade/bearparser.git)
+ udis86 (version with CMake files added: https://github.com/hasherezade/udis86.git)


udis86 must be pre-prepared by generating itab.c/.h:
udis86:
./autogen.sh
./configure
make

autogen.sh requires: autoreconf
apt-get install dh-autoreconf

