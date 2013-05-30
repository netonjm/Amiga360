#include <xtl.h>
#include <xui.h>
#include <xuiapp.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>

#ifndef MAIN_H
#define MAIN_H

class CAmiga360App : public CXuiModule
{
protected:
    // Override RegisterXuiClasses so that CMyApp can register classes.
    virtual HRESULT RegisterXuiClasses();

    // Override UnregisterXuiClasses so that CMyApp can unregister classes. 
    virtual HRESULT UnregisterXuiClasses();
 	
};

#endif