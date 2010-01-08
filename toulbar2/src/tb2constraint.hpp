/** \file tb2constraint.hpp
 *  \brief Abstract constraint.
 *
 */

#ifndef TB2CONSTRAINT_HPP_
#define TB2CONSTRAINT_HPP_

#include "tb2types.hpp"
#include <algorithm>

class Constraint : public WCSPLink
{
    Long conflictWeight;
  
    // make it private because we don't want copy nor assignment
    Constraint(const Constraint &c);
    Constraint& operator=(const Constraint &c);

public:
    Constraint(WCSP *wcsp);
    Constraint(WCSP *wcsp, int elimCtrIndex);
        
    virtual ~Constraint() {}

    // remove a constraint from the set of active constraints
    virtual bool connected() {cout << "dummy connected on (" << this << ")!" << endl;return true;}
    virtual bool deconnected() {cout << "dummy deconnected on (" << this << ")!" << endl;return false;}
    virtual void deconnect(bool reuse = false) {cout << "dummy deconnect on (" << this << ")!" << endl;}
    virtual void reconnect() {cout << "dummy reconnect on (" << this << ")!" << endl;}

    virtual int arity() const = 0;
    virtual Variable *getVar(int scopeIndex) const = 0;
    virtual int getIndex(Variable* var) const = 0;

    void conflict();
    Long getConflictWeight() const {return conflictWeight;}
    void incConflictWeight() {conflictWeight++;}
    void resetConflictWeight() {conflictWeight=1;}
    
	double tight;
    double getTightness() { if(tight < 0) computeTightness(); return tight; }
    virtual double  computeTightness() = 0;
    
    // return the smallest wcsp index in the constraint scope except for one variable having a forbidden scope index
    virtual int getSmallestVarIndexInScope(int forbiddenScopeIndex) = 0;
    virtual int getSmallestVarIndexInScope() = 0;
    virtual int getDACScopeIndex() {cout << "dummy getDACScopeIndex on (" << this << ")!" << endl; return 0;}
    virtual void setDACScopeIndex() {}

    virtual void propagate() = 0;
    virtual void increase(int index) {propagate();}
    virtual void decrease(int index) {propagate();}
    virtual void remove(int index) {propagate();}
    virtual void projectFromZero(int index) {}
    virtual void assign(int index) {propagate();}

    virtual void fillEAC2(int index) {}
    virtual bool isEAC(int index, Value a) {return true;}
    virtual void findFullSupportEAC(int index) {}

    void projectLB(Cost cost);

    virtual bool verify() {return true;};
    
    virtual void print(ostream& os) {os << this << " Unknown constraint!";}

    virtual void dump(ostream& os) {os << this << " Unknown constraint!";}

    virtual unsigned long long getDomainSizeProduct();

	virtual void firstlex() {}
	virtual bool nextlex(String& t, Cost& c) { return false; }
 
    virtual void first() {}
    virtual bool next( String& t, Cost& c) { return false; }

	virtual void setTuple( String& t, Cost c, EnumeratedVariable** scope_in ) {}
	virtual void addtoTuple( String& t, Cost c, EnumeratedVariable** scope_in ) {}

	virtual void setTuple( int* t, Cost c, EnumeratedVariable** scope_in ) {}
	virtual void addtoTuple( int* t, Cost c, EnumeratedVariable** scope_in ) {}


	virtual void getScope( TSCOPE& scope_inv ) {}
	virtual Cost evalsubstr( String& s, Constraint* ctr ) { return MIN_COST; }
	virtual Cost getDefCost() { return MIN_COST; }
	virtual void setDefCost( Cost df ) {}       

    virtual bool universal();

    virtual Cost getMinCost();
	
	void sumScopeIncluded( Constraint* ctr );
	

	bool scopeIncluded( Constraint* ctr )
	{
		bool isincluded = true;
		int a_in = ctr->arity();
		if(a_in >= arity()) return false;		
		for(int i=0;isincluded && i<a_in;i++) isincluded = isincluded && (getIndex( ctr->getVar(i) ) >= 0);   
		return isincluded;
	}

	
	void scopeCommon( TSCOPE& scope_out, Constraint* ctr ) 
	{
		TSCOPE scope1,scope2;
		getScope( scope1 );
		ctr->getScope( scope2 );
		
		TSCOPE::iterator it1 = scope1.begin();
		TSCOPE::iterator it2 = scope2.begin();
		while(it1 != scope1.end()) { it1->second = 0; ++it1; }
		while(it2 != scope2.end()) { it2->second = 0; ++it2; }
		set_intersection( scope1.begin(), scope1.end(),
				  	   	  scope2.begin(), scope2.end(),
					  	  inserter(scope_out, scope_out.begin()) );			 	  
	}
	
		
	void scopeUnion( TSCOPE& scope_out, Constraint* ctr ) 
	{
		TSCOPE scope1,scope2;
		getScope( scope1 ); ctr->getScope( scope2 );

		assert(arity() == (int) scope1.size());
		assert(ctr->arity() == (int) scope2.size());

		set_union( scope1.begin(), scope1.end(),
		  	   	   scope2.begin(), scope2.end(),
			  	   inserter(scope_out, scope_out.begin()) );		
	}
		
	void scopeDifference( TSCOPE& scope_out, Constraint* ctr )
	{
		TSCOPE scope1,scope2;
		getScope( scope1 );
		ctr->getScope( scope2 );
		set_difference( scope1.begin(), scope1.end(),
			  	   	    scope2.begin(), scope2.end(),
				  	    inserter(scope_out, scope_out.begin()) );				
	}
	
	int order( Constraint* ctr )
	{
		if(arity() < ctr->arity()) return 1;
		else if (arity()  > ctr->arity()) return -1;	
		TSCOPE scope1,scope2;
		getScope( scope1 );
		ctr->getScope( scope2 );
		TSCOPE::iterator it1 = scope1.begin();
		TSCOPE::iterator it2 = scope2.begin();
		while(it1 != scope1.end()) {
			if(it1->first < it2->first) return 1;
			else if (it1->first > it2->first) return -1;
			++it1;
			++it2;
		}	
		return 0;
	}
	
	
    //   added for tree decomposition stuff	
	int  cluster;
	int  getCluster()      {return cluster;}
	void setCluster(int i) {cluster = i;}
	void assignCluster();

    bool isSep_;
	void setSep() 		   {isSep_ = true;}
	bool isSep() 		   {return isSep_;}


    bool isDuplicate_;
	void setDuplicate()	   {isDuplicate_ = true; if (ToulBar2::verbose >= 1) { cout << *this << " set duplicate" << endl; }}
	bool isDuplicate() 	   {return isDuplicate_;}
	
	
	
    friend ostream& operator<<(ostream& os, Constraint &c) {
        c.print(os);
        return os;
    }
};

#endif /*TB2CONSTRAINT_HPP_*/