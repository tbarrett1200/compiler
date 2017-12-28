# compiler

Input:
```
var a
var b
var c

function doSomething {
    return a + b * c
}

function main {
    a = 2
    b = 3
    c = 5
    return 2 * doSomething
}
```

Output:
```
target triple = "x86_64-apple-macosx10.12.0"
@a = global i32 0
@b = global i32 0
@c = global i32 0

define i32 @doSomething () {
entry:
	%0 = load i32, i32* @a
	%1 = load i32, i32* @b
	%2 = load i32, i32* @c
	%3 = mul i32 %1, %2
	%4 = add i32 %0, %3
	ret i32 %4
}

define i32 @main () {
entry:
	store i32 2, i32* @a
	store i32 3, i32* @b
	store i32 5, i32* @c
	%0 = call i32 @doSomething()
	%1 = mul i32 2, %0
	ret i32 %1
}

```
