
Toolbar was a generic solver for Weighted CSP, Max-SAT, and Bayesian Networks. Toulbar2 is a new C++ version that includes most algorithms of toolbar, some new ones, the ability to work in collaboration with ILOG Solver, and to deal with big (thousands of variables and/or large domains) problems. It is a free software under BSD license. A reference to what a WCSP is, and what the algorithms inside toulbar2 are can be found in:

Soft NC, AC, DAC, FDAC
 In the quest of the best form of local consistency for Weighted CSP
 J. Larrosa & T. Schiex
 In Proc. of IJCAI-03. Acapulco, Mexico, 2003
 http://www.lsi.upc.edu/~larrosa/PAPERS/PAPER-IJCAI03/Final/IJCAI03.ps

EDAC
 Existential arc consistency: Getting closer to full arc consistency in weighted csps
 S. de Givry, M. Zytnicki, F. Heras, and J. Larrosa
 In Proc. of IJCAI-05, Edinburgh, Scotland, 2005
 http://www.inra.fr/mia/T/degivry/Heras05.pdf

BTD
 Exploiting Tree Decomposition and Soft Local Consistency in Weighted CSP
 S. de Givry, T. Schiex, and G. Verfaillie
 In Proc. of AAAI-06, Boston, MA, 2006 
 http://www.inra.fr/mia/T/degivry/Schiex06a.pdf

VAC
 Virtual arc consistency for weighted csp
 M. Cooper, S. de Givry, M. Sanchez, T. Schiex, and M. Zytnicki
 In Proc. of AAAI-08, Chicago, IL, 2008
 http://www.inra.fr/mia/T/degivry/Cooper08.pdf

RDS-BTD
 Russian doll search with tree decomposition
 M Sanchez, D Allouche, S de Givry, and T Schiex
 In Proc. of IJCAI'09, Pasadena (CA), USA, 2009
 http://www.inra.fr/mia/T/degivry/Sanchez09a.pdf

BAC
 Bounds Arc Consistency for Weighted CSPs
 M. Zytnicki, C. Gaspin, S. de Givry, and T. Schiex
 Journal of Artificial Intelligence Research, 35:593-621, 2009
 http://www.inra.fr/mia/T/degivry/Zytnicki09a.pdf

#BTD, Approx_#BTD
 Exploiting problem structure for solution counting
 A. Favier, S. de Givry, and P. J�gou
 In Proc. of CP-09, Lisbon, Portugal, 2009
 http://www.inra.fr/mia/T/degivry/Favier09a.pdf


See a short description of the toolbar/toulbar2 collaborative project at:
http://www.inra.fr/mia/T/degivry/ToolBar.pdf
and
http://www.inra.fr/mia/T/degivry/Sanchez08b.pdf

toolbar and toulbar2 are developped as open source programs and are accessible at INRA source forge:

    * toolbar: http://mulcyber.toulouse.inra.fr/projects/toolbar (not anymore developed) 
    * toulbar2: http://mulcyber.toulouse.inra.fr/projects/toulbar2 

The XCSP 2.1 format reader of CSP and WCSP instances has been provided by Olivier Roussel (roussel at cril.univ-artois.fr).

Executable narycsp is part of INCOP, a local search solver developed by Bertrand Neveu (Bertrand.Neveu@sophia.inria.fr). See http://www-sop.inria.fr/coprin/neveu/incop/presentation-incop.html

Executable peo implements various ordering heuristics for variable elimination methods used by BTD-like search methods. It has been provided by Cyril Terrioux (cyril.terrioux@univ-cezanne.fr).
