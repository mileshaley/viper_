

def main() -> None:

    def foo(a, b, c = 2, *args, i, j = 3, k, **kwargs):
        print(f"{a=}, {b=}, {c=}, {args=}, {i=}, {j=}, {k=}, {kwargs=}\n{'-'*10}\n")
    
    foo(1, 4, k='hello', i=10) # okay
    #foo(1, 4, j=15, k='hello', i=10, j=15) # not okay (kwarg j duplicated)

    foo(1, c=3, b=4, j=15, k='hello', i=10) # okay (even through b named after c)

    #foo(b=4, a=32, "world", j=15, k='hello', i=10) # not okay (positional c after any keyword)

    foo(1, 2, 3, 4, 5, x=12, bar = 12, j=15, k='hello', i=10) # okay

    foo(1, c=3, x='tra', j=15, b=4, k='hello', i=10) # okay




if __name__ == "__main__":
    main()
