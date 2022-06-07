import sys
from random import seed, randint
seed(123456789)

import pytoulbar2

N = int(sys.argv[1])

top = N**2 +1

Problem = pytoulbar2.CFN(top)

for i in range(N):
    Problem.AddVariable('Q' + str(i+1), ['row' + str(a+1) for a in range(N)])
    
for i in range(N):
	for j in range(i+1,N):
	    	
		#Two queen cannot be on the same row constraints
		ListConstraintsRow = []
		for a in range(N):
			for b in range(N):
				if a != b :
					ListConstraintsRow.append(0)
				else:
				 	ListConstraintsRow.append(top)
		Problem.AddFunction([i, j], ListConstraintsRow)
		
		#Two queens cannot be on the same upper diagonal constraints
		ListConstraintsUpperD = []
		for a in range(N):
			for b in range(N):
				if a + i != b + j :
					ListConstraintsUpperD.append(0)
				else:
				 	ListConstraintsUpperD.append(top)
		Problem.AddFunction([i, j], ListConstraintsUpperD)
		
		#Two queens cannot be on the same lower diagonal constraints
		ListConstraintsLowerD = []
		for a in range(N):
			for b in range(N):
				if a - i != b - j :
					ListConstraintsLowerD.append(0)
				else:
				 	ListConstraintsLowerD.append(top)
		Problem.AddFunction([i, j], ListConstraintsLowerD)

# random unary costs
for i in range(N):
	ListConstraintsUnaryC = []
	for j in range(N):
		ListConstraintsUnaryC.append(randint(1,N))
	Problem.AddFunction([i], ListConstraintsUnaryC)


#Problem.Dump('WeightQueen.cfn')
res = Problem.Solve(showSolutions = 3)
if res:
	for i in range(N):
		row = [' ' for j in range(N)]
		row[res[0][i]] = 'X'
		print(row)
