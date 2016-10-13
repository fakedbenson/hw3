/****************************************************************************
  FileName     [ test.cpp ]
  PackageName  [ test ]
  Synopsis     [ Test program for simple database db ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <cstdlib>
#include "dbTable.h"

using namespace std;

extern DBTable dbtbl;

class CmdParser;
CmdParser* cmdMgr = 0; // for linking purpose

int
main(int argc, char** argv)
{
   if (argc != 2) {  // testdb <cvsfile>
      cerr << "Error: using testdb <cvsfile>!!" << endl;
      exit(-1);
   }

   ifstream inf(argv[1]);

   if (!inf) {
      cerr << "Error: cannot open file \"" << argv[1] << "\"!!\n";
      exit(-1);
   }

   if (dbtbl) {
      cout << "Table is resetting..." << endl;
      dbtbl.reset();
   }
   if (!(inf >> dbtbl)) {
      cerr << "Error in reading csv file!!" << endl;
      exit(-1);
   }

   cout << "========================" << endl;
   cout << " Print table " << endl;
   cout << "========================" << endl;
   cout << dbtbl << endl;

   // TODO
   // Insert what you want to test here by calling DBTable's member functions
   

   /*vector<int> init,init2;
   for(int i = 0; i < 5; i++) {init.push_back(i);init2.push_back(i*i);}
   DBRow InitRow(init), InitRow2(init2);
   dbtbl.addRow(InitRow);
   dbtbl.addRow(InitRow2);
   vector<int> addv,addv2;
   addv.push_back(20);
   addv.push_back(24);
   addv2.push_back(NONE);
   addv2.push_back(-96.5);
   dbtbl.addCol(addv);
   dbtbl.addCol(addv2);
   dbtbl.delCol(3);*/
   vector < int> init,init2;
   init.push_back(3);
   init.push_back(6);
   for(int i = 0; i < 5; i++) if(i !=2 ) init2.push_back(7); else init2.push_back(NONE);
   DBRow InitRow(init);

   dbtbl.addRow(InitRow);
   dbtbl.addCol(init2);

   DBSort s;
   s.pushOrder(1);
   s.pushOrder(0);
   dbtbl.sort(s);
   cout << "test\n" << dbtbl << endl;
   dbtbl.printSummary();

   return 0;
}
