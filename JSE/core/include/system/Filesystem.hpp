#ifndef JSE_FILESYSTEM_H
#define JSE_FILESYSTEM_H

#include <filesystem>

#include "system/SystemTypes.hpp"

namespace jse {

	class FileSystem
	{
	public:
		FileSystem();
		void SetWorkingDir(String const&);
		const String GetWorkingDir() const;
		const String Resolve(const String&) const;
		bool ReadTextFile(const String&, String&);
		bool ReadTextFileBase(const String&, String&);
		ByteVector ReadBinaryFile(const String& aFileName);
	private:
		std::filesystem::path mWorkingPath;
	};
}
#endif

