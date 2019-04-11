#!/opt/local/bin/python3
import os

# os.system("python3 comparison.py test4.txt \"fig:comparison1\" \"Comparison of LDEGraph and Slopes on the four examples in paper~\\cite{filgueiras1995fast}.\" > table1.tex")

# os.system("python3 comparison.py test8.txt \"fig:comparison2\" \"Comparison of LDEGraph and Slopes on the eight examples in paper~\\cite{clausen1989efficient}.\" > table2.tex")

os.system("python3 stressTest.py \"fig:comparison3\" \"Comparison of LDEGraph and Slopes.\" > table3.tex")
