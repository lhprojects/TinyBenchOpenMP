# TinyBenchOpenMP
This is a compact benchmark for OpenMP.
The results for my machine are stored in AMD3700X_VC++_Win10.txt and AMD3700X_g++_WSL.txt.
The titles represent the operations.
The column `len` indicates the number of elements to be evaluated.
The column `seq[MOPS]` shows the time used for `std::execution::sequential`, in units of million operations per second.
The column `par/seq` displays the ratio of time for `std::execution::parallel` compared to `std::execution::sequential`.
The column `OpenMP/seq` represents the ratio of time for OpenMP in relation to `std::execution::sequential`.
