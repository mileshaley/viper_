

def main() -> None:

    def foo(a, b, c = 2, *args, i, j = 3, k, **kwargs):
        print(f"{a=}, {b=}, {c=}, {args=}, {i=}, {j=}, {k=}, {kwargs=}\n{'-'*10}\n")
    
    foo(1, 4, k='hello', i=10) # okay
    #foo(1, 4, j=15, k='hello', i=10, j=15) # not okay (kwarg j duplicated)

    foo(1, c=3, b=4, j=15, k='hello', i=10) # okay (even through b named after c)

    #foo(b=4, a=32, "world", j=15, k='hello', i=10) # not okay (positional c after any keyword)

    foo(1, 2, 3, 4, 5, x=12, bar = 12, j=15, k='hello', i=10) # okay

    foo(1, c=3, x='tra', j=15, b=4, k='hello', i=10) # okay

    print("--"*20)

    
    def bar(a, b, c, d):
        print(f"{a=}, {b=}, {c=}, {d=} \n{'-'*10}\n")
    bar(d=4,a=1,c=3,b=2) # okay

    #bar(a=1,b=2,c=3) # not okay (missing argument without default)
    #bar(a=1,b=2,c=3, 4) # not okay (positional d after any keyword)
    bar(1,b=2,c=3, d=4) # okay


    try:
        bar(1, 2)
    except Exception as e:
        print(f"oops: {e}")

    def bar(a, b, c, d, *e, w, x=2, y):
        print(f"{a=}, {b=}, {c=}, {d=}, {e=}, {w=}, {x=}, {y=} \n{'-'*10}\n")

    print("--"*20)
    a = 2
    print(a)

    a = 'hi'
    print(a)

    a = ["one", "two", "three"]
    print(a.__len__())
    #a = None

    #print(a.pop(-4))
    #print(a.pop(-1))

    print("plung ", a.index("two", 2, 1))

    print("--"*20)

    def baz(a, b, c, d, *e, w, x=2, y):
        print(f"{a=}, {b=}, {c=}, {d=}, {e=}, {w=}, {x=}, {y=} \n{'-'*10}\n")


if __name__ == "__main__":
    main()
