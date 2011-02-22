#include <iostream>
#include <string>
#include <stdexcept>
#include <list>
#include <algorithm>
#include "buildgear/config.h"
#include "buildgear/filesystem.h"
#include "buildgear/debug.h"
#include "buildgear/buildfiles.h"
#include "buildgear/dependency.h"

void CDependency::Resolve(string name, list<CBuildFile*> *buildfiles)
{
   bool found = false;
   list<CBuildFile*>::iterator it, itr;
   
   cout << "Resolving dependencies..." << endl;
   
   /* Traverse buildfiles */
   for (it=buildfiles->begin(); 
        it!=buildfiles->end();
        it++)
   {
      // If matching buildfile found
      if (name == (*it)->name)
      {
         // Resolve target dependency
         CDependency::ResolveDep(*it, &resolved, 
                                      &unresolved);
         found = true;
      }
   }

   if (!found)
   {
      cout << "Error: " << name << " is not found" << endl;
      exit(EXIT_FAILURE);
   }   

   /* Create download order list */
   download_order = resolved;
   download_order.sort();
   download_order.unique();
   
   cout << "Done" << endl << endl;
}

void CDependency::ShowResolved(void)
{
   int i=1;

   list<CBuildFile*>::iterator it;
   
   cout <<  "Download order:" << endl;

   for (it=download_order.begin(); it!=download_order.end(); it++, i++)
   {
      cout << " " << i << ". " << (*it)->name << endl;
   }
   
   cout << endl;
   
   i=1;

   cout <<  "Build order:" << endl;

   for (it=resolved.begin(); it!=resolved.end(); it++, i++)
   {
      cout << " " << i << ". " << (*it)->name << endl;
   }
   cout << endl;
}

void CDependency::ResolveDep(CBuildFile *buildfile,
                             list<CBuildFile*> *resolved,
                             list<CBuildFile*> *unresolved)
{	
	list<CBuildFile*>::iterator it;
	
   debug << buildfile->filename << endl;
   
   unresolved->push_back(buildfile);

	for (it=buildfile->dependency.begin(); 
        it!=buildfile->dependency.end(); it++)
   {
      if(find(resolved->begin(), resolved->end(), *it) == resolved->end())
      {
         if(find(unresolved->begin(), unresolved->end(), *it) != unresolved->end())
         {
            cout << "Error: Circular dependency detected (" 
                 << buildfile->name << " <-> "
                 << (*it)->name << ")" << endl;
            exit(EXIT_FAILURE);
         }
         CDependency::ResolveDep(*it, resolved, unresolved);
      }
   }
	
   resolved->push_back(buildfile);
   unresolved->remove(buildfile);
}
