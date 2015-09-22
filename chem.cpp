
#include "util.h"
#include "core.h"
#include "types.h"


#include <stdio.h>
#include <conio.h>

#include <list>
#include <iostream>
#include <map>
#include <algorithm>
#include <fstream>
#include <set>
#include <strstream>
#include <iomanip>
using namespace std;
extern Trace trace;


const char * help = "\n1. Specify stechiometric reaction equation.\n"
"for inst. H2SO4+Ba(OH)2 = 2H2O + BaSO4.\n"
"2. Enter initial quantities (masses)\n"
"select reactant by specifying corresponding index."
"Masses are specified in [kg], quantities in [kg/mol]\n"
"3. Specify conversion.\n"
"conversion can be set from the interval (-1, 1). "
"Negative values indicate that the reaction flows in "
"back order. Default value is '0'\n"
"4. Calculation\n"
"prints out initial and resulting quantities/masses. "
"The context and the data is saved and "
"reaction can be recalculated without "
"reentering all initial data again\n\n"
"Program has no knowledge about structural "
"features of compounds, that is why it can't "
"recognize two compounds with the same stechiometric "
"formula as the same or different reactants and "
"always treats them as different. For inst., if you enter "
"smth. like  \"NaOH+NaOH=NaOH\", the program will treat all the three "
"tokens as different chemical substances and calculate them separately\n"
;

const char* no_equation_specified = "no equation specified";

extern context ctx;
extern ChemicalTable table;
extern unit_manager unit_mgr;


extern int code;



class ShellOption{
public:
	ShellOption(char act_symbol){
		this->action_symbol = act_symbol;
	}

	virtual char* title() = 0;

	virtual void callback() = 0;

	char get_action_symbol(){
		return action_symbol;
	}



protected:

	char action_symbol;


};


 //////////////////////////// //////////////////////////// ////////////////
 //////////////////////////// SHELL OPTIONS ///////////////////////////////
 //////////////////////////// //////////////////////////// ////////////////



class ShellOptionOne : public ShellOption{
public: 
	ShellOptionOne(): ShellOption('1'){

	}

	virtual char* title(){
		return "1. Enter chemical reaction";
	}

	virtual void callback(){
		cout << endl;
		cout << "enter chemical reaction: "  << endl;
		char input[255];
		gets_s(input);


		
		analize(input);

		if(ctx.equation_good){
			
			cout << "equation is OK\n" <<endl;
		}
		else{
			cout<< "equation invalid!" <<endl;	
		}

		cout << endl;

		
		

	}


};

class ShellOptionExit : public ShellOption{
public: 
	ShellOptionExit() :ShellOption('9'){

	}

	virtual char* title(){
		return "9. Exit";
	}

	virtual void callback(){
		cout << "Goodbye!"  << endl;
		code = 0;

	}


};

class ShellOptionQuant : public ShellOption{
public: 
	ShellOptionQuant() :ShellOption('2'){

	}

	virtual char* title(){
		return "2. Enter quantities";
	}

	virtual void callback(){
		cout << endl;
		if(!ctx.equation_good){
			cout << no_equation_specified << '\n' << endl;
			return;

		}

		cout << "enter quantities ("<<  unit_mgr.get_quantity_unit()->name  << "): " <<endl;
		enter_quantities();
		
		cout << endl;
	}


};

class ShellOptionMass : public ShellOption{
public: 
	ShellOptionMass() :ShellOption('3'){

	}

	virtual char* title(){
		return "3. Enter masses";
	}

	virtual void callback(){
		cout << endl;
		if(!ctx.equation_good){
			cout << no_equation_specified << '\n' << endl;
			return;

		}

		cout << "enter masses (" << unit_mgr.get_mass_unit()->name <<   "): " <<endl;
		enter_masses();
		
		cout << endl;
		

	}


};

class ShellOptionTest : public ShellOption{
public: 
	ShellOptionTest() :ShellOption('6'){

	}

	virtual char* title(){
		return "6. Element lookup";
	}

	virtual void callback(){
		
		cout << "enter element name (-1 to stop): " <<endl;

		while(1){
		 char buf[10];
		 gets_s(buf);
		 if(strcmp(buf, "-1")==0)
			return;

		 Element* el = table.resolve(buf);
		 if(el){
			//cout << "entry found : ";
			print_out_el(el);
		 }
		 else{
		  //cout << "entry not found" << endl;
		 }
			
		}
		

	}


};

class ShellOptionDump : public ShellOption{
public: 
	ShellOptionDump () :ShellOption('7'){

	}

	virtual char* title(){
		return "7. Print table";
	}

	virtual void callback(){

		cout << endl;
		table.dump();
		cout << endl;
		

	}


};

class ShellOptionConversion : public ShellOption{
public: 
	ShellOptionConversion () :ShellOption('4'){

	}

	virtual char* title(){
		return "4. Enter conversion";
	}

	virtual void callback(){
		cout << endl;
		cout << "enter conversion : ";

		char buf[20];
		gets_s(buf);

		double value = atof(buf);
		if(value>1 || value <-1){
			cout << "invalid argument" <<endl;
			return;
		}
		
		ctx.conversion = value;
		cout << "OK\n" << endl;				

	}


};


class ShellOptionCalculate : public ShellOption{
public: 
	ShellOptionCalculate () :ShellOption('5'){

	}

	virtual char* title(){
		return "5. Calculate";
	}

	virtual void callback(){
		cout << endl;
		if(!ctx.equation_good){
			cout << no_equation_specified << '\n' << endl;
			return;
		}

		prepare();
		cout << "Initial : " << endl;
		print_reactants();
	
		calculate();	
		cout << "Result : " << endl;
		print_reactants();
		
		cout << "Conversion : " << ctx.conversion << endl;

		cout << endl;

	}


};

class ShellOptionHelp : public ShellOption{
public: 
	ShellOptionHelp () :ShellOption('0'){

	}

	virtual char* title(){
		return "0. Help";
	}

	virtual void callback(){
		cout << help << endl;		

	}


};

class ShellOptionUnits : public ShellOption{
public: 
	ShellOptionUnits () :ShellOption('8'){

	}

	virtual char* title(){
		return "8. Calculation units";
	}

	virtual void callback(){
			
		cout <<"\nset calculation units:\n" ;

		set_units();
		cout << endl;
			
	
	}


};

 //////////////////////////// //////////////////////////// ////////////////
 ////////////////////////////  //////////// ///////////////////////////////

list<ShellOption*> options;


void register_options(){
	options.push_back(new ShellOptionOne);
	options.push_back(new ShellOptionQuant);
	options.push_back(new ShellOptionMass); 
	options.push_back(new ShellOptionConversion);
	options.push_back(new ShellOptionCalculate);

	options.push_back(new ShellOptionTest);
	options.push_back(new ShellOptionDump);
	options.push_back(new ShellOptionUnits);

	options.push_back(new ShellOptionExit);
	options.push_back(new ShellOptionHelp);
}




void shutdown(){

 for(list<ShellOption*>::iterator it = options.begin() ; it!=options.end() ; it++){
  delete (*it);
 }

 ctx.drop();

}

int main(int argc, char* argv[])
{
	const char * chemicalTableFileName = (argc == 2) ? argv[1] : "table.txt";

 cout << setprecision(5);
 register_options();
 populate_table(chemicalTableFileName);

 

 if(code!=-1)
	goto end;

 while(1){

 	for(list<ShellOption*>::iterator it = options.begin() ; it!=options.end() ; it++){
     
  	cout << (*it)->title() << endl;
 }

 while(1){
  char input;
  input = _getch();
  for(list<ShellOption*>::iterator it = options.begin() ; it!=options.end() ; it++){
     
    if((*it)->get_action_symbol()==input){
	
	(*it)->callback();
	if(code!=-1)
		goto end;

	goto mark;
    }


  }
   
 }

 mark:;

}

end:;


 shutdown();
 return code;
}