#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"

#include <algorithm>
#include <vector>
#include <map>

extern int semant_debug;
extern char *curr_filename;


ClassTable *class_table
std::map<Symbol,Class_> class_map;

typedef std::pair<Symbol,Symbol> method_id
std::map<method_id,method_class *> method_env



//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol 
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
}



ClassTable::ClassTable(Classes classes) : semant_errors(0) , error_stream(cerr) {

    /* Fill this in */
    install_basic_classes();

    for(int i=classes->first(); classes->more(i);i=classes->next(i)){
           Class_ cls = classes->nth(i);
           Symbol name = cls->get_name();

           if (class_map.find(name) != class_map.end()){
                sement_error(cls) << "Class redifinition" << name << "." << std::endl;
                return;
           }

           if (name == SELF_TYPE){
                sement_error(cls) << "Class redifinition of basic class SELF_TYPE"<< std::endl;
                return;
           }

        class_map.insert(std::make_pair(name,cls));
    }


    if (class_map.find(Main) != class_map.end()){
        sement_error() << "Main class not defined" << name << "." << std::endl;
        return;
    }


    for (int i = classes->first();classes->more(i);i=classes->next(i)){
        Class_ cls = classes->nth(i);
        Symbol startingClass = cls-> get_name();

        for (Symbol parent = cls->gt_parent();parent != Object; cls = class_map[parent], parent = cls->get_parent()){
            if(class_map.find(parent) == class_map.end()){
                semant_error(cls) << "Parent class " << parent << "undefined." << std::endl;
                return;
            }

            if(parent == Int || parent == Bool || parent == Str || parent == SELF_TYPE){
                semant_error(cls) << "Classes can not inherit from basic classes" << parent << std::endl;
                return;
            }
            if(parent == starting_class){
                semant_error(cls) << "Inheritance cycle encountered"  << std::endl;
                return;
            }
        }
    }

}

void ClassTable::install_basic_classes() {

    // The tree package uses these globals to annotate the classes built below.
   // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");
    
    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.
    
    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    // 
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
	class_(Object, 
	       No_class,
	       append_Features(
			       append_Features(
					       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
					       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			       single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	       filename);

    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class = 
	class_(IO, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       single_Features(method(out_string, single_Formals(formal(arg, Str)),
										      SELF_TYPE, no_expr())),
							       single_Features(method(out_int, single_Formals(formal(arg, Int)),
										      SELF_TYPE, no_expr()))),
					       single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
			       single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	       filename);  

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    Class_ Int_class =
	class_(Int, 
	       Object,
	       single_Features(attr(val, prim_slot, no_expr())),
	       filename);

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    Class_ Str_class =
	class_(Str, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       append_Features(
									       single_Features(attr(val, Int, no_expr())),
									       single_Features(attr(str_field, prim_slot, no_expr()))),
							       single_Features(method(length, nil_Formals(), Int, no_expr()))),
					       single_Features(method(concat, 
								      single_Formals(formal(arg, Str)),
								      Str, 
								      no_expr()))),
			       single_Features(method(substr, 
						      append_Formals(single_Formals(formal(arg, Int)), 
								     single_Formals(formal(arg2, Int))),
						      Str, 
						      no_expr()))),
	       filename);

    class_map.insert(std::make_pair(Object, Object_Class));
    class_map.insert(std::make_pair(IO,IO_Class));
    class_map.insert(std::make_pair(Int,Int_Class));
    class_map.insert(std::make_pair(Bool,Bool_Class));
    class_map.insert(std::make_pair(Str,Str_Class));
}






////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()                
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)  
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{                                                             
    return semant_error(c->get_filename(),c);
}    

ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
{
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error()                  
{                                                 
    semant_errors++;                            
    return error_stream;
} 

/////////////////////////////////////////////////////////////////////////////

/*
 * The method returns pointer to method with same name, if no method defined for the given class
 *
 * will not check for the siperclasses of the provided class
 *
 * */


method_class *method_in_cls(Symbol class_name, Symbol method_name){
    auto iter = method_env.find(std::make_pair(class_name, method_name));
    if(iter == method_env.end()){
        return nullptr;
    }

    return iter-> second;
}



/*
 *
 * Get interface of global method environment
 *
 * returns result of M(C,f).
 * */

method_class *lookup_method(Symbol class_name, Symbol method_name){
    for(auto c_iter = class_map.find(class_name);
        c_iter != class_map.end();
        c_iter = class_map.find(c_iter->second->get_parent())){
    
        method_class *method = method_in_cls(c_iter -> second -> get_name(),method_name);
        if(method){
            return method
        }
    }

    return nullptr;
}


bool cls_is_defined(Symbol cls_name){
    if(cls_name == SELF_TYPE){
	return true;
    }

    if (class_map.find(cls_name) == class_map.end()){
	return false;
    }

    return true;
}


/*
 * Return true if sub us a subclass of a superclasss
 *
 * */

bool is_subclass(Symbol sub, Symbol super, type_env &tenv){
	if(sub == SELF_TYPE){
		if(super == SELF_TYPE){
			return true;
		}

		sub = tenv.c -> get_name();
	}

	for (auto c_iter = class_map.find(sub);
	c_iter != class_map.end();
	citer = class_map.find(c_iter-> second-> get_parent()){
		if(c_iter->second->get_name() == super){
			return true;
		}
	}

	return false;
}


/*
 *
 * Returns first common ancestor of a and b classes
 *
 * */

Symbol cls_join(Symbol a, Symbol b, type_env &tenv){
	if(a==SELF_TYPE){
		a = tenv.c->get_name();
	}
	if(b==SELF_TYPE){
		b = tenv.c->get_name();
	}

	Class_ cls = class_map[a];

	for (; !is_subclass(b, cls->get_name(),tenv)); 
	cls = class_map[cls->get_parent()]{}

	return cls-> get_name();
}


//Type Checking Methods


Symbol method_class::typecheck(type_env &tenv){
	tenv.o.enterscope();

	tenv.o.addid(self,new Symbol(SELF_TYPE));

	method_class *m = method_in_cls(tenv.c-> get_name(),name);


	if(this !=m) {
		classtable-> semant_error(tenv.c-> get_filename(),this)<< "Method" << name << " is multiply defined." << std::endl;

	}

	auto c_iter = class_map.find(tenv.c->get_parent());
	if(c-iter != class_map.end()){
		m = lookup_method(c_iter->second->get_name(), name);
		// m now holds the derived version of method (if any)
	}

	bool derived_formals_are_less = false;

	std::vector<Symbol> defined;

	int i;
	for(i = formals-> first(); formals-> more(i); i = formals-> next(i)){
		Formal f  = formals->nth(i);
		Symbol f_name = f->get_name();
		Symbol type_decl = f-> get_type_decl();

		if(f_name == self){
			classtable-> semant_error(tenv.c->get_filename(),this)<< "'self' cannot be the name of a formal parameter." <<std::endl;
		}
		else{
			if(type_decl == SELF_TYPE){
				classtable->semant_error(tenv.c->get_filename(),this)<< "Formal parameter " << f_name << " cannot have type SELF_TYPE." <<std::endl;
			} else if(!cls_is_defined(type_decl)){
				classtable->semant_error(tenv.c->get_filename(),this)<< "Class " <<type_decl << "of formal parameter" << f_name << " is undefined." << std::endl;

			}


			if(std::find(defined.begin(),defined.end(),f_name) == defined.end()){
				defined.push_back(f_name);
			}else{
				classtable->semant_error(tenv.c->get_filename(),this)<< "Formal parameter " << f_name << "is multiply defined. " <<std::endl;
			}

			tenv.o.addid(f->get_name(),new Symbol(type_decl));
		}

		if(m){
			//
		}

	}
}

/////////////////////////////////////////////////////////////////////////////

/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant()
{
    initialize_constants();

    /* ClassTable constructor may do some semantic analysis */
    ClassTable *classtable = new ClassTable(classes);

    /* some semantic analysis code may go here */

    if (classtable->errors()) {
	cerr << "Compilation halted due to static semantic errors." << endl;
	exit(1);
    }
}


