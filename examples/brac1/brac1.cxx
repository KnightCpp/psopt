
//////////////////////////////////////////////////////////////////////////
//////////////////            brac1.cxx        ///////////////////////////
//////////////////////////////////////////////////////////////////////////
////////////////           PSOPT  Example             ////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////// Title:                 Brachistochrone problem   ////////////////
//////// Last modified:         04 January 2009           ////////////////
//////// Reference:             Bryson and Ho (1975)      ////////////////
//////// (See PSOPT handbook for full reference)          ////////////////
//////////////////////////////////////////////////////////////////////////
////////     Copyright (c) Victor M. Becerra, 2009        ////////////////
//////////////////////////////////////////////////////////////////////////
//////// This is part of the PSOPT software library, which ///////////////
//////// is distributed under the terms of the GNU Lesser ////////////////
//////// General Public License (LGPL)                    ////////////////
//////////////////////////////////////////////////////////////////////////

#include "psopt.h"

//////////////////////////////////////////////////////////////////////////
///////////////////  Define the end point (Mayer) cost function //////////
//////////////////////////////////////////////////////////////////////////

adouble endpoint_cost(adouble* initial_states, adouble* final_states,
                      adouble* parameters,adouble& t0, adouble& tf,
                      adouble* xad, int iphase, Workspace* workspace)
{
    return tf;
}

//////////////////////////////////////////////////////////////////////////
///////////////////  Define the integrand (Lagrange) cost function  //////
//////////////////////////////////////////////////////////////////////////

adouble integrand_cost(adouble* states, adouble* controls, adouble* parameters,
                     adouble& time, adouble* xad, int iphase, Workspace* workspace)
{
    return  0.0;
}

//////////////////////////////////////////////////////////////////////////
///////////////////  Define the DAE's ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void dae(adouble* derivatives, adouble* path, adouble* states,
         adouble* controls, adouble* parameters, adouble& time,
         adouble* xad, int iphase, Workspace* workspace)
{
   adouble xdot, ydot, vdot;

   adouble x = states[ 0 ];
   adouble y = states[ 1 ];
   adouble v = states[ 2 ];

   adouble theta = controls[ 0 ];

   xdot = v*sin(theta);
   ydot = v*cos(theta);
   vdot = 9.8*cos(theta);

   derivatives[ 0 ] = xdot;
   derivatives[ 1 ] = ydot;
   derivatives[ 2 ] = vdot;
}

////////////////////////////////////////////////////////////////////////////
///////////////////  Define the events function ////////////////////////////
////////////////////////////////////////////////////////////////////////////

void events(adouble* e, adouble* initial_states, adouble* final_states,
            adouble* parameters,adouble& t0, adouble& tf, adouble* xad,
            int iphase, Workspace* workspace)
{
   adouble x0 = initial_states[ 0 ];
   adouble y0 = initial_states[ 1 ];
   adouble v0 = initial_states[ 2 ];
   adouble xf = final_states[ 0];
   adouble yf = final_states[ 1];

   e[ 0 ] = x0;
   e[ 1 ] = y0;
   e[ 2 ] = v0;
   e[ 3 ] = xf;
   e[ 4 ] = yf;
}

///////////////////////////////////////////////////////////////////////////
///////////////////  Define the phase linkages function ///////////////////
///////////////////////////////////////////////////////////////////////////

void linkages( adouble* linkages, adouble* xad, Workspace* workspace)
{
  // No linkages as this is a single phase problem
}


////////////////////////////////////////////////////////////////////////////
///////////////////  Define the main routine ///////////////////////////////
////////////////////////////////////////////////////////////////////////////


int main(void)
{

////////////////////////////////////////////////////////////////////////////
///////////////////  Declare key structures ////////////////////////////////
////////////////////////////////////////////////////////////////////////////

    Alg  algorithm;
    Sol  solution;
    Prob problem;

////////////////////////////////////////////////////////////////////////////
///////////////////  Register problem name  ////////////////////////////////
////////////////////////////////////////////////////////////////////////////

    problem.name        		= "Brachistochrone Problem";

    problem.outfilename                 = "brac1.txt";

////////////////////////////////////////////////////////////////////////////
////////////  Define problem level constants & do level 1 setup ////////////
////////////////////////////////////////////////////////////////////////////

    problem.nphases   			= 1;
    problem.nlinkages                   = 0;

    psopt_level1_setup(problem);


/////////////////////////////////////////////////////////////////////////////
/////////   Define phase related information & do level 2 setup /////////////
/////////////////////////////////////////////////////////////////////////////

    problem.phases(1).nstates   		= 3;
    problem.phases(1).ncontrols 		= 1;
    problem.phases(1).nevents   		= 5;
    problem.phases(1).npath     		= 0;
    problem.phases(1).nodes                     << 40;

    psopt_level2_setup(problem, algorithm);


////////////////////////////////////////////////////////////////////////////
///////////////////  Enter problem bounds information //////////////////////
////////////////////////////////////////////////////////////////////////////


    problem.phases(1).bounds.lower.states   	<<  0,  0,  0;
    problem.phases(1).bounds.upper.states   	<< 20, 20, 20;

    problem.phases(1).bounds.lower.controls 	<< 0.0;
    problem.phases(1).bounds.upper.controls 	<< 2*pi;

    problem.phases(1).bounds.lower.events   	<< 0,  0,  0,  2,  2;
    problem.phases(1).bounds.upper.events   	<< 0,  0,  0,  2,  2;


    problem.phases(1).bounds.lower.StartTime    = 0.0;
    problem.phases(1).bounds.upper.StartTime    = 0.0;

    problem.phases(1).bounds.lower.EndTime      = 0.0;
    problem.phases(1).bounds.upper.EndTime      = 10.0;


////////////////////////////////////////////////////////////////////////////
///////////////////  Register problem functions  ///////////////////////////
////////////////////////////////////////////////////////////////////////////


    problem.integrand_cost 	= &integrand_cost;
    problem.endpoint_cost 	= &endpoint_cost;
    problem.dae 		= &dae;
    problem.events 		= &events;
    problem.linkages		= &linkages;


////////////////////////////////////////////////////////////////////////////
///////////////////  set scaling factors (optional) ////////////////////////
////////////////////////////////////////////////////////////////////////////

//    problem.phases(1).scale.controls    = 1.0*ones(1,1);
//    problem.phases(1).scale.states      = 1.0*ones(3,1);
//    problem.phases(1).scale.events      = 1.0*ones(5,1);
//    problem.phases(1).scale.defects     = 1.0*ones(3,1);
//    problem.phases(1).scale.time        = 1.0;

//    problem.scale.objective             = 1.0;

////////////////////////////////////////////////////////////////////////////
///////////////////  Define & register initial guess ///////////////////////
////////////////////////////////////////////////////////////////////////////



    MatrixXd x0(3,20);

    x0.row(0) = linspace(0.0,1.0, 20);
    x0.row(1) = linspace(0.0,1.0, 20);
    x0.row(2) = linspace(0.0,1.0, 20);

    problem.phases(1).guess.controls       = ones(1,20);
    problem.phases(1).guess.states         = x0;
    problem.phases(1).guess.time           = linspace(0.0, 2.0, 20);

////////////////////////////////////////////////////////////////////////////
///////////////////  Enter algorithm options  //////////////////////////////
////////////////////////////////////////////////////////////////////////////


    algorithm.nlp_method                  = "IPOPT";
    algorithm.scaling                     = "automatic";
    algorithm.derivatives                 = "automatic";
    algorithm.nlp_iter_max                = 1000;
    algorithm.nlp_tolerance               = 1.e-6;
//    algorithm.hessian			  = "exact";
    algorithm.collocation_method          = "Legendre";
//    algorithm.mesh_refinement             = "automatic";





////////////////////////////////////////////////////////////////////////////
///////////////////  Now call PSOPT to solve the problem   /////////////////
////////////////////////////////////////////////////////////////////////////

    psopt(solution, problem, algorithm);

    if (solution.error_flag) exit(0);

////////////////////////////////////////////////////////////////////////////
///////////  Extract relevant variables from solution structure   //////////
////////////////////////////////////////////////////////////////////////////

    MatrixXd x 		= solution.get_states_in_phase(1);
    MatrixXd u 		= solution.get_controls_in_phase(1);
    MatrixXd t 		= solution.get_time_in_phase(1);
    MatrixXd H           = solution.get_dual_hamiltonian_in_phase(1);
    MatrixXd lambda      = solution.get_dual_costates_in_phase(1);

////////////////////////////////////////////////////////////////////////////
///////////  Save solution data to files if desired ////////////////////////
////////////////////////////////////////////////////////////////////////////

    Save(x,"x.dat");
    Save(u,"u.dat");
    Save(t,"t.dat");
    Save(lambda,"p.dat");


////////////////////////////////////////////////////////////////////////////
///////////  Plot some results if desired (requires gnuplot) ///////////////
////////////////////////////////////////////////////////////////////////////

    plot(t,x,problem.name + ": states", "time (s)", "states", "x y v");

    plot(t,u,problem.name + ": control", "time (s)", "control", "u");

    plot(t,H,problem.name + ": Hamiltonian", "time (s)", "H", "H");

    plot(t,lambda,problem.name + ": costates", "time (s)", "lambda", "lambda_1 lambda_2 lambda_3");

    plot(t,x,problem.name + ": states", "time (s)", "states", "x y v",
                                  "pdf", "brac1_states.pdf");

    plot(t,u,problem.name + ": control", "time (s)", "control", "u",
                              "pdf", "brac1_control.pdf");

    plot(t,H,problem.name + ": Hamiltonian", "time (s)", "H", "H",
                                  "pdf", "brac1_hamiltonian.pdf");

    plot(t,lambda,problem.name + ": costates", "time (s)", "lambda", "lambda_1 lambda_2 lambda_3",
                                  "pdf", "brac1_costates.pdf");



}

////////////////////////////////////////////////////////////////////////////
///////////////////////      END OF FILE     ///////////////////////////////
////////////////////////////////////////////////////////////////////////////


