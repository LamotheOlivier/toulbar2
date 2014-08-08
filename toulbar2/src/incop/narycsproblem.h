class NaryConstraint;
class NaryVariable;

/* CSP n-aires en extension r�solus en Max-CSP avec poids sur les n-uplets*/
/** NaryCSPs solved as weighted Max-CSPs with weights on the tuples */
class NaryCSProblem : public CSProblem
{ public :
  vector<NaryConstraint*>* naryconstraints;
  vector<NaryVariable*>*   naryvariables;
  NaryCSProblem( int nbvar, int nbconst);
  NaryCSProblem (int nbvar, int nbconst, Long lower);
  ~NaryCSProblem() {;};
  // Long move_evaluation(Configuration* configuration,Move* move);
  // void compute_var_conflict(Configuration* configuration);
/* evaluation et remplissage de la structure de donn�es des conflits*/
/** evaluation and filling the conflict datastructure */
  Long config_evaluation(Configuration* configuration);
  void fullincr_update_conflicts(FullincrCSPConfiguration* configuration,Move* move);
/* calcul du nombre de conflits d'une affectation simple dans une configuration */
/** number of conflicts of a simple assignment in a complete configuration */
  Long compute_conflict(Configuration* configuration, int var , int val);
  // void compute_var_conflict(Configuration* configuration);
  void incr_update_conflicts (IncrCSPConfiguration* configuration, Move* move);
// Long move_evaluation  (Configuration* configuration,Move* move);
/* choix du mode d'incr�mentalit� : IncrCSPConfiguration ou FullincrCSPConfiguration */
/** choice of incrementality mode : IncrCSPConfiguration ou FullincrCSPConfiguration */
Configuration* create_configuration();
};

/* Contrainte N-aire en extension avec poids sur les n-uplets qui violent la contrainte */
/** Nary constraint in extension with weigths defined on the tuples */
class NaryConstraint
{ public :
  int arity;
  NaryConstraint (int arit);
/* evaluation de la contrainte : recherche dans le tableau des n-uplets */
/** Constraint Evalution : searching in the tuple table */
  Long constraint_value (Configuration* configuration);
  int compute_index(int* values, vector<int>* tabdomaines);
  int compute_indexpart (int i, int vali, vector<int>* tabdomaines);
/* variables li�es par la contraintes */
/** variables linked by the constraint */
  vector <int>  constrainedvariables;
/* table des n-uplets valu�s */
/** table of valued tuples */
  vector <Long> tuplevalues;
  vector <int> multiplyers;
  void compute_indexmultiplyers( vector<int>* tabdomaines);
  int compute_indexmultiplyer(int i, vector<int>* tabdomaines);
  int nbtuples(vector<int>* tabdomaines );
};

/* Variable li�e � une contrainte n-aire */
/** Variable constrained by a n-ary constraint */
class NaryVariable
{public :
  vector <NaryConstraint*> constraints;
  NaryVariable();
};

NaryCSProblem* weighted_narycsp_creation (int nbvar, int nbconst, int maxdomsize, 
 vector<NaryVariable*>* vv,vector<NaryConstraint*>* vct     );

void wcspdomaines_file_read (ifstream & file, int nbvar, vector<int>* tabdomaines);

void  wcspdata_constraint_read (ifstream & file, int nbconst, vector<NaryVariable*>* vv, vector<NaryConstraint*>* vct, 
				vector <int>* connexions, vector<int> * tabdomaines);


int narycsp(int argc, char** argv, int tuningmode);
