#include "guilib/GUIKernel.h"
#include "kernel/Kernel.h"
#include "kernel/LinearKernel.h"
#include "kernel/LinearByteKernel.h"
#include "kernel/PolyKernel.h"
#include "kernel/GaussianKernel.h"
#include "lib/io.h"
#include "gui/GUI.h"

#include <string.h>

CGUIKernel::CGUIKernel(CGUI * gui_): gui(gui_)
{
	kernel=NULL;
}

CGUIKernel::~CGUIKernel()
{
	delete kernel;
}

CKernel* CGUIKernel::get_kernel()
{
	return kernel;
}

bool CGUIKernel::set_kernel(char* param)
{
	int size=100;
	char kern_type[1024];
	char data_type[1024];
	param=CIO::skip_spaces(param);
	
	if (sscanf(param, "%s %s %d", kern_type, data_type, &size) >= 2)
	{
		if (strcmp(kern_type,"LINEAR")==0)
		{
			if (strcmp(data_type,"BYTE")==0)
			{
				sscanf(param, "%s %s %d", kern_type, data_type, &size);
				delete kernel;
				kernel=new CLinearByteKernel(size);
				if (kernel)
				{
					CIO::message("LinearByteKernel created\n");
					return true;
				}
			}
			else if (strcmp(data_type,"REAL")==0)
			{
				sscanf(param, "%s %s %d", kern_type, data_type, &size);
				delete kernel;
				kernel=new CLinearKernel(size);
				return true;
			}
		}
		else if (strcmp(kern_type,"POLY")==0)
		{
			if (strcmp(data_type,"REAL")==0)
			{
				int inhomogene=0;
				int degree=2;

				sscanf(param, "%s %s %d %d %d", kern_type, data_type, &degree, &inhomogene, &size);
				delete kernel;
				kernel=new CPolyKernel(size, degree, inhomogene==1);

				if (kernel)
				{
					CIO::message("Polynomial Kernel created\n");
					return true;
				}
			}
		}
		else if (strcmp(kern_type,"GAUSSIAN")==0)
		{
			if (strcmp(data_type,"REAL")==0)
			{
				double width=1;

				sscanf(param, "%s %s %lf %d", kern_type, data_type, &width, &size);
				delete kernel;
				kernel=new CGaussianKernel(size, width);
				if (kernel)
				{
					CIO::message("Gaussian Kernel created\n");
					return true;
				}
			}
		}
		else 
			CIO::not_implemented();
	}
	else 
		CIO::message("see help for params!\n");

	CIO::not_implemented();
	return false;
}

bool CGUIKernel::load_kernel_init(char* param)
{
	bool result=false;
	char filename[1024];

	if (kernel)
	{
		if ((sscanf(param, "%s", filename))==1)
		{
			FILE* file=fopen(filename, "r");
			if ((!file) || (!kernel->load_init(file)))
				CIO::message("reading from file %s failed!\n", filename);
			else
			{
				CIO::message("successfully read kernel init data from \"%s\" !\n", filename);
				result=true;
			}

			if (file)
				fclose(file);
		}
		else
			CIO::message("see help for params\n");
	}
	else
		CIO::message("no kernel set!\n");
	return result;
}

bool CGUIKernel::save_kernel_init(char* param)
{
	bool result=false;
	char filename[1024];

	if (kernel)
	{
		if ((sscanf(param, "%s", filename))==1)
		{
			FILE* file=fopen(filename, "w");
			if (!file)
				CIO::message("fname: %s\n", filename);
			if ((!file) || (!kernel->save_init(file)))
				CIO::message("writing to file %s failed!\n", filename);
			else
			{
				CIO::message("successfully written kernel init data into \"%s\" !\n", filename);
				result=true;
			}

			if (file)
				fclose(file);
		}
		else
			CIO::message("see help for params\n");
	}
	else
		CIO::message("no kernel set!\n");
	return result;
}

bool CGUIKernel::init_kernel(char* param)
{
	char target[1024];
	bool do_init=false;

	if ((sscanf(param, "%s", target))==1)
	{
		if (!strncmp(target, "TRAIN", 5))
		{
			do_init=true;
			if (gui->guifeatures.get_train_features())
			{
				if (!kernel->check_features(gui->guifeatures.get_train_features()))
				{
					CIO::message("kernel can not process this feature type\n");
					return false ;
				}
				kernel->init(gui->guifeatures.get_train_features(),gui->guifeatures.get_train_features(), do_init) ;
			}
			else
				CIO::message("assign train features first\n");
		}
		else if (!strncmp(target, "TEST", 5))
		{
			if (gui->guifeatures.get_train_features() && gui->guifeatures.get_test_features())
			{
				if (!kernel->check_features(gui->guifeatures.get_train_features()) && !kernel->check_features(gui->guifeatures.get_test_features()))
				{
					CIO::message("kernel can not process this feature type\n");
					return false ;
				}
				
				CIO::message("initialising kernel with TEST DATA, train: %d test %d\n",gui->guifeatures.get_train_features(), gui->guifeatures.get_test_features() );

				// lhs -> always train_features; rhs -> alway test_features
				kernel->init(gui->guifeatures.get_train_features(), gui->guifeatures.get_test_features(), do_init) ;
			}
			else
				CIO::message("assign train and test features first\n");

		}
		else
			CIO::not_implemented();
	}
	else 
		CIO::message("see help for params\n");
	return true;
}

bool CGUIKernel::save_kernel(char* param)
{
	bool result=false;
	char filename[1024];

	if (kernel)
	{
		if ((sscanf(param, "%s", filename))==1)
		{
			if (!kernel->save(filename))
				CIO::message("writing to file %s failed!\n", filename);
			else
			{
				CIO::message("successfully written kernel to \"%s\" !\n", filename);
				result=true;
			}
		}
		else
			CIO::message("see help for params\n");
	}
	else
		CIO::message("no kernel set!\n");
	return result;
}
