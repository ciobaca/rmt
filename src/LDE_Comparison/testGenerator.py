from random import randint


def hardcodedTests():
  print "3 4\n1 2 3\n7 5 6 4"
  print "3 4\n1 7 4\n2 5 6 3"
  print "2 5\n107 57\n77 101 46 55 63"
  print "2 5\n14 107\n89 95 93 22 31"


def genTests(n, m, mxVal, cntTests=10):
  while cntTests > 0:
    cntTests -= 1
    print n, m
    print ' '.join([str(mxVal)] + [str(randint(1, mxVal)) for _ in range(n - 1)])
    print ' '.join(str(randint(1, mxVal)) for _ in range(m))


if __name__ == '__main__':
  hardcodedTests()
  nms = [(1, 2), (1, 3), (1, 4), (1, 5), (1, 6), (1, 7), (1, 8), (1, 9)]
  nms += [(2, 2), (2, 3), (2, 4), (2, 5), (2, 6), (2, 7), (2, 8)]
  nms += [(3, 3), (3, 4), (3, 5), (3, 6), (4, 4), (4, 5)]
  mxVals = [2, 3, 5, 13, 29, 39, 107, 503, 1021]
  for n, m in nms:
    for mxVal in mxVals:
      if n * m > 8 and mxVal > 500:
        continue
      if n * m > 10 and mxVal > 150:
        continue
      if n * m > 16 and mxVal > 50:
        break
      genTests(n, m, mxVal)