viper_
=====
```cpp
"viper"_ = "An attempt to replicate Python's syntax in C++"; 
```
----
## About viper_:
The goal of is to leverage as many C++ language features as possible to replicate the **clean syntax** and **dynamic typing** of python.

This project isn't designed to be used seriously but instead as a fun demonstration of some of the useful, interesting, and obscure features C++ has to offer.

`viper_.h` is the single include header file that requires no setup but can be configured with the preprocessor.

## Features:
##### Variables can be declared without explicit types, named with a literal and an underscore:
```cpp
"my_variable"_ = 100;
// Also supports other literals:
123_ = "Hello, World!";
3.14_ = 123_; // 3.14 = "Hello, World!"
```
##### Variables can be easily printed with `print()`:
```cpp
print("{123} This is a format print so my_variable = {my_variable}. Cool, right?");
```
> Hello, World! This is a format print so my_variable = 100. Cool, right?

##### Variables can optionally be given a type hint for type safety with `_(Type)`:
```cpp
"hinted"_ _(int) = 100; // Okay
"bad_hint"_ _(int) = "100"; // Throws a type error
```

## Comparison:

##### viper_:

```cpp
int main() {
    "my_variable"_ = 100;
    // Also supports other literals:
    123_ = "Hello, World!";
    3.14_ = 123_; // 3.14 = "Hello, World!"
    print("{123} This is a format print so my_variable = {my_variable}. Cool, right?");
    "hinted"_ _(int) = 100; // Okay
    "bad_hint"_ _(int) = "100"; // Throws a type error
}
```

##### Python:
```py
my_variable = 100
# Doesn't support variable names beginning with numbers or containing '.'
var123 = "Hello, World!"
var3_14 = var123 # var3_14 = "Hello, World!"
print(f"{var123} This is a format print so my_variable = {my_variable}. Cool, right?")
hinted : int = 100 # Okay
bad_hint : int = "100" # No error but could be caught by a static analyzer
```
