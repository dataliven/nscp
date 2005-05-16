NSC_WRAPPERS_MAIN();
#include <config.h>
#include <strEx.h>

class CheckDisk {
private:

public:
	CheckDisk();
	virtual ~CheckDisk();
	// Module calls
	bool loadModule();
	bool unloadModule();
	std::string getModuleName();
	NSCModuleWrapper::module_version getModuleVersion();
	bool hasCommandHandler();
	bool hasMessageHandler();
	NSCAPI::nagiosReturn handleCommand(const strEx::blindstr command, const unsigned int argLen, char **char_args, std::string &message, std::string &perf);

	// Check commands
	NSCAPI::nagiosReturn CheckFileSize(const unsigned int argLen, char **char_args, std::string &message, std::string &perf);
	NSCAPI::nagiosReturn CheckDriveSize(const unsigned int argLen, char **char_args, std::string &message, std::string &perf);
};