import sys
from random import randint, seed		
seed(123456789)

import pytoulbar2
try:
	n = int(sys.argv[1])
	m = int(sys.argv[2])
except:
	print('Two integer need to be in argument:  n then m')
	exit()
	
top = n*m + 1

Problem = pytoulbar2.CFN(top)
#Create a variable for each square
for i in range(n):
	for j in range(m):
		Problem.AddVariable('sq(' + str(i) + ',' + str(j) + ')', range(n*m))

#create a varaible for maximum of color
Problem.AddVariable('max', range(n*m))
#quaterny hard constraints for rectangle with same color angles
#for each square on the chessboard
for i1 in range(n):
	for i2 in range(m):
		#for every square on the chessboard that could form a rectangle withe first sqare as up left corner and this square as down right corner
		for j1 in range(i1+1, n):
			for j2 in range(i2+1, m):
				Constraints = []
				for k in range(n*m):
					for l in range(n*m):
						for o in range(n*m):
							for p in range(n*m):
								if k ==l and l == o and o == p:
									#if they are all the same color 
									Constraints.append(top)
								else:
									Constraints.append(0)
				Problem.AddFunction(['sq(' + str(i1) + ',' + str(i2) + ')', 'sq(' + str(i1) + ',' + str(j2) + ')', 'sq(' + str(j1) + ',' + str(i2) + ')', 'sq(' + str(j1) + ',' + str(j2) + ')'], Constraints)

#binary hard constraints to fix the variable max as an upper bound
for i in range(n):
	for j in range(m):
		Constraints = []
		for k in range(n*m):
			for l in range(n*m):
				if k>l:
					#if the color of the square is more than the number of the max
					Constraints.append(top)
				else:
					Constraints.append(0)
		Problem.AddFunction(['sq(' + str(i) + ',' + str(j) + ')', 'max'], Constraints)

#minimize the number of colors
Problem.AddFunction(['max'], range(n*m))
#Problem.Dump('BoardColoration.cfn')
res = Problem.Solve(showSolutions =3)

#visual print of the soltuions
if res:
	for i in range(n):
		row = []
		for j in range(m):
			row.append(res[0][m*i+j])
		print(row)
else:
	print('No solutions found')
