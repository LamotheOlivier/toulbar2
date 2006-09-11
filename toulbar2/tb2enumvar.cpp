
/*
 * ****** Variable with domain represented by an enumerated domain *******
 */
 
#include "tb2enumvar.hpp"
#include "tb2wcsp.hpp"
#include "tb2binconstr.hpp"
#include "tb2ternaryconstr.hpp"


/*
 * Constructors and misc.
 * 
 */


EnumeratedVariable::EnumeratedVariable(WCSP *w, string n, Value iinf, Value isup) : 
        Variable(w, n, iinf, isup), 
        domain(iinf, isup, &w->getStore()->storeDomain),
        support(iinf, &w->getStore()->storeValue)
{
    init();
}

EnumeratedVariable::EnumeratedVariable(WCSP *w, string n, Value *d, int dsize) : 
        Variable(w, n, min(d,dsize), max(d, dsize)), 
        domain(d, dsize, &w->getStore()->storeDomain),
        support(min(d,dsize), &w->getStore()->storeValue)
{
    init();
}

void EnumeratedVariable::init()
{
    if (wcsp->getStore()->getDepth() > 0) {
        cerr << "You cannot create a variable during the search!" << endl;
        exit(EXIT_FAILURE);
    }

    costs = vector<StoreCost>(getDomainInitSize(), StoreCost(0, &wcsp->getStore()->storeCost));
    linkACQueue.content.var = this;
    linkACQueue.content.timeStamp = -1;
    linkDACQueue.content.var = this;
    linkDACQueue.content.timeStamp = -1;
}

void EnumeratedVariable::getDomain(Value *array)
{
    for (iterator iter = begin(); iter != end(); ++iter) {
    	*array = *iter;
    	++array;
    }
}

void EnumeratedVariable::getDomainAndCost(ValueCost *array)
{
    for (iterator iter = begin(); iter != end(); ++iter) {
    	array->value = *iter;
    	array->cost = getCost(*iter);
    	++array;
    }
}

Cost EnumeratedVariable::getBinaryCost(ConstraintLink c, Value myvalue, Value itsvalue) {
    return (c.scopeIndex == 0)?((BinaryConstraint *) c.constr)->getCost(myvalue, itsvalue):((BinaryConstraint *) c.constr)->getCost(itsvalue, myvalue);
}

Cost EnumeratedVariable::getBinaryCost(BinaryConstraint* c, Value myvalue, Value itsvalue) {
    return (c->getIndex(this) == 0)?c->getCost(myvalue, itsvalue):c->getCost(itsvalue, myvalue);
}


void EnumeratedVariable::print(ostream& os)
{
    if (unassigned()) {
        os << " " << domain;
    } else {
        os << " [" << inf << "," << sup << "]";
    }
    os << "/" << getDegree();
    if (unassigned()) {
        os << " <";
        for (iterator iter=begin(); iter != end(); ++iter) {
            os << " " << getCost(*iter);
        }
        os << " > s:" << support;
    }
}


/*
 * Propagation methods
 * 
 */

void EnumeratedVariable::queueAC()
{
    wcsp->queueAC(&linkACQueue);
}

void EnumeratedVariable::queueDAC()
{
    wcsp->queueDAC(&linkDACQueue);
}

void EnumeratedVariable::project(Value value, Cost cost)
{
    assert(cost >= 0);
    Cost oldcost = getCost(value);
    costs[toIndex(value)] += cost;
    Cost newcost = oldcost + cost;
    if (value == maxCostValue || newcost > maxCost) queueNC();
    if (oldcost == 0 && cost > 0) queueDAC();
    if (newcost + wcsp->getLb() >= wcsp->getUb()) removeFast(value);     // Avoid any unary cost overflow
}

void EnumeratedVariable::projectInfCost(Cost cost)
{
    assert(cost >= 0);
    Value value = getInf();
    project(value, cost);
    if (support == value) findSupport();
}

void EnumeratedVariable::projectSupCost(Cost cost)
{
    assert(cost >= 0);
    Value value = getSup();
    project(value, cost);
    if (support == value) findSupport();
}

void EnumeratedVariable::extend(Value value, Cost cost)
{
    assert(cost >= 0);
    assert(costs[toIndex(value)] >= cost);
    costs[toIndex(value)] -= cost;
    if (value == maxCostValue) queueNC();
}

void EnumeratedVariable::findSupport()
{
    if (cannotbe(support) || getCost(support) > 0) {
        Value newSupport = getInf();
        Cost minCost = getCost(newSupport);
        iterator iter = begin();
        for (++iter; minCost > 0 && iter != end(); ++iter) {
            Cost cost = getCost(*iter);
            if (cost < minCost) {
                minCost = cost;
                newSupport = *iter;
            }
        }
        if (minCost > 0) {
            extendAll(minCost);
            if (ToulBar2::verbose >= 2) cout << "lower bound increased " << wcsp->getLb() << " -> " << wcsp->getLb()+minCost << endl;
            wcsp->increaseLb(wcsp->getLb() + minCost);
        }
        assert(canbe(newSupport) && getCost(newSupport) == 0);
        support = newSupport;
    }
}

void EnumeratedVariable::propagateNC()
{
    if (ToulBar2::verbose >= 3) cout << "propagateNC for " << getName() << endl;
    Value maxcostvalue = getSup()+1;
    Cost maxcost = -1;
    // Warning! the first value must be visited because it may be removed
    for (iterator iter = begin(); iter != end(); ++iter) {
        Cost cost = getCost(*iter);
        if (cost + wcsp->getLb() >= wcsp->getUb()) {
            removeFast(*iter);
        } else if (cost > maxcost) {
            maxcostvalue = *iter;
            maxcost = cost;
        }
    }
    setMaxUnaryCost(maxcostvalue, getCost(maxcostvalue));
}

bool EnumeratedVariable::verifyNC()
{
    bool supported = false;
    for (iterator iter = begin(); iter != end(); ++iter) {
        if (getCost(*iter) + wcsp->getLb() >= wcsp->getUb()) {
            cout << *this << " not NC!" << endl;
            return false;
        }
        if (getCost(*iter) == 0) supported = true;
    }
    if (!supported) cout << *this << " not NC*!" << endl;
    if (cannotbe(support) || getCost(support)>0) {
        cout << *this << " has an unvalid NC support!" << endl;
        supported = false;
    }
    return supported;
}

void EnumeratedVariable::propagateAC()
{
    for (ConstraintList::iterator iter=constrs.begin(); iter != constrs.end(); ++iter) {
        (*iter).constr->remove((*iter).scopeIndex);
    }
}

void EnumeratedVariable::propagateDAC()
{
    for (ConstraintList::iterator iter=constrs.rbegin(); iter != constrs.rend(); --iter) {
        (*iter).constr->projectFromZero((*iter).scopeIndex);
    }
}

void EnumeratedVariable::increaseFast(Value newInf)
{
    if (ToulBar2::verbose >= 2) cout << "increase " << getName() << " " << inf << " -> " << newInf << endl;
    if (newInf > inf) {
        if (newInf > sup) THROWCONTRADICTION;
        else {
            newInf = domain.increase(newInf);
            if (newInf == sup) assign(newInf);
            else {
                inf = newInf;
                queueInc();
                if (ToulBar2::setmin) (*ToulBar2::setmin)(wcsp->getIndex(), wcspIndex, newInf);
            }
        }
      }
}

void EnumeratedVariable::increase(Value newInf)
{
    if (ToulBar2::verbose >= 2) cout << "increase " << getName() << " " << inf << " -> " << newInf << endl;
    if (newInf > inf) {
        if (newInf > sup) THROWCONTRADICTION;
        else {
            newInf = domain.increase(newInf);
            if (newInf == sup) assign(newInf);
            else {
                inf = newInf;
                if (newInf > maxCostValue) queueNC();           // diff with increaseFast
                if (newInf > support) findSupport();            // diff with increaseFast
                queueDAC();                                     // diff with increaseFast
                queueInc();
                if (ToulBar2::setmin) (*ToulBar2::setmin)(wcsp->getIndex(), wcspIndex, newInf);
            }
        }
      }
}

void EnumeratedVariable::decreaseFast(Value newSup)
{
    if (ToulBar2::verbose >= 2) cout << "decrease " << getName() << " " << sup << " -> " << newSup << endl;
    if (newSup < sup) {
        if (newSup < inf) THROWCONTRADICTION;
        else {
            newSup = domain.decrease(newSup);
            if (inf == newSup) assign(newSup);
            else {
                sup = newSup;
                queueDec();
                if (ToulBar2::setmax) (*ToulBar2::setmax)(wcsp->getIndex(), wcspIndex, newSup);
            }
        }
      }
}

void EnumeratedVariable::decrease(Value newSup)
{
    if (ToulBar2::verbose >= 2) cout << "decrease " << getName() << " " << sup << " -> " << newSup << endl;
    if (newSup < sup) {
        if (newSup < inf) THROWCONTRADICTION;
        else {
            newSup = domain.decrease(newSup);
            if (inf == newSup) assign(newSup);
            else {
                sup = newSup;
                if (newSup < maxCostValue) queueNC();           // diff with decreaseFast
                if (newSup < support) findSupport();            // diff with decreaseFast
                queueDAC();                                     // diff with decreaseFast
                queueDec();
                if (ToulBar2::setmax) (*ToulBar2::setmax)(wcsp->getIndex(), wcspIndex, newSup);
            }
        }
      }
}

void EnumeratedVariable::removeFast(Value value)
{
    if (ToulBar2::verbose >= 2) cout << "remove " << *this << " <> " << value << endl;
    if (value == inf) increaseFast(value + 1);
    else if (value == sup) decreaseFast(value - 1);
    else if (canbe(value)) {
        domain.erase(value);
        queueAC();
        if (ToulBar2::removevalue) (*ToulBar2::removevalue)(wcsp->getIndex(), wcspIndex, value);
    }
}

void EnumeratedVariable::remove(Value value)
{
    if (ToulBar2::verbose >= 2) cout << "remove " << *this << " <> " << value << endl;
    if (value == inf) increase(value + 1);
    else if (value == sup) decrease(value - 1);
    else if (canbe(value)) {
        domain.erase(value);
        if (value == maxCostValue) queueNC();
        if (value == support) findSupport();
        queueDAC();
        queueAC();
        if (ToulBar2::removevalue) (*ToulBar2::removevalue)(wcsp->getIndex(), wcspIndex, value);
    }
}

void EnumeratedVariable::assign(Value newValue)
{
    if (ToulBar2::verbose >= 2) cout << "assign " << *this << " -> " << newValue << endl;
    if (unassigned() || getValue() != newValue) {
        if (cannotbe(newValue)) THROWCONTRADICTION;
        changeNCBucket(-1);
        maxCostValue = newValue;
        maxCost = 0;
        inf = newValue;
        sup = newValue;
        support = newValue;
   
        Cost cost = getCost(newValue);
        if (cost > 0) {
            deltaCost += cost;
            if (ToulBar2::verbose >= 2) cout << "lower bound increased " << wcsp->getLb() << " -> " << wcsp->getLb()+cost << endl;
            wcsp->increaseLb(wcsp->getLb() + cost);
        }

	    if (ToulBar2::setvalue) (*ToulBar2::setvalue)(wcsp->getIndex(), wcspIndex, newValue);
        for (ConstraintList::iterator iter=constrs.begin(); iter != constrs.end(); ++iter) {
            (*iter).constr->assign((*iter).scopeIndex);
        }
    }
}


// eliminates the current (this) variable that participates
// in a single binary consraint ctr
void EnumeratedVariable::elimVar( BinaryConstraint* ctr )
{
       assert(getDegree() == 1);
       // deconnect first to be sure the current var is not involved in future propagation	
       ctr->deconnect();

	   EnumeratedVariable *x = (EnumeratedVariable *) wcsp->getVar(ctr->getSmallestVarIndexInScope(ctr->getIndex(this)));
       bool supportBroken = false;
       for (iterator iter1 = x->begin(); iter1 != x->end(); ++iter1) {
           Cost mincost = MAX_COST;
           for (iterator iter = begin(); iter != end(); ++iter) {
               Cost curcost = getCost(*iter) + getBinaryCost(ctr, *iter, *iter1);
               if (curcost < mincost) mincost = curcost;
           }
           if (mincost > 0) {
		       if (x->getSupport() == *iter1) supportBroken = true;
               x->project(*iter1, mincost);
           }
       }
      if (supportBroken) {  x->findSupport();  }
}


// eliminates the current (this) variable that participates
// in two binary constraints (its links ara xylink and xzlink)
void EnumeratedVariable::elimVar( ConstraintLink  xylink,  ConstraintLink xzlink )
{
  	 assert(getDegree() == 2);
     xylink.constr->deconnect(); 	            
	 xzlink.constr->deconnect();

	 EnumeratedVariable *y = (EnumeratedVariable *) wcsp->getVar(xylink.constr->getSmallestVarIndexInScope(xylink.scopeIndex));
	 EnumeratedVariable *z = (EnumeratedVariable *) wcsp->getVar(xzlink.constr->getSmallestVarIndexInScope(xzlink.scopeIndex));
	
	 BinaryConstraint* yz = y->getConstr(z);
	        
     BinaryConstraint* yznew = wcsp->newBinaryConstr(y,z); 

	 for (iterator itery = y->begin(); itery != y->end(); ++itery) {
	 for (iterator iterz = z->begin(); iterz != z->end(); ++iterz) {
	    Cost mincost = MAX_COST;

	    for (iterator iter = begin(); iter != end(); ++iter) {
	        Cost curcost = getCost(*iter) + 
						   getBinaryCost(xylink, *iter, *itery) +
						   getBinaryCost(xzlink, *iter, *iterz);
						   
	        if (curcost < mincost) mincost = curcost;
	    }
		yznew->setcost(*itery,*iterz,mincost);
	 }}

	 if(yz) yz->addCosts( yznew );
	 else { 
	 		yz = yznew;
	 		yz->reconnect();
	 		wcsp->elimination();
	 }	
     yz->propagate(); 
     y->queueAC();  y->queueDAC(); 
     z->queueAC();  z->queueDAC();
}

// eliminates the current (this) variable that participates
// in the ternary constraint 'xyz'
// the function can fail to eliminate the current variable
// if it is linked to more than (in total) two variables.
// It returns true if the current variable was eliminated
bool EnumeratedVariable::elimVar( TernaryConstraint* xyz )
{
	BinaryConstraint* yz = NULL;
	if(xyz->xy->getIndex(this) < 0) yz = xyz->xy;
	else if(xyz->xz->getIndex(this) < 0) yz = xyz->xz;
	else if(xyz->yz->getIndex(this) < 0) yz = xyz->yz;	 
	assert(yz != NULL);

	int n2links = 0;
	int n3links = 0;

	ConstraintLink links[2] = {{NULL, 0},{NULL, 0}};
 	for(ConstraintList::iterator iter=constrs.begin(); iter != constrs.end(); ++iter) {
 	   if((*iter).constr->arity() == 2) links[n2links++] =  (*iter);
 	   else n3links++;
 	}

	if(n3links > 1) return false;

	for(int i=0; i<n2links; i++) {		
		int idvar = links[i].constr->getSmallestVarIndexInScope(links[i].scopeIndex);
		if(xyz->getIndex( wcsp->getVar(idvar) ) < 0) return false; 
	}

	xyz->deconnect();
	if(n2links > 0) links[0].constr->deconnect();
	if(n2links > 1) links[1].constr->deconnect();

	EnumeratedVariable* y = (EnumeratedVariable*) yz->getVar(0);
	EnumeratedVariable* z = (EnumeratedVariable*) yz->getVar(1);

	bool flag_rev = false;
	if(n2links > 0) {
		if(this == links[0].constr->getVar(0)) flag_rev = (y != links[0].constr->getVar(1)); 
		else flag_rev = (y != links[0].constr->getVar(0)); 
	}
		
	for (iterator itery = y->begin(); itery != y->end(); ++itery) {
	for (iterator iterz = z->begin(); iterz != z->end(); ++iterz) {
	    Cost mincost = MAX_COST;
	    for (iterator iter = begin(); iter != end(); ++iter) {
	        Cost curcost = getCost(*iter) + xyz->getCost(this,y,z,*iter,*itery,*iterz); 

			if(!flag_rev) {
		        if(n2links > 0) curcost += getBinaryCost(links[0], *iter, *itery);
				if(n2links > 1) curcost += getBinaryCost(links[1], *iter, *iterz);
			} else {
		        if(n2links > 0) curcost += getBinaryCost(links[0], *iter, *iterz);
				if(n2links > 1) curcost += getBinaryCost(links[1], *iter, *itery);
			}

	        if (curcost < mincost) mincost = curcost;
	    }
		yz->addcost(*itery,*iterz,mincost);
	 }}

	if(!yz->connected()) yz->reconnect();
    yz->propagate(); 
    y->queueAC();  y->queueDAC(); 
    z->queueAC();  z->queueDAC();
	 
	return true;
}


void EnumeratedVariable::eliminate()
{
    assert(ToulBar2::elimLevel <= 2);

    if((ToulBar2::elimLevel == 1) &&  (getDegree() > 1)) return;
    if((ToulBar2::elimLevel == 2) &&  (getDegree() > 3)) return;

	if(getDegree() > 0) {
		TernaryConstraint* ternCtr = existTernary();
				
		if(ternCtr) { if(!elimVar(ternCtr)) return; }
		else {
			if(getDegree() > 2) return;
	
			ConstraintLink xylink = *constrs.begin();
			ConstraintLink xzlink = {NULL,0};
			
			if(getDegree() == 2) {
				xzlink = *constrs.rbegin();
				elimVar(xylink,xzlink);		
			}
			else 
			{
				BinaryConstraint* xy = (BinaryConstraint*) xylink.constr;	
				elimVar( xy );
			}		
		}
	}

	assert(getDegree() == 0);
	if (ToulBar2::verbose) cout << "Eliminate " << getName() << endl;
	assert(getCost(support) == 0); // it is ensured by previous calls to findSupport
	assign(support); // warning! dummy assigned value
}


