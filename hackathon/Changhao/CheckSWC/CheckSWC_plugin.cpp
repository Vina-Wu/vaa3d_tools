/* CheckSWC_plugin.cpp
 * This is a test plugin, you can use it as a demo.
 * 2019-11-18 : by YourName
 */
 
#include "v3d_message.h"
#include <vector>
#include "CheckSWC_plugin.h"

using namespace std;
Q_EXPORT_PLUGIN2(CheckSWC, TestPlugin);
 
QStringList TestPlugin::menulist() const
{
	return QStringList() 
        <<tr("CheckSWC_plugin")
		<<tr("about");
}

QStringList TestPlugin::funclist() const
{
	return QStringList()
		<<tr("func1")
		<<tr("func2")
		<<tr("help");
}

void TestPlugin::domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent)
{
    if (menu_name == tr("CheckSWC_plugin"))
    {
        int flag = CheckSWC(callback,parent);
        if (flag == 1)
        {
            cout<<endl;
            cout<<"********************"<<endl;
            cout<<"*CheckSWC finished!*"<<endl;
            cout<<"********************"<<endl;
        }
        else
        {   v3d_msg("menu flag != 1"); return;}
    }
	else
	{
		v3d_msg(tr("This is a test plugin, you can use it as a demo.. "
			"Developed by YourName, 2019-11-18"));
	}
}

bool TestPlugin::dofunc(const QString & func_name, const V3DPluginArgList & input, V3DPluginArgList & output, V3DPluginCallback2 & callback,  QWidget * parent)
{
	vector<char*> infiles, inparas, outfiles;
	if(input.size() >= 1) infiles = *((vector<char*> *)input.at(0).p);
	if(input.size() >= 2) inparas = *((vector<char*> *)input.at(1).p);
	if(output.size() >= 1) outfiles = *((vector<char*> *)output.at(0).p);

	if (func_name == tr("func1"))
	{
		v3d_msg("To be implemented.");
	}
	else if (func_name == tr("func2"))
	{
		v3d_msg("To be implemented.");
	}
	else if (func_name == tr("help"))
	{
		v3d_msg("To be implemented.");
	}
	else return false;

	return true;
}

