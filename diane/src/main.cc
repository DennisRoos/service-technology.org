/*****************************************************************************\
 Diane -- Decomposition of Petri nets.

 Copyright (C) 2009  Stephan Mennicke <stephan.mennicke@uni-rostock.de>

 Diane is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Diane is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Diane.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "cmdline.h"
#include "config.h"
#include "config-log.h"
#include "decomposition.h"
#include "pnapi/pnapi.h"
#include "verbose.h"
//#include "recompose.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::map;
using std::ofstream;
using std::setfill;
using std::setw;
using std::string;
using std::stringstream;
using std::vector;
using std::set;
using pnapi::PetriNet;
using pnapi::Place;
using pnapi::Label;
using pnapi::Transition;

string  filename;

/// statistics structure
struct Statistics {
    // runtime
    time_t t_total_;
    time_t t_computingComponents_;
    time_t t_buildingComponents_;

    // net statistics
    int netP_;
    int netT_;
    int netF_;

    // biggest fragment
    int largestNodes_;
    int largestP_;
    int largestT_;
    int largestF_;
    int largestPi_;
    int largestPo_;

    // average fragment
    // each must be devided by #fragments
    int averageP_;
    int averageT_;
    int averageF_;
    int averagePi_;
    int averagePo_;

    // #fragments
    int fragments_;
    // #trivial fragments
    int trivialFragments_;

    // clearing the statistics
    void clear() {
        t_total_ = t_computingComponents_ = t_buildingComponents_ = 0;
        netP_ = netT_ = netF_ = 0;
        largestP_ = largestT_ = largestF_ = largestPi_ = largestPo_ = 0;
        averageP_ = averageT_ = averageF_ = averagePi_ = averagePo_ = 0;
        fragments_ = trivialFragments_ = 0;
        largestNodes_ = 0;
    }
} _statistics;


/// the command line parameters
gengetopt_args_info args_info;

/// evaluate the command line parameters
void evaluateParameters(int, char**);
/// statistical output
void statistics(PetriNet&, vector<PetriNet*> &);

//prints components, their complements and their composition (the structure is computed)
void printComponent(Component c);


int main(int argc, char* argv[]) {
    evaluateParameters(argc, argv);
    // initial clearing
    _statistics.clear();

    if (!args_info.inputs_num) {
    	exit(EXIT_SUCCESS);
    }

    string invocation;
    // store invocation in a string for meta information in file output
    for (int i = 0; i < argc; ++i) {
        invocation += string(argv[i]) + " ";
    }

    time_t start_total = time(NULL);
    /*
     * 1. Reading Petri net via PNAPI
     */
    PetriNet net;
    try {
        ifstream inputfile;
        inputfile.open(args_info.inputs[0]);
        if (inputfile.fail()) {
            abort(1, "could not open file '%s'", args_info.inputs[0]);
        }
        switch (args_info.mode_arg) {
            case(mode_arg_standard): {
                //find out whether it is a .lola or a .owfn (for the final marking)
                std::string s = args_info.inputs[0];
                if (s.find(".owfn") != string::npos) { //compare(s.size()-4,4, "owfn")==0)
                    inputfile >> pnapi::io::owfn >> net;
                } else {
                    inputfile >> pnapi::io::lola >> net;
                }
                break;
            }
            case(mode_arg_freechoice):
                inputfile >> pnapi::io::owfn >> net;
                break;
            default:
                break;
        }
        inputfile.close();
    } catch (pnapi::exception::InputError e) {
        abort(2, "could not parse file '%s': %s", args_info.inputs[0], e.message.c_str());
    }
    status("reading file '%s'", args_info.inputs[0]);
	filename=args_info.inputs[0];
	
    /*
     * 2. Determine the mode and compute the service parts.
     */
    int* tree = NULL;
    int size, psize;
    map<int, Node*> reremap;
    size = net.getNodes().size();
    psize = net.getPlaces().size();
    tree = new int [size];

    time_t start_computing = time(NULL);
    switch (args_info.mode_arg) {
        case(mode_arg_standard):
            _statistics.fragments_ = decomposition::computeComponentsByUnionFind(net,
                                                                                 tree, size, psize, reremap);
            break;
        case(mode_arg_freechoice):
            _statistics.fragments_ = 0;
            break;
        default:
            break; /* do nothing */
    }
    time_t finish_computing = time(NULL); 
    _statistics.t_computingComponents_ = finish_computing - start_computing;
    //status("computed components of net '%s'", args_info.inputs[0]);
	//cout << _statistics.fragments_<<endl;

    /*
     * 3. Build Fragments.
     */
    vector<PetriNet*> nets(size);
    for (int j = 0; j < size; j++) {
        nets[j] = NULL;
    }

    // save start time of building
    time_t start_building = time(NULL);
    switch (args_info.mode_arg) {
        case(mode_arg_standard):
            decomposition::createOpenNetComponentsByUnionFind(nets,net, tree, size, psize, reremap);
			//statistics(net, nets);
            break;
        case(mode_arg_freechoice):
            break;
        default:
            break;
    }
    // save the finish time and make statistics
    time_t finish_building = time(NULL);
    _statistics.t_buildingComponents_ = finish_building - start_building;
    //status("created net fragments of '%s'", args_info.inputs[0]);

	
	
    /*
     * 4. Write output files.
     */
    if (!args_info.quiet_flag && !args_info.compose_flag) {
        // writing file output
		
        string fileprefix;
        string filepostfix = ".owfn";
        if (args_info.prefix_given) {
            fileprefix += args_info.prefix_arg;
        }
        fileprefix += args_info.inputs[0];
        fileprefix += ".part";

        int netnumber = 0;

        int digits;
        string digits_s;
        stringstream ds;
        ds << _statistics.fragments_;
        ds >> digits_s;
        digits = digits_s.length();

        for (int j = 0; j < (int) nets.size(); j++)
            if (nets[j] == NULL) {
                continue;
            } else {
                stringstream ss;
                string num;
                /// leading zeros
                int z = digits - num.length();
                ss /*<< setfill('0') << setw(z)*/ << netnumber;
                ss >> num;
                ofstream outputfile((fileprefix + num + filepostfix).c_str());
                outputfile << pnapi::io::owfn
                           << meta(pnapi::io::INPUTFILE, args_info.inputs[0])
                           << meta(pnapi::io::CREATOR, PACKAGE_STRING)
                           << meta(pnapi::io::INVOCATION, invocation)
                           << *nets[j];
                netnumber++;
				//if complement compute difference between original net and the 
				//respective complement then complement the difference and add the construction
				if (args_info.complement_flag)
				{
					net=addComplementaryPlaces(net);//add complementary places to the origina net
				
					//ofstream outputfile((std::string(argv[0]) + "compl.owfn").c_str());
					//compute the difference for  each place / transition of the bign net delete the one from the second net
					PetriNet diff=net;//delete internal places
					//build the complement 
					PetriNet rest=complementnet(diff, *nets[j]);
					std::ofstream o;
					o.open((std::string(fileprefix+num+filepostfix + ".compl.owfn").c_str()), std::ios_base::trunc);
					//PetriNet net2;
					o << pnapi::io::owfn << rest;
				}
				
            }
    }
	
	/*
	 * Recomposing until two medium sized components are built 
	 */
	if(args_info.compose_flag){
		 statistics(net, nets);
		//composes components until a medium sized component is reached, then computes its complement
		vector<Component> vcp;
		//status("compose %d components until a medium sized component is reached, then computes its complement",  _statistics.fragments_);
		vcp= recompose(nets, net);
			
		if (!args_info.quiet_flag){
			for (int j = 0; j < (int) vcp.size(); j++)
				printComponent(vcp.at(j));
		}
	}
	

    time_t finish_total = time(NULL);
    _statistics.t_total_ = finish_total - start_total;

    /*
     * 5. Make statistical output.
     */
    statistics(net, nets);
	
	

    // memory cleaner
    delete [] tree;
    _statistics.clear();


    exit(EXIT_SUCCESS);
}


/*!
 * Evaluates gengetopt command-line parameters
 */
void evaluateParameters(int argc, char** argv) {
    // set default values
    cmdline_parser_init(&args_info);

    // initialize the parameters structure
    struct cmdline_parser_params* params = cmdline_parser_params_create();

    // call the cmdline parser
    cmdline_parser(argc, argv, &args_info);

    // debug option
    if (args_info.bug_flag) {
        FILE* debug_output = fopen("bug.log", "w");
        fprintf(debug_output, "%s\n", CONFIG_LOG);
        fclose(debug_output);
        fprintf(stderr, "Please send file 'bug.log' to %s.\n", PACKAGE_BUGREPORT);
        exit(EXIT_SUCCESS);
    }

    free(params);
}


/*!
 * Provides statistical output.
 */
void statistics(PetriNet& net, vector<PetriNet*> &nets) {
    if (args_info.statistics_flag) {
        /*
         * 1. Net Statistics
         */
        _statistics.netP_ = (int) net.getPlaces().size() + net.getInterface().getAsynchronousLabels().size();
        _statistics.netT_ = (int) net.getTransitions().size();
        _statistics.netF_ = (int) net.getArcs().size();
        /*
         * 2. Fragment Statistics
         */
        for (int j = 0; j < (int) nets.size(); j++) {
            if (nets[j] == NULL) {
                continue;
            }
            if ((int) nets[j]->getNodes().size() > _statistics.largestNodes_) {
                _statistics.largestNodes_ = nets[j]->getNodes().size() + nets[j]->getInterface().getAsynchronousLabels().size();
                _statistics.largestP_ = nets[j]->getPlaces().size() + nets[j]->getInterface().getAsynchronousLabels().size();
                _statistics.largestT_ = nets[j]->getTransitions().size();
                _statistics.largestF_ = nets[j]->getArcs().size();
                _statistics.largestPi_ = nets[j]->getInterface().getInputLabels().size();
                _statistics.largestPo_ = nets[j]->getInterface().getOutputLabels().size();
            }

            if (nets[j]->getNodes().size() == 1) {
                _statistics.trivialFragments_++;
            }

            _statistics.averageP_ += nets[j]->getPlaces().size() + nets[j]->getInterface().getAsynchronousLabels().size();
            _statistics.averagePi_ += nets[j]->getInterface().getInputLabels().size();
            _statistics.averagePo_ += nets[j]->getInterface().getOutputLabels().size();
            _statistics.averageT_ += nets[j]->getTransitions().size();
            _statistics.averageF_ += nets[j]->getArcs().size();
        }
        /*
         * 3. Statistics Output
         */
        if (!args_info.csv_flag) {
            cout << "*******************************************************" << endl;
            cout << "* " << PACKAGE << ": " << args_info.inputs[0] << endl;
            cout << "* Statistics:" << endl;
            cout << "*******************************************************" << endl;
            cout << "* Petri net Statistics:" << endl;
            cout << "*   |P|= " << (net.getPlaces().size() + net.getInterface().getAsynchronousLabels().size())
                 << " |T|= " << net.getTransitions().size()
                 << " |F|= " << net.getArcs().size() << endl;
            cout << "*******************************************************" << endl;
            cout << "* Number of fragments:         " << _statistics.fragments_ << endl;
            cout << "* Number of trivial fragments: " << _statistics.trivialFragments_ << endl;
            cout << "* Largest fragment:" << endl;
            cout << "*   |P|= " << _statistics.largestP_
                 << " |P_in|= " << _statistics.largestPi_
                 << " |P_out|= " << _statistics.largestPo_
                 << " |T|= " << _statistics.largestT_
                 << " |F|= " << _statistics.largestF_ << endl;
            cout << "* Average fragment:" << endl;
            cout << "*   |P|= " << std::setprecision(2) << (float) _statistics.averageP_ / _statistics.fragments_
                 << " |P_in|= " << std::setprecision(2) << (float) _statistics.averagePi_ / _statistics.fragments_
                 << " |P_out|= " << std::setprecision(2) << (float) _statistics.averagePo_ / _statistics.fragments_
                 << " |T|= " << std::setprecision(2) << (float) _statistics.averageT_ / _statistics.fragments_
                 << " |F|= " << std::setprecision(2) << (float) _statistics.averageF_ / _statistics.fragments_ << endl;
            cout << "*******************************************************" << endl;
            cout << "* Runtime:" << endl;
            cout << "*   Total:     " << _statistics.t_total_ << endl;
            cout << "*   Computing: " << _statistics.t_computingComponents_ << endl;
            cout << "*   Buildung:  " << _statistics.t_buildingComponents_ << endl;
            cout << "*******************************************************"
                 << endl << endl;
        } else {
            /*
             * filename,|P| net,|T| net,|F| net,#fragments,#trivial fragments,
             * |P| lf, |P_in| lf, |P_out| lf, |T| lf, |F| lf,
             * |P| af, |P_in| af, |P_out| af, |T| af, |F| af,
             * total runtime, computing runtime, building runtime
             *
             * where lf = largest fragment, af = average fragment
             */
            cout << args_info.inputs[0] << ","
                 << (net.getPlaces().size() + net.getInterface().getAsynchronousLabels().size()) << ","
                 << net.getTransitions().size() << ","
                 << net.getArcs().size() << ","
                 << _statistics.fragments_ << ","
                 << _statistics.trivialFragments_ << ","
                 << _statistics.largestP_ << ","
                 << _statistics.largestPi_ << ","
                 << _statistics.largestPo_ << ","
                 << _statistics.largestT_ << ","
                 << _statistics.largestF_ << ","
                 << std::setprecision(2) << (float) _statistics.averageP_ / _statistics.fragments_ << ","
                 << std::setprecision(2) << (float) _statistics.averagePi_ / _statistics.fragments_ << ","
                 << std::setprecision(2) << (float) _statistics.averagePo_ / _statistics.fragments_ << ","
                 << std::setprecision(2) << (float) _statistics.averageT_ / _statistics.fragments_ << ","
                 << std::setprecision(2) << (float) _statistics.averageF_ / _statistics.fragments_ << ","
                 << _statistics.t_total_ << ","
                 << _statistics.t_computingComponents_ << ","
                 << _statistics.t_buildingComponents_
                 << endl;
        }
    } else {
        cout << PACKAGE_NAME << ": " << args_info.inputs[0]
             << " - number of components: " << _statistics.fragments_
             << endl;
    }
}

//prints components, their complements and their composition
void printComponent(Component c){
	//status("Printing component:");
	PetriNet cn=makeComponentNet(c);//get the component net first
	//compute its complement and also the composition and print them as well to the output
	std::ofstream oss;
	PetriNet cwithcompl=addComplementaryPlaces(cn);
	PetriNet nnp=addPattern(cwithcompl);
	oss.open((std::string(filename+"_"+c.cname+".owfn").c_str()), std::ios_base::trunc);
	oss << pnapi::io::owfn << nnp;//print it
	cout<<"adding... "<<filename+"_"+c.cname+".owfn"<<endl;

	//nnp
	//compute complement; cmplfordiff
	if (args_info.complement_flag){
		cout<<"adding... "<<filename+"_"+c.cname+".compl.owfn"<<endl;

		PetriNet proccompl=addComplementaryPlaces(c.process);
		PetriNet diff=complementnet(proccompl, cwithcompl);
		//PetriNet xdiff=diff;
		std::ofstream ox;
		ox.open((std::string(filename+"_"+c.cname+".compl.owfn").c_str()), std::ios_base::trunc);
		//ox << pnapi::io::owfn << xdiff;//its complement
		ox << pnapi::io::owfn << diff;
		std::ofstream oc;
		oc.open((std::string(filename+"_"+c.cname + ".composed.owfn").c_str()), std::ios_base::trunc);
		//cout<<"adding... "<<filename+"_"+c.cname+".owfn"<<endl;
		//PetriNet netcompl=cn
		nnp.compose(diff, "1", "2");
		//addinterfcompl adds complementary places to the process to all but interface places
		//and adds patterns  correspondingly
		PetriNet nnnn=addinterfcompl(nnp, c.inputl,c.outputl);
		//cout <<endl << "all size="<<all.size()<<endl;
		oc << pnapi::io::owfn << nnnn;//print the composition
	}
	
	//cout<<"component "<<c.fileprefix+" "+c.cname+" of size "<<c.sizep<<endl;
}
