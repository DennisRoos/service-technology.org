/*****************************************************************************\
 Hello -- <<-- Hello World -->>

 Copyright (c) <<-- 20XX Author1, Author2, ... -->>

 Hello is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Hello is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Hello.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#ifndef WEXITSTATUS
# define WEXITSTATUS(stat_val) ((unsigned int) (stat_val) >> 8)
#endif

/* <<-- CHANGE START (main program) -->> */
// include header files
#include <config.h>
#include <ctime>
#include <libgen.h>
#include <fstream>
#include <sstream>
#include <string>

#include <pnapi/pnapi.h>
#include "config-log.h"
#include "cmdline.h"
#include "Output.h"
#include "verbose.h"
#include "Tara.h"
#include "syntax_graph.h"
#include "MaxCost.h"
#include "ServiceTools.h"
#include "iModification.h"


// input files
extern FILE* graph_in;
extern FILE* costfunction_in;

// the parsers
extern int graph_parse();
extern int graph_lex_destroy();
extern int costfunction_parse();
extern int costfunction_lex_destroy();

using std::cerr;
using std::cout;
using std::endl;
using std::map;
using std::ofstream;

/// the command line parameters
gengetopt_args_info args_info;

/// the invocation string
std::string invocation;

/// a variable holding the time of the call
clock_t start_clock = clock();

/// check if a file exists and can be opened for reading
inline bool fileExists(const std::string& filename) {
    std::ifstream tmp(filename.c_str(), std::ios_base::in);
    return tmp.good();
}


/// evaluate the command line parameters
void evaluateParameters(int argc, char** argv) {
    // overwrite invocation for consistent error messages
    argv[0] = basename(argv[0]);

    // store invocation in a std::string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += std::string(argv[i]) + " ";
    }

    // initialize the parameters structure
    struct cmdline_parser_params* params = cmdline_parser_params_create();

    // call the cmdline parser
    if (cmdline_parser(argc, argv, &args_info) != 0) {
        abort(7, "invalid command-line parameter(s)");
    }

    // debug option
    if (args_info.bug_flag) {
        {
            Output debug_output("bug.log", "configuration information");
            debug_output.stream() << CONFIG_LOG << std::flush;
        }
        message("please send file 'bug.log' to %s!", PACKAGE_BUGREPORT);
        exit(EXIT_SUCCESS);
    }

    // read a configuration file if necessary
    if (args_info.config_given) {
        // initialize the config file parser
        params->initialize = 0;
        params->override = 0;

        // call the config file parser
        if (cmdline_parser_config_file(args_info.config_arg, &args_info, params) != 0) {
            abort(14, "error reading configuration file '%s'", args_info.config_arg);
        } else {
            status("using configuration file '%s'", args_info.config_arg);
        }
    } else {
        // check for configuration files
        std::string conf_generic_filename = std::string(PACKAGE) + ".conf";
        std::string conf_filename = fileExists(conf_generic_filename) ? conf_generic_filename :
                                    (fileExists(std::string(SYSCONFDIR) + "/" + conf_generic_filename) ?
                                     (std::string(SYSCONFDIR) + "/" + conf_generic_filename) : "");

        if (conf_filename != "") {
            // initialize the config file parser
            params->initialize = 0;
            params->override = 0;
            if (cmdline_parser_config_file(const_cast<char*>(conf_filename.c_str()), &args_info, params) != 0) {
                abort(14, "error reading configuration file '%s'", conf_filename.c_str());
            } else {
                status("using configuration file '%s'", conf_filename.c_str());
            }
        } else {
            status("not using a configuration file");
        }
    }


    // check whether at most one file is given
    if (args_info.inputs_num > 1) {
        abort(4, "at most one reachability graph must be given");
    }

    free(params);
}


/// a function collecting calls to organize termination (close files, ...)
void terminationHandler() {
    /* [USER] Add code here */

    // print statistics
    if (args_info.stats_flag) {
        message("runtime: %.2f sec", (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC);
        fprintf(stderr, "%s: memory consumption: ", PACKAGE);
        int i = system((std::string("ps -o rss -o comm | ") + TOOL_GREP + " " + PACKAGE + " | " + TOOL_AWK + " '{ if ($1 > max) max = $1 } END { print max \" KB\" }' 1>&2").c_str());
    }
}


/// main-function
int main(int argc, char** argv) {
    
    time_t start_time, end_time;

    // set the function to call on normal termination
    atexit(terminationHandler);

    /*--------------------------------------.
    | 0. parse the command line parameters  |
    `--------------------------------------*/
    evaluateParameters(argc, argv);
    Output::setTempfileTemplate(args_info.tmpfile_arg);
    Output::setKeepTempfiles(args_info.noClean_flag);

    /*----------------------.
    | 1. parse the open Tara::net |
    `----------------------*/

    // Parsing the open Tara::net, using the PNAPI
	status("Processing %s", args_info.net_arg);
	Tara::net = new pnapi::PetriNet;

	try {

		// parse either from standard input or from a given file
	
		std::ifstream inputStream;
		inputStream.open(args_info.net_arg);
		inputStream >> pnapi::io::owfn >> *(Tara::net);
		inputStream.close();

		/*maybe only with verbose-flag?*/
		std::stringstream pnstats;
		pnstats << pnapi::io::stat << *(Tara::net);

		status("read Tara::net %s", pnstats.str().c_str());
	} catch (pnapi::exception::InputError error) {
		std::stringstream inputerror;
		inputerror << error;
		abort(3, "pnapi error %i", inputerror.str().c_str());
		}

    /*----------------------------------.
    | 2. get most permissive Partner MP |
    `----------------------------------*/

// TODO: renice this temp file stuff, maybe use Output class in an appropiate way?
    std::string tempFN;
     // create a temporary file
#if defined(__MINGW32__)
        tempFN = mktemp(basename(args_info.tmpfile_arg));
#else
        tempFN = mktemp(args_info.tmpfile_arg);
#endif


    std::string partnerTemp=tempFN+"-mp-partner.sa";

    if(!isControllable((*Tara::net), partnerTemp, false) ) {
	    message("Tara::net is not controllable. Exit.");
	    exit(EXIT_FAILURE); // TODO maybe change return status, is this a failure ??
    }
     
    // first create automaton partner
    pnapi::Automaton partner;
    std::ifstream partnerStream;

    //stream automaton
    partnerStream.open(partnerTemp.c_str(), std::ifstream::in);
    if(!partnerStream) { //TODO: is this check necessary ?
	message("Tara::net is not controllable. Exit.");
	exit(EXIT_FAILURE);
    }

    partnerStream >> pnapi::io::sa >> partner;
    
    // convert to petri Tara::net
    pnapi::PetriNet composition(partner);
    
    //and now we compose
    composition.compose(*Tara::net, "mp-partner-", "");

    /*------------------------.
    | 3. call lola with n+mp  |
    `------------------------*/

    message("Step 2: Build the state space of '%s' and its most-permissive partner", args_info.net_arg);    

    // create a temporary file
    std::string lolaFN=tempFN+ "-lola.rg";

    // run lola-statespace from the service tools
    getLolaStatespace(composition,lolaFN);


    //TODO: maybe wrap parsers in classes?
    /*-------------------------------------.
    | 4. Parse Costfunction to partial map |
    \-------------------------------------*/
    message("Step 3: Parse the cost function from '%s' and apply it to the built statespace", args_info.costfunction_arg);    

     status("parsing costfunction");
       // TODO Check if file exists   
     costfunction_in=fopen(args_info.costfunction_arg, "r");
     costfunction_parse(); 
 
    // TODO clean up: destroy lexer, file pointer etc


    /*-------------------------.
    | 5. Parse the inner Graph |
    \-------------------------*/

    graph_in=fopen(lolaFN.c_str(),"r");
    graph_parse();
    /* TODO destroy lexer etc */


    /*------------------------------------------.
    | 5. Compute MaxCosts from the parsed graph | 
    \------------------------------------------*/
    message("Step 4: Find an upper bound for the minimal budget w.r.t. Tara::net '%s' and cost function '%s'", args_info.net_arg, args_info.costfunction_arg);    

    // max Costs are the costs of the most expensive path through
    // the inner state graph
    
    unsigned int maxCostOfComposition=maxCost(Tara::net);


    /*------------------------------------------.
    | 6. Find a corresponding partner           | 
    \------------------------------------------*/

    // Build the modified Tara::net for maxCostOfComposition
    iModification iMod(Tara::net, maxCostOfComposition);

    // Check whether N is controllable under budget maxCostOfComposition. If not, return the most permissive partner.
    std::stringstream ssi;
    ssi << maxCostOfComposition;
    std::string minCostPartner=tempFN+"-min-partner-";  
    std::string curMinCostPartner=minCostPartner+ssi.str()+".sa";
   //std::cout << pnapi::io::owfn << *Tara::net << std::endl;
   //return 0;
    bool bounded = isControllable(*Tara::net, curMinCostPartner, true); 

    // output file of the min cost partner
    std::ifstream minCostPartnerStream;

    // If N is not controllable under budget maxCostofComposition, return the mpp. 
    
    if (!bounded) {
        // Every partner is trivially cost-minimal. Thus, return the mpp
        if (args_info.sa_given) {
		    message("Any partner is cost-minimal, returning the most-permissive partner.");
		    minCostPartnerStream.open(partnerTemp.c_str());
            if (std::string(args_info.sa_arg).compare("-") != 0) {
                std::ofstream outputFile;
                outputFile.open(args_info.sa_arg);
                outputFile << minCostPartnerStream.rdbuf();
                outputFile.close();
            } else {
    		    cout << minCostPartnerStream.rdbuf();
            }
    	}
        if (args_info.og_given) {
            std::string s = "writing operating guidelines to ";
            if (std::string(args_info.og_arg).compare("-") == 0) {
                s += "standard out";             
            } else {
                s += "file '" + std::string(args_info.og_arg) + "'";
            }
		    message("Any partner is cost-minimal, %s.", s.c_str());
            computeOG(*Tara::net, args_info.og_arg);
        }

    } else { // there exists a partner with a bounded budget 
        
        unsigned int minBudget = maxCostOfComposition; // Initially set the minimal budget to the maxCostOfComposition
        
        if (maxCostOfComposition > 0) { // Binary search is only necessary if the upper bound is greater than 0.
            
            status("Step 5: Find the minimal budget with a binary search");

            int bsUpper = maxCostOfComposition-1; // for maxCostofComposition, it is controllable anyway. 
            int bsLower = 0;
            
            while (bsLower <= bsUpper) {
               
                // Set the new budget to the middle of the interval
                iMod.setToValue((bsLower + bsUpper) / 2);
                
                // Prepare the Wendy Call
            	ssi.str("");
	            ssi << iMod.getI();
                curMinCostPartner=minCostPartner+ssi.str()+".sa";

                status("Checking budget %d (lower bound: %d, upper bound: %d)", iMod.getI(), bsLower, bsUpper);             
                bool bsControllable = isControllable(*Tara::net, curMinCostPartner, true);
                if (bsControllable) {
                    minBudget = iMod.getI();        
                    bsUpper = iMod.getI()-1;
                } else {
                    bsLower = iMod.getI()+1;
                }
            }
        
        } 
        
        // Binary search done. The minimal budget is found. Return the partner for the minimal budget.    
        
        message("Minimal budget found: %d", minBudget);


        if (args_info.sa_given) {
		    message("Synthesized a cost-minimal partner. (Costs = %d)", minBudget);
            ssi.str("");
            ssi << minBudget;
            curMinCostPartner=minCostPartner+ssi.str()+".sa";
            minCostPartnerStream.open(curMinCostPartner.c_str());
            if (std::string(args_info.sa_arg).compare("-") != 0) {
    		    message("Writing partner to file '%s'.",args_info.sa_arg);
                std::ofstream outputFile;
                outputFile.open(args_info.sa_arg);
                outputFile << minCostPartnerStream.rdbuf();
                outputFile.close();
            } else {
    		    message("Writing partner to standard out.");
                std::ofstream outputFile;
    		    cout << minCostPartnerStream.rdbuf();
            }
    	}
        if (args_info.og_given) {
            std::string s = "writing operating guidelines to ";
            if (std::string(args_info.og_arg).compare("-") == 0) {
                s += "standard out";             
            } else {
                s += "file '" + std::string(args_info.og_arg) + "'";
            }
		    message("Computing representation of all cost-minimal partners, %s.", s.c_str());
            iMod.setToValue(minBudget);
            computeOG(*Tara::net, args_info.og_arg);
        }

    } 

    // A partner was written to standard out. Let's do the clean up and exit with success.    
    
    // TODO: CLEANUP

    return EXIT_SUCCESS;
}
/* <<-- CHANGE END -->> */
