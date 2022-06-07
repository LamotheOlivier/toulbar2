import sys

import pytoulbar2

N = int(sys.argv[1])
	
top = N**2 + 1

Problem = pytoulbar2.CFN(top)
#Create a variable for each square
for i in range(N):
	Problem.AddVariable('X' + str(i), range(N**2))


#binary soft constraints for overlapping sqaures
for i in range(N):
	for j in range(i+1, N):
		Problem.AddVariable('X' + str(i) + '-X' + str(j), range(N**2))
		Constraint = []
		for k in range(N**2):
			for l in range(N**2):
				for m in range(N**2):
					if l-k == m:
						Constraint.append(0)
					else:
						Constraint.append(top)
		Problem.AddFunction(['X' + str(i), 'X' + str(j), 'X' + str(i) + '-X' + str(j)], Constraint)

Problem.AddAllDifferent(['X' + str(i) + '-X' + str(j) for i in range(N) for j in range(i+1,N)])
Problem.AddFunction(['X' + str(N-1)], range(N**2))
#Problem.Dump('SquareSoft.cfn')
res = Problem.Solve(showSolutions =3)

