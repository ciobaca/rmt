from random import randint


def hardcodedTests():
  print "3 4\n1 2 3\n7 5 6 4"
  print "3 4\n1 7 4\n2 5 6 3"
  print "2 5\n107 57\n77 101 46 55 63"
  print "2 5\n14 107\n89 95 93 22 31"


def 


for _ in range(100):
  n, m = randint(5, 15), randint(5, 15)
  print n, m
  print ' '.join(str(randint(1, 15)) for __ in range(n))
  print ' '.join(str(randint(1, 15)) for __ in range(m))