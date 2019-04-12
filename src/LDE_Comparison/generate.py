#!/opt/local/bin/python3
import subprocess

res = subprocess.Popen("py -3 comparison.py test4.txt \"fig:comparison1\" \"Comparison of Lexicographic, Completion, Graph and Slopes algorithms on the four examples in paper~\\cite{filgueiras1995fast}.\" > table1.tex", shell=True)
if res.wait() != 0:
  print("Cam Trist1")
res = subprocess.Popen("py -3 comparison.py test8.txt \"fig:comparison2\" \"Comparison of Lexicographic, Completion, Graph and Slopes algorithms on the eight examples in paper~\\cite{clausen1989efficient}.\" > table2.tex", shell=True)
if res.wait() != 0:
  print("Cam Trist2")
res = subprocess.Popen("py -3 stressTest.py \"fig:comparison3\" \"Comparison of Graph and Slopes.\" > table3.tex", shell=True)
if res.wait() != 0:
  print("Cam Trist3")