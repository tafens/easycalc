# EasyCalc

History

EasyCalc started its life as computer science project in the Year of the Lord 1998 during a course in compiler theory at MdH (Mälardalens Högskola, now Mälardalen University). It started out as nothing more than a parser for simple arithmetic expressions, but grew into a utility that I use to this day thanks to its features tailored for my own use cases. Development hasn't been active for a long time, but it has seen quite a lot of use over the years. Development was started on the Unix systems in school and continued on AmigaOS, later under Cygwin/Windows and Linux, and in later years also has been compiled on MacOS. Finally, at long last, almost 25 years later, it sees public release on GitHub. Perhaps someone out there will find it an learn how to do something, or indeed, learn how NOT to do something; either way, I'm happy.

Now what is it?

EasyCalc is a command line calculator that can handle expressions with addition, subtraction, multiplication, division, negation, modulus, power of, faculty, comparisons (equal to, not equal to, larger, lesser, larger or equal to, lesser or equal to), logical operators (and, or, not), parantheses, variables, mathematical functions and macros. The expression is written in the ususal way (a+b-c and the like). Several expressions can be chained together with a semicolon and the result value will be equal to the last expression in the chain.
Program flow can be controlled by the statements while(expr,statements), if(expression,true-statements,false-statements), break, return and quit.
EasyCalc can also handle different number bases: decimal (with scientific notation), hexadecimal, octal, binary, and as an experiment, base "pi". It can also handle input in hours:minutes:seconds (each component in any supported base/notation, including decimals).

How to use it?

EasyCalc is made to be (at least somewhat) self-documenting, just give the option "-h" on the command line or type "!help" at the prompt after invoking it without parameters. Other than that, just type your mathematical expression at the prompt to get your result.

Enjoy!
/Stefan
