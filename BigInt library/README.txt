Big_Int is a class that implements arithmetic operations on big integers.
The digits are stored in a vector<int>, I preferred this container since it can contain up to 1073741823 elements (I got this information from cplusplus.com),
therefore with this implementation we are able to store numbers up to 2^(1073741823) - 1, fair enough.
This class only works with integers, so numbers with decimal part are not considered.
The binomial coefficient has been implemented through dynamic programming paradigm, also another recursive version has been coded, but it is really slow.
The factorial method works definitely good, it is able to print out the correct result for any kind of crazy number.

In order to compile the program, type "make" inside your command shell.
In order to execute the program, type "./main" inside your command shell.
In order to delete all the object files, type "make clean" inside your command shell.
