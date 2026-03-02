

a = 10
b = 20

def foo(b):
    print(b)

o: object = object()


class glass:
    def __init__(self):
        self.g = 200
    def grint(self):
        print(self.__dict__)

g: glass = glass()
g.grint()

n = "n-global"

def outer():
    n = "n-outer"
    def inner():
        global n
        #n = "n-inner"
        print(f"{n=}")

    inner()

outer()

g: str = None

tum = object()
print(tum.__str__().__str__().__str__().__str__().__str__())
