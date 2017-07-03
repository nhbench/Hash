###############################################################
# Program:
#     Week 12, Hash
#     Brother JonesL, CS235
# Author:
#     Nathan Bench, Jed Bilman, Jeremy Chandler, Justin Chandler
# Summary:
#     An implementation of hash tables with a rudimentary spell checker
# Time:
#     Nathan: 6 hours
#     Jed:
#     Jeremy:
#     Justin: 
###############################################################

##############################################################
# The main rule
##############################################################
a.out: week12.o spellCheck.o
	g++ -o a.out week12.o spellCheck.o -g
	tar -cf week12.tar *.h *.cpp makefile

##############################################################
# The individual components
#      week12.o     : the driver program
#      spellCheck.o   : the spell-check program and driver
##############################################################
week12.o: hash.h week12.cpp bst.h bnode.h stack.h
	g++ -c week12.cpp -g

spellCheck.o: hash.h spellCheck.h spellCheck.cpp
	g++ -c spellCheck.cpp -g

