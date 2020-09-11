
/**
 * Square Packing Problem
 */

// Compile with cmake option -DLIBTB2=ON -DPYTB2=ON to get C++ toulbar2 library lib/Linux/libtb2.so
// Then,
// g++ -o square square.cpp -Isrc -Llib/Linux -std=c++11 -O3 -DNDEBUG -DBOOST -DLINUX -DLONGDOUBLE_PROB -DLONGLONG_COST -DWCSPFORMATONLY libtb2.so

#include "toulbar2lib.hpp"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int N = atoi(argv[1]);
    int S = atoi(argv[2]);

    tb2init(); // must be call before setting specific ToulBar2 options and creating a model

    ToulBar2::verbose = 0; // change to 0 or higher values to see more trace information

    initCosts(); // last check for compatibility issues between ToulBar2 options and Cost data-type

    Cost top = UNIT_COST;
    WeightedCSPSolver* solver = WeightedCSPSolver::makeWeightedCSPSolver(top);

    for (int i=0; i<N; i++) {
        solver->getWCSP()->makeEnumeratedVariable("sq" + to_string(i+1), 0, S*S - 1);
    }

    vector<Cost> costs(S*S, MIN_COST);
    for (int i=0; i<N; i++) {
    	for (int a=0; a<S*S; a++) {
            costs[a] = ((((a % S) + i + 1 <= S) && ((a / S) + i + 1 <= S))?MIN_COST:top);
        }
        solver->getWCSP()->postUnary(i, costs);
    }

    costs.resize(S*S*S*S, MIN_COST);
    for (int i=0; i<N; i++) {
        for (int j=i+1; j<N; j++) {
    	    for (int a=0; a<S*S; a++) {
    	        for (int b=0; b<S*S; b++) {
                    costs[a*S*S+b] = ((((a%S) + i + 1 <= (b%S)) || ((b%S) + j + 1 <= (a%S)) || ((a/S) + i + 1 <= (b/S)) || ((b/S) + j + 1 <= (a/S)))?MIN_COST:top);
                }
            }
            solver->getWCSP()->postBinaryConstraint(i, j, costs);
        }
    }

    solver->getWCSP()->sortConstraints(); // must be done at the end of the modeling

    tb2checkOptions();
    if (solver->solve()) {
            vector<Value> sol;
            solver->getSolution(sol);
    	    for (int y=0; y<S; y++) {
                for (int x=0; x<S; x++) {
                    char c = ' ';
                    for (int i=0; i<N; i++) {
                        if (x >= (sol[i]%S) && x < (sol[i]%S ) + i + 1 && y >= (sol[i]/S) && y < (sol[i]/S) + i + 1) {
                            c = 65+i;
                            break;
                        }
                     }
                     cout << c;
                }
                cout << endl;
            }
    } else {
            cout << "No solution found!" << endl;
    }

    return 0;
}

