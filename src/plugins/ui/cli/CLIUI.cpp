#include "CLIUI.h"
#include "CLIDialogBuilder.h"
#include "ImportTask.h"
#include "ExportTask.h"

#include <unistd.h>
#include <string.h>
#include <util/Log.h>
#include <util/File.h>
#include <config/AppConfig.h>
#include <export.h>
#include <pluginmanager/PluginManager.h>

#include <unistd.h>

using namespace GCM;
using namespace GCM::io;
using namespace GCM::config;
using namespace GCM::pluginmanager;

/**
 * Quit the application when the task is done.
 */
class QuitTask: public GCM::ui::BackgroundTask {
	private:
		ptrUI mUI;

	public:
		QuitTask(ptrUI ui): BackgroundTask(), mUI(ui) {}

		virtual void onPreExecute() {
		}

		virtual void doInBackground() {
		}

		virtual void onPostExecute() {
			mUI->quitMainLoop();
		}
};

CLIUI::CLIUI(int argc, char **argv):
	UI(argc, argv), dialogBuilder(new CLIDialogBuilder()),exportFile("") {
}

void CLIUI::parseLine(int argc, char **argv) {
	if (argc > 1 && optind < argc) {
    
    // in main() we process global options (like -d for debug output)
    // here take care of the rest
    const char *op_char = argv[optind];

		if (strncmp(op_char, "import", 6+1) == 0) {
			this->op = OP_IMPORT;
      this->importDB = NULL;

			if (argc - optind > 1) {
				for (int i = optind+1; i < argc; i++) {
					const char *file = argv[i];
          int l = strlen(file);
  			  File dbFile(AppConfig::getInstance()->getDataDirectory());
	   		  dbFile->append(file);
          // test if this is DB file
          if(file[l-4] == '.' && file[l-3] == 'd' && file[l-2] == 'b' && file[l-1] == '3' && dbFile->exists()){
            //use this DB for import
            this->importDB = file;
            continue;
          }

					File fFile(AppConfig::getInstance()->getWorkingDirectory());
					fFile->append(file);
					
					// Search file in application startup directory
					if (fFile->exists()) {
						this->importFile->push_back(fFile->getAbsolutePath());
					} else {
						// Search the file in the data directory.
						fFile = File(file);
						if (fFile->exists()) {
							this->importFile->push_back(fFile->getAbsolutePath());
						} else {
							GCM::util::Log::e("CLI", "File %s was not found.", file);
						}
					}
				}
				
				if (this->importFile->size() == 0) {
					GCM::util::Log::e("CLI", "You must specify at least one file to import.");
					this->op = OP_NOP;
				}
			} else {
				GCM::util::Log::e("CLI", "You must specify at least one file to import.");
				this->op = OP_NOP;
			}
		} else if (strncmp(op_char, "export", 6+1) == 0) {
			this->op = OP_EXPORT;
			if (argc - optind > 1) {
        //file to export to
        File fFile(argv[optind+1]);
        this->exportFile = fFile->getAbsoluteFile();

				this->exportFmt = "pqgpx";
				this->exportDB = NULL;
				this->exportFilter = NULL;

        if(argc - optind > 2)
				  this->exportFmt = argv[optind+2];

        if(argc - optind > 3)
				  this->exportDB = argv[optind+3];

        if(argc - optind > 4)
				  this->exportFilter = argv[optind+4];
        
			} else {
				GCM::util::Log::e("CLI", "You must specify at least file to export to.");
				this->op = OP_NOP;
			}
		} else if (strncmp(op_char, "fmt-list", 8+1) == 0) {
	    PluginManager *pm = PluginManager::Instance();
	
    	PluginManager::PluginFunctionList &funcs = pm->listPlugins("Export");
	    PluginManager::PluginFunctionList::iterator i = funcs.begin();
	
	    while (i != funcs.end()) {
		    fExportInfo infoCall = (fExportInfo)i->second;
		    GC< List<ExportInfo> > info = infoCall();
		
    		List<ExportInfo>::iterator ii = info->begin();
		    while (ii != info->end()) {
			    printf("%s: ", (*ii)->exportId->c_str());
			    printf("%s ", (*ii)->formatDescription->c_str());
			    printf("(.%s)\n", (*ii)->fileExtension->c_str());
			    ii++;
		    }
	    	i++;
      }
		} else if (strncmp(op_char, "version", 6+1) == 0) {
      printf("GCM CLI interface %s\n", GCM_VERSION);
		} else if (strncmp(op_char, "help", 4+1) == 0) {
      printf("GCM CLI interface %s\n\n", GCM_VERSION);
      printf("  usage: gcm-cli [options] command [params]\n");
      printf("\n  options:\n");
      printf("    -d[FILE] --debug[=FILE]         - enable debug output to display (or FILE)\n");
      printf("\n  commands:\n");
      printf("    help                            - print this help info\n");
      printf("    version                         - print version info\n");
      printf("    fmt-list                        - list available export formats\n");
      printf("    import [DB.db3] FILE [FILE]...  - import GPX from FILE(s)\n");
      printf("    export FILE [FORMAT] [DB.db3] [FILTER.xml]\n");
      printf("                                    - export GPX from DB using FILTER to FILE in FORMAT\n");
      printf("\n");
		} else {
			GCM::util::Log::e("CLI", "Unknown operation '%s'. Try: gcm-cli help", op_char);
		}
	} else {
		this->op = OP_NOP;
		GCM::util::Log::e("CLI", "No operation to be performed. Try: gcm-cli help");
	}
}

#include <templater/Templater.h>
using namespace GCM::templater;

void CLIUI::start() {
	GCM::util::Log::d("CLI", "Selected CLI interface.");
	
	/*Templater tpl;
	File f("../src/plugins/ui/qt4/src/res/html/listing.html");
	
	GC<Variable> cache = new Variable();
	cache->assign("Name", "SPORE Cache Brno");
	
	tpl.assign("Cache", cache);
	
	GCM::String out = tpl.eval(f->read());
	GCM::util::Log::d("Templater", out->c_str());*/
	
	this->parseLine(argc, argv);
	
	switch (this->op) {
		case OP_NOP:
			break;

		case OP_IMPORT: {
			StringList::iterator i = this->importFile->begin();
			while (i != this->importFile->end()) {
				String file = *i;
				this->addTask(new ImportTask(file->c_str(), this->importDB, this));
				i++;
			}
			break;
		}
		case OP_EXPORT:
				this->addTask(new ExportTask(this->exportFile, this->exportFmt, this->exportDB, this->exportFilter, this));
			break;
	}

	// After the task has been done, quit.
	this->addTask(new QuitTask(this));

	UI::start();
}

ptrDialogBuilder CLIUI::getDialogBuilder() {
	return dialogBuilder;
}

CLIUI::~CLIUI() {
}

