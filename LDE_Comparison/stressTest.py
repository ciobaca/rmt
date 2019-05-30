#!/opt/local/bin/python3
import time
import os
import subprocess
import sys
from random import randint, seed

exes = ["ldegraphmain.exe", "slopesV7i.exe"]

def genTest(n, m, mxVal):
  return ' '.join(map(str, sorted(randint(1, mxVal) for _ in range(n)))) + ' 0 ' + ' '.join(map(str, sorted([mxVal] + [randint(1, mxVal) for _ in range(m - 1)])[::-1])) + ' 0'

label = sys.argv[1] # "fig:comparison1"
title = sys.argv[2] # "Comparison of LDEGraph and Slopes."

nms = [(1, 2), (1, 3), (1, 4), (1, 5), (1, 6), (1, 7), (1, 8), (1, 9)]
nms += [(2, 2), (2, 3), (2, 4), (2, 5), (2, 6), (2, 7), (2, 8)]
nms += [(3, 3), (3, 4), (3, 5), (3, 6), (4, 4), (4, 5)]
mxVals = [2, 3, 5, 13, 29, 39, 107, 503, 1021]

print("""
\\begin{figure}
\\begin{center}
\\begin{tabular}{c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c}
\hline
& A & 2 & 3 & 5 & 13 & 29 & 39 & 107 & 503 & 1021 \\\\
N & M \\\\
\hline""")

winG, winS, cntClasses = 0, 0, 0
for n, m in nms:
  if n == m:
    print("\\hline")
  toPrint = "%d & %d" % (n, m)
  for mxVal in mxVals:
    if mxVal > 500 and ((m > 4 and n > 1) or (n == 1 and m > 7)) or mxVal > 1000 and n > 1 and m > 4:
      toPrint += " & "
      continue
    graph, slopes, stars = 0, 0, 0
    for __ in range(10):
      test = genTest(n, m, mxVal)
      rt, sols = [], []
      for exe in exes:
          times = []
          for __ in range(5):
              start = time.perf_counter()
              res = subprocess.Popen("echo %s | %s 1>temp.txt" % (test, exe), shell=True)
              if res.wait() != 0:
                open('temp.txt', 'w').write('-1')
              end = time.perf_counter()
              times.append(end - start)
              sols.append(int(open('temp.txt', 'r').readline()))
              sys.stderr.write("%s %s %d %.4lf\n" % (test, exe, sols[-1], times[-1]))
              sys.stderr.flush()
              if times[-1] > 15 or sols[-1] == -1:
                break
          if times[-1] > 15:
            rt.append(sum(times) / len(times))
          else:
            times.sort()
            rt.append(sum(times[1:-1]) / 3)
      if abs(rt[0] - rt[1]) < 1e-6:
        pass
      elif rt[0] < rt[1]:
        graph += 1
      else:
        slopes += 1
      stars += len(set(sols)) != 1
    toPrint += (" & %d:%d" % (graph, slopes)) + ('*' * stars) + ' '
    cntClasses += 1
    if graph > 7:
      winG += 1
    if slopes > 7:
      winS += 1
  print(toPrint, '\\\\')

print("""
\hline
\end{tabular}
\caption{\label{%s}%s}
\end{center}
\end{figure}""" % (label, title))
print('%% %d %d / %d' % (winG, winS, cntClasses))