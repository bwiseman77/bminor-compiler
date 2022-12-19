# Bminor Compiler
This was a project in CSE 40243 at the University of Notre Dame taken under Douglas Thain (http://compilerbook.org). Given the specifcations of a C-like language (Bminor) we wrote the stages of a compiler: scanner, parser, printer, typechecker, and codegenerator. We used tools like flex to parse tokens for the scanner, and bison to write the grammer for the parser. By the end we were able to generate X86 assembly which could then be linked with the standard C library to be turned into an executable.

To make and run different flags to run stages of compiler:
```
> make
> ./bminor -h
Usage: ./bminor [options] $FILE

Options:
   -scan       File to scan
   -parse      File to parse
   -print      File to print
   -resolve    File to resolve
   -typecheck  File to typecheck
   -codegen    File to codegen, followed by filename of new assembly
   -h          Print help message
```

There are still some improvments to be made:
- fix small issues from class
- more error messages
- non-global arrays
- non-int arrays
- non-1D arrays
- constant folding
- better string functionality 

# Language Overview
## Whitespace and Comments
In B-minor, whitespace is any combination of the following characters: tabs, spaces, linefeed (\n), and carriage return (\r). The placement of whitespace is not significant in B-minor. Both C-style and C++-style comments are valid in B-minor:

```
/* A C-style comment */
a=5; // A C++ style comment
```

## Identifiers
Identifiers (i.e. variable and function names) may contain letters, numbers, and underscores. Identifiers must begin with a letter or an underscore. These are examples of valid identifiers:
```
i x mystr fog123 BigLongName55
```
The following strings are B-minor keywords and may not be used as identifiers:
```
array auto boolean char else false for function if integer print return string true void while
```

## Types
B-minor has four atomic types: integers, booleans, characters, and strings. A variable is declared as a name followed by a colon, then a type and an optional initializer. For example:

```
x: integer;
y: integer = 123;
b: boolean = false;
c: char    = 'q';
s: string  = "hello bminor\n";
```
An integer is always a signed 64 bit value. boolean can take the literal values true or false. char is a single 8-bit ASCII character. string is a double-quoted constant string that is null-terminated and cannot be modified.

Both char and string may contain the following backslash codes. \n indicates a linefeed (ASCII value 10), \0 indicates a null (ASCII value zero), and a backslash followed by anything else indicates exactly the following character. Both strings and identifiers may be up to 255 characters long, not including the null terminator.

B-minor also supports arrays of a fixed size. They may be declared with no value, which causes them to contain all zeros:
```
a: array [5] integer;
```
Or, the entire array may be given specific values:
```
a: array [5] integer = {1,2,3,4,5};
```
A variable of type auto indicates an automatic type which is to be inferred by the value given on the right hand side. For example, in the following code, a is of type integer, b is of type string, and c is of type boolean:
```
a: auto = 10;
b: auto = "hello";
c: auto = a < 100;
```

## Expressions
B-minor has many of the arithmetic operators found in C, with the same meaning and level of precedence:
| Symbol |	Meaning |
| ------ | -------- |
| () [] f()	| grouping, array subscript, function call |
| ++ --	| postfix increment, decrement |
| - !	| unary negation, logical not |
| ^	| exponentiation |
| * / %	| multiplication, division, remainder |
| + -	| addition, subtraction |
| < <= > >= == !=	| comparison |
| &&	| logical and |
| \|\|	| logical or |
| = ?:	| assignment, ternary |

B-minor is strictly typed. This means that you may only assign a value to a variable (or function parameter) if the types match exactly. You cannot perform many of the fast-and-loose conversions found in C.

Following are examples of some (but not all) type errors:
```
x: integer = 65;
y: char = 'A';
if(x>y) ... // error: x and y are of different types!

f: integer = 0;
if(f) ...      // error: f is not a boolean!

writechar: function void ( c: char );
a: integer = 65;
writechar(a);  // error: a is not a char!

b: array [2] boolean = {true,false};
x: integer = 0;
x = b[0];      // error: x is not a boolean!
```
Following are some (but not all) examples of correct type assignments:

```
b: boolean;
x: integer = 3;
y: integer = 5;
b = x<y;     // ok: the expression x<y is boolean

f: integer = 0;
if(f==0) ...    // ok: f==0 is a boolean expression

c: char = 'a';
if(c=='a') ...  // ok: c and 'a' are both chars
```

## Declarations and Statements
In B-minor, you may declare global variables with optional constant initializers, function prototypes, and function definitions. Within functions, you may declare local variables (including arrays) with optional initialization expressions. Scoping rules are identical to C. Function definitions may not be nested.

Within functions, basic statements may be arithmetic expressions, return statements, print statements, if and if-else statements, for loops, or code within inner { } groups:

```
// An arithmetic expression statement.
y = m*x + b;

// A return statement.
return (f-32)*5/9;

// An if-else statement.
if( temp>100 ) {
    print "It's really hot!\n";
} else if( temp>70 ) {
    print "It's pretty warm.\n";
} else {
    print "It's not too bad.\n";
}

// A for loop statement.
for( i=0; i<100; i++ ) {
    print i;0
}
```
B-minor does not have switch statements, while-loops, or do-while loops. (But you could consider adding them as a little extra project.)

The print statement is a little unusual because it is a statement and not a function call like printf is in C. print takes a list of expressions separated by commas, and prints each out to the console, like this:
```
print "The temperature is: ", temp, " degrees\n";
```

## Functions
Functions are declared in the same way as variables, except giving a type of function followed by the return type, arguments, and code:
```
square: function integer ( x: integer ) = {
	return x^2;
}
```
The return type must be one of the four atomic types, or void to indicate no type. Function arguments may be of any type. integer, boolean, and char arguments are passed by value, while string and array arguments are passed by reference. As in C, arrays passed by reference have an indeterminate size, and so the length is typically passed as an extra argument:

```
printarray: function void ( a: array [] integer, size: integer ) = {
	i: integer;
	for( i=0;i<size;i++) {
		print a[i], "\n";
	}
}
```
A function prototype may be given, which states the existence and type of the function, but includes no code. This must be done if the user wishes to call an external function linked by another library. For example, to invoke the C function puts:
```
puts: function void ( s: string );

main: function integer () = {
	puts("hello world");
}
```
A complete program must have a main function that returns an integer. the arguments to main may either be empty, or use argc and argv with the same meaning as in C:
```
main: function integer ( argc: integer, argv: array [] string ) = {
        puts("hello world");
}
```

