from AIList import AIList
import numpy as np


i = AIList()
i.add(10, 11)
i.add(15, 19)
i.add(11, 13)
i.add(19, 22)
i.add(20, 25)
i.add(30, 100)
i.add(30, 95)
i.display()

w = i.wps()
print(w)

print("Merging")
m = i.merge()
m.display()

print("Filtering")
f = i.filter(3, 10)
f.display()

len(i)
print("Intersecting")
o = i.intersect(3,15)
o.display()

print("Length distribution")
i.display()
ld = i.length_dist()
print(ld)

for x in i:
    print(x)