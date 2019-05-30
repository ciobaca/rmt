#!/opt/local/bin/python3
import time
import subprocess
import sys

exes = [ "ldelexmain.exe", "ldecompmain.exe", "ldegraphmain.exe", "slopesV7i.exe" ]

filename = sys.argv[1]
label = sys.argv[2] # "fig:comparison1"
title = sys.argv[3] # "Comparison of LDEGraph and Slopes."

print("""
\\begin{figure}
\\begin{center}
\\begin{tabular}{c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c}
\hline
Coefficients & Lex & Comp & Graph & Slopes & Result \\\\
\hline""")
with open(filename, "r") as f:
    lines = f.readlines()
    for line in lines:
        if not line.strip():
          continue
        rt, sols = [], []
        mxVal = max(*map(int, line.strip().split()))
        for index, exe in enumerate(exes):
            if index < 1 and mxVal > 25:
              rt.append('> 60')
              continue
            times = []
            for _ in range(10):
                start = time.perf_counter()
                res = subprocess.Popen("echo %s | %s 1>temp.txt" % (line.strip(), exe), shell=True)
                if res.wait() != 0:
                  open('temp.txt', 'w').write('-1')
                end = time.perf_counter()
                times.append(end - start)
                sols.append(int(open('temp.txt', 'r').readline()))
                sys.stderr.write("%s %s %d %.4lf\n" % (line.strip(), exe, sols[-1], times[-1]))
                sys.stderr.flush()
            times.sort()
            rt.append("%.3lf" % (sum(times[2:-2]) / 6))
        if len(set(sols)) != 1:
            print("%s & %s & %s & %s & %s* & %d \\\\" % (line[:-1], rt[0], rt[1], rt[2], rt[3], max(sols)))
        else:
          print("%s & %s & %s & %s & %s & %d \\\\" % (line[:-1], rt[0], rt[1], rt[2], rt[3], sols[0]))
print("""
\hline
\end{tabular}
\caption{\label{%s}%s}
\end{center}
\end{figure}""" % (label, title))