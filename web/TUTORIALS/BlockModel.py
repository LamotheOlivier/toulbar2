import sys
import pytoulbar2

Lines = open(sys.argv[1], 'r').readlines()
N = len(Lines)
Matrix = [[int(e) for e in l.split(' ')] for l in Lines]
Top = 1 + N*N

K = int(sys.argv[2])

Var = [(chr(65 + i) if N < 28 else "x" + str(i)) for i in range(N)] # Political actor or any instance
#    Var = ["ron","tom","frank","boyd","tim","john","jeff","jay","sandy","jerry","darrin","ben","arnie"] # Transatlantic
#    Var = ["justin","harry","whit","brian","paul","ian","mike","jim","dan","ray","cliff","mason","roy"] # Sharpstone
#    Var  = ["Sherrif","CivilDef","Coroner","Attorney","HighwayP","ParksRes","GameFish","KansasDOT","ArmyCorps","ArmyReserve","CrableAmb","FrankCoAmb","LeeRescue","Shawney","BurlPolice","LyndPolice","RedCross","TopekaFD","CarbFD","TopekaRBW"] # Kansas

Problem = pytoulbar2.CFN(Top)


#create a variable for each coeficient of the matrix
for u in range(K):
	for v in range(K):
		Problem.AddVariable("M_" + str(u) + "_" + str(v), range(2))


#Create a variable for each node
for i in range(N):
	Problem.AddVariable(Var[i], range(K))


#general case for cost function
for u in range(K):
	for v in range(K):
		for i in range(N):
			for j in range(N):
				if i != j:
					ListCost = []
					for m in range(2):
						for k in range(K):
							for l in range(K):
								if (u == k and v == l and Matrix[i][j] != m):
									ListCost.append(1)
								else:
									ListCost.append(0)
					Problem.AddFunction(["M_" + str(u) + "_" + str(v), Var[i], Var[j]],ListCost)

# self-loops
for u in range(K):
	for i in range(N):
		ListCost = []
		for m in range(2):
			for k in range(K):
				if (u == k and Matrix[i][i] != m):
					ListCost.append(1)
				else:
					ListCost.append(0)
		Problem.AddFunction(["M_" + str(u) + "_" + str(u), Var[i]], ListCost)

# breaking partial symmetries by fixing first (K-1) domain variables to be assigned to cluster less than or equal to their index
for l in range(K-1):
	Constraints = []
	for k in range(K):
		if k > l:
			Constraints.append(Top)
		else:
			Constraints.append(0)
	Problem.AddFunction([Var[l]], Constraints)

#Problem.Dump(sys.argv[1].replace('.mat','.cfn'))
res = Problem.Solve(showSolutions = 3)
	
if res:
	for i in range(K):
		Line = []
		for j in range(K):
			Line.append(res[0][i*K+j])
		print(Line)
	for i in range(N):
		print("The node number " + str(i+1) + " is in cluster " + str(res[0][K**2+i]) + ".")
