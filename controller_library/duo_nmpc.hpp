/*
 *    This file is part of duo-nmpc.
 *
 *    Duo-nmpc
 *    Copyright (C) 2018 Niels van Duijkeren, KU Leuven.
 *
 *    Duo-nmpc is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 3 of the License, or (at your option) any later version.
 *
 *    Duo-nmpc is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with duo-nmpc; if not, write to the Free Software Foundation,
 *    Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef DUO_NMPC_HPP_
#define DUO_NMPC_HPP_

#include <vector>
#include "transverse_ocp.hpp"
#include "tangential_ocp.hpp"

class DuoNmpc {
public:
    DuoNmpc(const std::vector<double> &xinit, const std::vector<double> &uinit, int maxit, double reg_eps);
    virtual ~DuoNmpc();

    std::vector<double> Solve(const std::vector<double> &x0, const std::vector<double> &ptr, const std::vector<double> &pta, double epsN);
    void Shift();

    double GetTransverseKkt();
    double GetTangentialKkt();

    TransverseOcp ocp_tr;
    TangentialOcp ocp_ta;
private:
    int sys_nx;
    int sys_nu;
    int maxit;
    double reg_eps;

    bool reset_tang;
};

#endif // DUO_NMPC_HPP_