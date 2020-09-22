# ResistanceCalc
This program helps calculate what 2 resistors to use when trying to make another value.

The available resistor values are:
1.0, 1.2, 1.5, 1.8, 2.2, 2.7, 3.3, 3.9, 4.7, 5.6, 6.8, 8.2
x1, x10, x100, x1,000, x10,000, x100,000, x1,000,000

How to use...
1) Run the program

2) Enter a desired value 
(examples: 1234, 754.3, 1000)

3) Returned from the program will be
[Connection details]  [R1 value] [R2 value] [Calculated value] [Error %]

4) Based on what resistors that you have, make your new value :)


The results have been kept within 2% of the value, which should be fine for most applications

Example session:

    *** Calculator for 2 resistor pair ***

    Enter desired value: 754.3


    Results within 2% of value...
    Parallel:       1k0     3k3     767.441833      1.7%
    Parallel:       1k5     1k5     750.000000      0.6%
    Series:         180R    560R    740.000000      1.9%
    Series:         270R    470R    740.000000      1.9%
    Series:         68R     680R    748.000000      0.8%
    Parallel:       820R    8k2     745.454529      1.2%

    Press enter to continue...

From this it can be seen that 2 1k5 resistors in parallel will give the closest value, but if I didn't have them 68R and 680R in series would also be within 1%.
