# A Dual-Objective NMPC Scheme

A sequential dual-objective NMPC scheme applied to a planar quadrotor. Optimize time-optimal motion, stabilizing a target neighborhood of a state-space manifold.

## Requirements
* A Linux OS
* matlab
* python (>=3)
* swig, tested with v3.0.12
* gcc, g++
* blas
* lapack
* casadi, tested with v3.3.0-rc2
* acados, tested with [bd193f3](https://github.com/acados/acados/commit/bd193f365d7d2cf04d027b386b8de58075cf7458)
  * blasfeo, tested with [4815341](https://github.com/giaf/blasfeo/commit/4815341368f2816de3db8b634d9baf2353a2e7b0)
  * hpipm, tested with [2eb8b1f](https://github.com/giaf/hpipm/commit/2eb8b1f2846eb2a17558f1747f8b6af2da9e692e)
  
## How to use
* From Matlab, execute ``code_generation/generate_code.m``
* In a terminal, browse to ``controller_library`` and execute ``make install``
* Use Python to run ``simulation/simulate.py`` for simulations
