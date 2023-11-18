import timeit

a = "H" * 1073741824
s = 0
for i in range(100):
    t = timeit.default_timer()
    'h' in a
    t = timeit.default_timer() - t
    print(t)

    s += t

print(f"avg: {s / 100}")
