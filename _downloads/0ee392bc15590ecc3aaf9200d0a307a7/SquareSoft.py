import sys
from random import randint, seed		
seed(123456789)

import pytoulbar2
try:
	N = int(sys.argv[1])
	S = int(sys.argv[2])
	assert N <= S
except:
	print('Two integer need to be in argument:  N then S')
	exit()
	
top = N**4 + 1

Problem = pytoulbar2.CFN(top)
#Create a variable for each square
for i in range(N):
	Problem.AddVariable('sq' + str(i+1), ['(' + str(l) + ',' + str(j) + ')' for l in range(S-i) for j in range(S-i)])


#binary soft constraints for overlapping sqaures
for i in range(N):
	for j in range(i+1,N):
		ListConstraintsOverlaps = []
		for a in [S*k+l for k in range(S-i) for l in range(S-i)]:
			for b in [S*m+n for m in range(S-j) for n in range(S-j)]:
				#Calculating the coodonate of the squares
				X_i = a%S
				X_j = b%S
				Y_i = a//S
				Y_j = b//S
				#Calculating if squares are overlapping
				if X_i >= X_j :
					if X_i - X_j < j+1:
						if Y_i >= Y_j:
							if Y_i - Y_j < j+1:
								ListConstraintsOverlaps.append(min(j+1-(X_i - X_j),i+1)*min(j+1-(Y_i - Y_j),i+1))
							else:
								ListConstraintsOverlaps.append(0)
						else:
							if Y_j - Y_i < i+1:
								ListConstraintsOverlaps.append(min(j+1-(X_i - X_j),i+1)*min(i+1-(Y_j - Y_i),j+1))
							else:
								ListConstraintsOverlaps.append(0)
					else:
						ListConstraintsOverlaps.append(0)
				else :
					if X_j - X_i < i+1:
						if Y_i >= Y_j:
							if Y_i - Y_j < j+1:
								ListConstraintsOverlaps.append(min(i+1-(X_j - X_i),j+1)*min(j+1-(Y_i - Y_j),i+1))
							else:
								ListConstraintsOverlaps.append(0)
						else:
							if Y_j - Y_i < i+1:
								ListConstraintsOverlaps.append(min(i+1-(X_j - X_i),j+1)*min(i+1-(Y_j - Y_i),j+1))
							else:
								ListConstraintsOverlaps.append(0)
					else:
						ListConstraintsOverlaps.append(0)
		Problem.AddFunction(['sq' + str(i+1), 'sq' + str(j+1)], ListConstraintsOverlaps)

#Problem.Dump('SquareSoft.cfn')
res = Problem.Solve(showSolutions =3)

#visual print of the soltuions
if res:
	for i in range(S):
		row = []
		for j in range(S):
			row.append(' 0')
			for k in range(N-1, -1, -1):
				if (res[0][k]%(S-k) <= j and j - res[0][k]%(S-k) <= k) and (res[0][k]//(S-k) <= i and i - res[0][k]//(S-k) <= k):
					row[-1] = f'{k+1:2}'
		print(row)
else:
	print('No solutions found')
