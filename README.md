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
#### Variables can be declared without explicit types, named with a literal and an underscore:
```cpp
"my_variable"_ = 100;
// Also supports other literals:
123_ = "Hello, World!";
3.14_ = 123_; // 3.14 = "Hello, World!"
```
#### Variables can be easily printed with `print()`:
```cpp
print("{123} This is a format print so my_variable = {my_variable}. Cool, right?");
```
> Hello, World! This is a format print so my_variable = 100...
#### Variables can optionally be given a type hint for type safety with `_(Type)`:
```cpp
"var"_ _(int) = 100;
```

## Comparison:
<table>
    <tr><td> viper_ </td> <td> Python </td></tr>
    <tr>
        <td style="padding: 0;"> 

```cpp
"var"_ _(int) = 100;
```

</td>
        <td style="padding: 0;">

```py
var : int = 100
```

</td>
    </tr>
    <tr>
<td> 400 </td>
<td>

**Markdown** _here_. (↕︎ Blank lines above and below!)

</td></tr>
</table>
