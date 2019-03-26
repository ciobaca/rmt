#!/opt/local/bin/python3
import time
import os
import sys

exes = [ "./ldegraphmain", "./slopesV7i" ]

filename = sys.argv[1]
label = sys.argv[2] # "fig:comparison1"
title = sys.argv[3] # "Comparison of LDEGraph and Slopes."

print("""
\\begin{figure}
\\begin{center}
\\begin{tabular}{c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c@{\hspace{0.5cm}}c}
\hline
Coefficients & LDEGraph & Slopes & Result \\\\
\hline""")

with open(filename, "r") as f:
    lines = f.readlines()
    for line in lines:
        if line != "":
            rt = []
            sols = []
            for exe in exes:
#                print("echo \"%s\" | %s 1>/dev/null 2>/dev/null" % (line[:-1], exe))
                times = []
                for repeat in range(10):
                    start = time.process_time()
                    os.system("echo \"%s\" | %s 1>temp.txt 2>/dev/null" % (line[:-1], exe))
                    end = time.process_time()
                    times.append(end - start)
                    with open("temp.txt", "r") as g:
                        sols.append(int(g.readline()))
                times.sort()
                rt.append(sum(times[2:-2]) / 6)
            if len(set(sols)) != 1:
                print("ERROR in solution sizes.")
            print("%s & %.4lf & %.4lf & %d \\\\" % (line[:-1], rt[0], rt[1], sols[0]))
print("""
  \hline
\end{tabular}
\caption{\label{%s}%s}
  \end{center}
\end{figure}""" % (label, title))

