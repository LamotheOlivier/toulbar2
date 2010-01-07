#!/bin/bash

# Usage:
# ./runall.sh ../validation

timelimit=3600
K=10000

rm -f outall

for e in `find $1 -regex ".*[.]wcsp" -print | sort` ; do
    dir=`dirname $e`
    base=`basename $e .wcsp`
    file=$dir/$base

# COMMENT OUT IF USING INITIAL UPPER BOUND
    ubfile=${dir}/${base}.ub

    rm -f out
    rm -f outsolver
    rm -f usedtime

    if [[ -e $ubfile ]] ; then
	    ub=`cat $ubfile`
	    ub=`expr $ub \* $K`
        echo -n $ub > out
    else
        ub=
        echo -n "-" > out
    fi
   
    echo -n $file " "
    echo -n $file " " >> outall

    ulimit -t $timelimit > /dev/null
    (/usr/bin/time -f "%U user %S sys" ./toulbar2 $file.wcsp $ub $2 C$K >> outsolver) 2> usedtime

    cat outsolver | awk 'BEGIN{opt="-";nodes=0;} /^Optimum: /{opt=$2; nodes=$7;} /^No solution /{opt="'$ub'"; nodes=$7;}  END{printf(" %s %d ",opt,nodes); }' >> out ; cat out

    cat usedtime | awk '/ user /{ printf("%.2f",0.0+$1+$3); }'
   
    cat out  | awk '/ /{ if($1 != $2) printf("           *******ERROR optimal cost"); }'
    
    cat out >> outall  
    cat usedtime | awk '/ user /{ printf(" %.2f",0.0+$1+$3); }' >> outall
    echo " " >> outall
    echo " " ;
done

cat outall | awk -f evalresults.awk | sort
