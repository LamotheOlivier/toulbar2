.. _tuto_sspp:

===========================
Square soft packing problem
===========================

.. include:: menu_backto.rst

Brief description
=================

The problem is almost identical to the Square packing problem with the difference that we now allow overlaps but we want to minimize it.

CFN model
=========

We reuse the :ref:`tuto_spp` model except that binary constraints are replaced by cost functions returning the overlapping size or zero if no overlaps.

To calculate an initial upper bound we will simply say that the worst case scenario can't be worse if we had N N*N square that are all stacks together. The cost of this is N**4, so we will take N**4 as the initial upperbound.

Python model solver
======================

The following code using python3 interpreter will solve the corresponding cost function network (e.g. "python3 SquareSoftpytb2.py 10 20").

:download:`SquareSoftpytb2.py<../../../web/TUTORIALS/SquareSoftpytb2.py>`

.. literalinclude:: ../../../web/TUTORIALS/SquareSoftpytb2.py

C++ program using libtb2.so
===========================

The following code uses the C++ toulbar2 library libtb2.so. Compile toulbar2 with "cmake -DLIBTB2=ON -DPYTB2=ON . ; make" and copy the library in your current directory "cp lib/Linux/libtb2.so ." before compiling "g++ -o squaresoft squaresoft.cpp -Isrc -Llib/Linux -std=c++11 -O3 -DNDEBUG -DBOOST -DLONGDOUBLE_PROB -DLONGLONG_COST -DWCSPFORMATONLY libtb2.so" and running the example (e.g. "./squaresoft 10 20").

:download:`squaresoft.cpp<../../../web/TUTORIALS/squaresoft.cpp>`

.. literalinclude:: ../../../web/TUTORIALS/squaresoft.cpp

