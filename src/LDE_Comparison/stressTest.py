#!/opt/local/bin/python3
import time
import os
import sys
from random import randint

exes = [ "./ldegraphmain", "./slopesV7i" ]

def genTest(n, m, mxVal):
  return ' '.join(str(randint(1, mxVal)) for _ in range(n)) + ' 0 ' + ' '.join([str(mxVal)] + [str(randint(1, mxVal)) for _ in range(m - 1)]) + ' 0'

label = sys.argv[1] # "fig:comparison1"
title = sys.argv[2] # "Comparison of LDEGraph and Slopes."

nms = []
nms += [(2, 2), (2, 3), (2, 4)]

#nms += [(1, 2), (1, 3), (1, 4), (1, 5), (1, 6), (1, 7), (1, 8), (1, 9)]
#nms += [(2, 2), (2, 3), (2, 4), (2, 5), (2, 6), (2, 7), (2, 8)]
#nms += [(3, 3), (3, 4), (3, 5), (3, 6), (4, 4), (4, 5)]

#mxVals = [2, 3, 5, 13, 29, 39, 107, 503, 1021]

mxVals = [29, 39, 107]

print("""
\\begin{figure}
\\begin{center}
\\begin{tabular}{c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c}
\hline
& A & 2 & 3 & 5 & 13 & 29 & 39 & 107 & 503 & 1021 \\\\
N & M \\\\
\hline""")


for n, m in nms:
  if n == m:
    print("\\hline")
  toPrint = "%d & %d" % (n, m)
  for mxVal in mxVals:
    if n * m > 8 and mxVal > 500 or n * m > 10 and mxVal > 150 or n * m > 16 and mxVal > 50:
      toPrint += " & N:N"
      continue
    graph, slopes = 0, 0
    for __ in range(10):
      test = genTest(n, m, mxVal)
      rt, sols = [], []
      for exe in exes:
          times = []
          for __ in range(10):
              start = time.perf_counter()
              os.system("echo %s | %s 1>temp.txt" % (test, exe))
              end = time.perf_counter()
              times.append(end - start)
              sols.append(int(open('temp.txt', 'r').readline()))
          times.sort()
          rt.append(sum(times[2:-2]) / 6)
      if len(set(sols)) != 1:
          print("ERROR in solution sizes.")
          print(sols)
#      print("Graph: %.4lf    Slopes: %.4lf" % (rt[0], rt[1]))
      if abs(rt[0] - rt[1]) < 1e-2:
        pass
#        graph, slopes = graph + 1, slopes + 1
      elif rt[0] < rt[1]:
        graph += 1
      else:
        slopes += 1
    toPrint += " & %d:%d" % (graph, slopes)
  print(toPrint, '\\\\')

print("""
\hline
\end{tabular}
\caption{\label{%s}%s}
\end{center}
\end{figure}""" % (label, title))

